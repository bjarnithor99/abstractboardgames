#!/usr/bin/env python3

# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2022 Bjarni Dagur Thor Karason <bjarni@bjarnithor.com>

import logging
import numpy as np
import torch
from torch import nn
from torchsummary import summary
from .model import Model
from ..agent import Agent
from ..mcts import MCTS


@Agent.register
class BreakthroughAgent:
    def __init__(self, sample_input):
        self.device = (
            torch.device("cuda") if torch.cuda.is_available() else torch.device("cpu")
        )
        self.model = Model(sample_input, self.device)
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
                self.save_checkpoint("./best_mcts.pth")

        self.load_checkpoint("./best_mcts.pth")

    def get_move(self, env, temperature=0.0):
        mcts = MCTS(env, self.model)
        root = mcts.run(n_simulations=50)
        return root.select_move(temperature)

    def save_checkpoint(self, checkpoint_path):
        torch.save(self.model.state_dict(), checkpoint_path)

    def load_checkpoint(self, checkpoint_path):
        self.model.load_state_dict(torch.load(checkpoint_path))
