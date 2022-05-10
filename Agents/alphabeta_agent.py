#!/usr/bin/env python3

# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2022 Bjarni Dagur Thor Karason <bjarni@bjarnithor.com>

import os
import logging
import numpy as np
import random
import torch
from torch import nn
from torchsummary import summary
from .neural_network_model import Model
from .agent import Agent


# A simple minimax-based agent with alpha-beta pruning.
@Agent.register
class AlphaBetaAgent:
    def __init__(self, sample_input, depth=4):
        self.device = (
            torch.device("cuda") if torch.cuda.is_available() else torch.device("cpu")
        )
        self.model = Model(sample_input, self.device)
        print(summary(self.model, sample_input.shape))
        self.log = logging.getLogger(__name__)
        self.BATCH_SIZE = 32
        self.LEARNING_RATE = 0.0002
        self.criterion = nn.MSELoss()
        self.DEPTH = depth

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
                self.save_checkpoint("./checkpoints/best_alphabeta.pth")

        self.load_checkpoint("./checkpoints/best_alphabeta.pth")

    def get_move(self, env, temperature=0.0):
        available_moves = env.generate_moves()

        if not available_moves:
            return None

        if random.random() < temperature:
            chosen_move_idx = random.randrange(len(available_moves))
            return available_moves[chosen_move_idx]

        best_move_score, best_move = float("-inf"), None
        for move in available_moves:
            env.execute_move(move)
            move_score = -self.negamax(
                env,
                self.DEPTH - 1,
                float("-inf"),
                float("inf"),
                env.get_current_player() == env.get_first_player(),
            )
            if move_score > best_move_score:
                best_move_score = move_score
                best_move = move
            env.undo_move()
        return best_move

    def negamax(self, env, depth, alpha, beta, max_player):
        if env.game_over() or depth == 0:
            board_score = self.model.predict(
                env.get_environment_representation()
            ).item()
            return board_score if max_player else -board_score

        found_moves = env.generate_moves()

        if not found_moves:
            board_score = self.model.predict(
                env.get_environment_representation()
            ).item()
            return board_score if max_player else -board_score
        else:
            best_score = float("-inf")
            for move in found_moves:
                env.execute_move(move)
                best_score = max(
                    best_score,
                    -self.negamax(env, depth - 1, -beta, -alpha, not max_player),
                )
                env.undo_move()
                alpha = max(alpha, best_score)
                if alpha >= beta:
                    break
            return best_score

    def save_checkpoint(self, checkpoint_path):
        if not os.path.exists(os.path.split(checkpoint_path)[0]):
            os.makedirs(os.path.split(checkpoint_path)[0])
        torch.save(self.model.state_dict(), checkpoint_path)

    def load_checkpoint(self, checkpoint_path):
        self.model.load_state_dict(torch.load(checkpoint_path))
