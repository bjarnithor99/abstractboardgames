#!/usr/bin/env python3

# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2022 Bjarni Dagur Thor Karason <bjarni@bjarnithor.com>

from math import sqrt
import os
import logging
import numpy as np
from tqdm import tqdm
import torch
from torch import nn
from torchsummary import summary
from .neural_network_model import Model
from .agent import Agent


# A UCT score used by the MCTS agent. Based on AlphaZero's score.
def ucb_score(parent, child):
    prior_score = child.prior * sqrt(parent.visit_count) / (child.visit_count + 1)
    value_score = -child.value() if child.visit_count else 0
    return prior_score + value_score


# A node in a MCTS search tree. Stores the necessary information for an
# AlphaZero-like MCTS agent.
class MCTSNode:
    def __init__(self, prior):
        self.prior = prior
        self.visit_count = 0
        self.value_sum = 0
        self.expanded = False
        self.children = {}
        self.state = None

    def value(self):
        return 0 if not self.visit_count else self.value_sum / self.visit_count

    def select_move(self, temperature):
        visit_counts = np.array([child.visit_count for child in self.children.values()])
        moves = list(self.children.keys())

        if not moves:
            return None

        if temperature == 0:
            move = moves[np.argmax(visit_counts)]
        elif temperature == np.inf:
            move = np.random.choice(moves)
        else:
            visit_count_distribution = visit_counts ** (1 / temperature)
            visit_count_distribution /= sum(visit_count_distribution)
            move_idx = np.random.choice(len(moves), p=visit_count_distribution)
            move = moves[move_idx]

        return move

    def select_child(self):
        best_score = -np.inf
        best_move = None
        best_child = None

        for move, child in self.children.items():
            score = ucb_score(self, child)
            if score > best_score:
                best_score = score
                best_move = move
                best_child = child

        return best_move, best_child

    def expand(self, env):
        moves = env.generate_moves()
        for move in moves:
            self.children[tuple(move)] = MCTSNode(prior=1 / len(moves))
        self.state = env.get_environment_representation()
        self.expanded = True

    def __str__(self):
        ret = f"Visit count: {self.visit_count}, value sum: {self.value_sum}, #children: {len(self.children)}"
        return ret


# A wrapper around a MCTS search tree.
class MCTS:
    def __init__(self, env, model):
        self.env = env
        self.model = model

    def backpropagate(self, path, score):
        for node in reversed(path):
            node.value_sum += score
            score *= -1
            node.visit_count += 1

    def run(self, n_simulations):
        root = MCTSNode(prior=0)
        root.expand(self.env)

        for _ in range(n_simulations):
            node = root
            search_path = [node]

            while node is not None and node.expanded:
                move, node = node.select_child()
                if move is not None:
                    search_path.append(node)
                    self.env.execute_move(move)

            if not self.env.game_over() and node is not None:
                node.expand(self.env)

            score = self.model.predict(self.env.get_environment_representation())

            self.backpropagate(search_path, score)

            while search_path[-1] != root:
                self.env.undo_move()
                search_path.pop()

        return root


# A simple AlphaZero-like MCTS agent.
@Agent.register
class MCTSAgent(Agent):
    def __init__(self, sample_input, n_simulations=50):
        self.device = (
            torch.device("cuda") if torch.cuda.is_available() else torch.device("cpu")
        )
        self.model = Model(sample_input, self.device)
        print(summary(self.model, sample_input.shape))
        self.log = logging.getLogger(__name__)
        self.BATCH_SIZE = 32
        self.LEARNING_RATE = 0.0002
        self.criterion = nn.MSELoss()
        self.N_SIMULATIONS = n_simulations

    def process_samples_to_dataloader(self, samples):
        X, y = map(lambda x: torch.tensor(x, dtype=torch.float), list(zip(*samples)))
        dataset = torch.utils.data.TensorDataset(X, y)
        dataloader = torch.utils.data.DataLoader(
            dataset, batch_size=self.BATCH_SIZE, shuffle=True
        )
        return dataloader

    def train(self, samples, n_epochs):
        self.log.info("Training on %d sample(s)", len(samples))
        dataloader = self.process_samples_to_dataloader(samples)
        optimizer = torch.optim.Adam(self.model.parameters(), lr=self.LEARNING_RATE)

        best_loss = np.inf
        for epoch in range(n_epochs):
            self.model.train()
            epoch_loss = 0.0
            for data in dataloader:
                X, y = data[0].to(self.device), data[1].to(self.device)
                optimizer.zero_grad()
                output = self.model(X)
                loss = self.criterion(output, y)
                loss.backward()
                optimizer.step()
                epoch_loss += loss.item()
            self.log.info(
                "Epoch %d/%d total loss: %f", epoch + 1, n_epochs, round(epoch_loss, 7)
            )
            if best_loss > epoch_loss:
                best_loss = epoch_loss
                self.save_checkpoint("./checkpoints/best_mcts.pth")

        self.load_checkpoint("./checkpoints/best_mcts.pth")

    def get_move(self, env, temperature=0.0):
        mcts = MCTS(env, self.model)
        root = mcts.run(n_simulations=self.N_SIMULATIONS)
        return root.select_move(temperature)

    def save_checkpoint(self, checkpoint_path):
        if not os.path.exists(os.path.split(checkpoint_path)[0]):
            os.makedirs(os.path.split(checkpoint_path)[0])
        torch.save(self.model.state_dict(), checkpoint_path)

    def load_checkpoint(self, checkpoint_path):
        self.model.load_state_dict(torch.load(checkpoint_path))
