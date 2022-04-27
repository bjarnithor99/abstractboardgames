#!/usr/bin/env python3

# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2022 Bjarni Dagur Thor Karason <bjarni@bjarnithor.com>

import torch
from torch import nn


class ConvLayer(nn.Module):
    def __init__(self, in_channels, out_channels, kernel_size, stride, padding):
        super().__init__()

        self.conv = nn.Conv2d(
            in_channels=in_channels,
            out_channels=out_channels,
            kernel_size=kernel_size,
            stride=stride,
            padding=padding,
        )
        self.batchnorm = nn.BatchNorm2d(num_features=out_channels)
        self.relu = nn.ReLU()

    def forward(self, x):
        x = self.conv(x)
        x = self.batchnorm(x)
        x = self.relu(x)
        return x


class ResLayer(nn.Module):
    def __init__(self, in_channels, out_channels, kernel_size, stride, padding):
        super().__init__()

        self.conv1 = nn.Conv2d(
            in_channels=in_channels,
            out_channels=out_channels,
            kernel_size=kernel_size,
            stride=stride,
            padding=padding,
        )
        self.batchnorm1 = nn.BatchNorm2d(num_features=out_channels)

        self.conv2 = nn.Conv2d(
            in_channels=out_channels,
            out_channels=out_channels,
            kernel_size=kernel_size,
            stride=stride,
            padding=padding,
        )
        self.batchnorm2 = nn.BatchNorm2d(num_features=out_channels)

        self.relu = nn.ReLU()

    def forward(self, x):
        identity = x
        x = self.conv1(x)
        x = self.batchnorm1(x)
        x = self.relu(x)
        x = self.conv2(x)
        x = self.batchnorm2(x)
        x += identity
        x = self.relu(x)
        return x


class ValueHead(nn.Module):
    def __init__(self, in_channels, in_features):
        super().__init__()

        self.conv = nn.Conv2d(
            in_channels=in_channels,
            out_channels=1,
            kernel_size=(1, 1),
            stride=(1, 1),
            padding=0,
        )
        self.batchnorm = nn.BatchNorm2d(num_features=1)

        self.fc1 = nn.Linear(in_features=in_features, out_features=64)

        self.fc2 = nn.Linear(in_features=64, out_features=1)

        self.relu = nn.ReLU()
        self.tanh = nn.Tanh()

    def forward(self, x):
        batch_size, _, _, _ = x.shape
        x = self.conv(x)
        x = self.batchnorm(x)
        x = self.relu(x)
        x = x.view(batch_size, -1)
        x = self.fc1(x)
        x = self.relu(x)
        x = self.fc2(x)
        x = self.tanh(x)
        return x


class Model(nn.Module):
    def __init__(self, sample_input, device):
        super().__init__()
        self.device = device
        in_channels, height, width = sample_input.shape
        n_filters = 64
        self.conv = ConvLayer(
            in_channels=in_channels,
            out_channels=n_filters,
            kernel_size=(3, 3),
            stride=(1, 1),
            padding=1,
        )
        self.res1 = ResLayer(
            in_channels=n_filters,
            out_channels=n_filters,
            kernel_size=(3, 3),
            stride=(1, 1),
            padding=1,
        )
        self.res2 = ResLayer(
            in_channels=n_filters,
            out_channels=n_filters,
            kernel_size=(3, 3),
            stride=(1, 1),
            padding=1,
        )

        self.res3 = ResLayer(
            in_channels=n_filters,
            out_channels=n_filters,
            kernel_size=(3, 3),
            stride=(1, 1),
            padding=1,
        )

        self.value = ValueHead(in_channels=n_filters, in_features=height * width)

        self.to(device)

    def forward(self, x):
        x = self.conv(x)
        x = self.res1(x)
        x = self.res2(x)
        x = self.res3(x)
        x = self.value(x)
        return x

    def predict(self, state):
        self.eval()
        with torch.no_grad():
            v = self.forward(
                torch.unsqueeze(torch.tensor(state, dtype=torch.float), dim=0).to(
                    self.device
                )
            )
        return v
