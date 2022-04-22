#!/usr/bin/env python3

# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2022 Bjarni Dagur Thor Karason <bjarni@bjarnithor.com>

from abc import ABCMeta, abstractmethod


class Agent(metaclass=ABCMeta):
    def __init__(self):
        pass

    @abstractmethod
    def train(self, samples):
        pass

    @abstractmethod
    def predict(self, state):
        pass

    @abstractmethod
    def save_checkpoint(self, checkpoint_path):
        pass

    @abstractmethod
    def load_checkpoint(self, checkpoint_path):
        pass
