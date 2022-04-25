#!/usr/bin/env python3

# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2022 Bjarni Dagur Thor Karason <bjarni@bjarnithor.com>

import logging
import random
import torch
from torch import nn
from torchsummary import summary
from .model import Model
from agent import Agent


@Agent.register
class BreakthroughAgent:
    def __init__(self, sample_input):
        self.device = (
            torch.device("cuda") if torch.cuda.is_available() else torch.device("cpu")
        )
        self.model = Model(sample_input).to(self.device)
        print(summary(self.model, sample_input.shape))
        self.log = logging.getLogger(__name__)
        self.BATCH_SIZE = 32
        self.LEARNING_RATE = 0.0002
        self.criterion = nn.MSELoss()

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

    def predict(self, state):
        self.model.eval()
        with torch.no_grad():
            v = self.model(
                torch.unsqueeze(torch.tensor(state, dtype=torch.float), dim=0).to(
                    self.device
                )
            )
        return v

    def get_move(self, env, random_prop=0.0):
        available_moves = env.generate_moves()

        if not available_moves:
            return None

        if random.random() < random_prop:
            chosen_move_idx = random.randrange(len(available_moves))
            return available_moves[chosen_move_idx]

        best_move_score, best_move = float("-inf"), None
        for move in available_moves:
            env.execute_move(move)
            move_score = -self.negamax(
                env, 7, float("-inf"), float("inf"), env.current_player == "white"
            )
            # print("move_score", move_score)
            if move_score > best_move_score:
                best_move_score = move_score
                best_move = move
            env.undo_move()
        return best_move

    def negamax(self, env, depth, alpha, beta, max_player):
        if env.variables.game_over or depth == 0:
            board_score = self.predict(env.get_environment_representation()).item()
            return board_score if max_player else -board_score

        found_moves = env.generate_moves()

        if not found_moves:
            env.check_terminal_conditions()
            board_score = self.predict(env.get_environment_representation()).item()
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
        torch.save(self.model.state_dict(), checkpoint_path)

    def load_checkpoint(self, checkpoint_path):
        self.model.load_state_dict(torch.load(checkpoint_path))
