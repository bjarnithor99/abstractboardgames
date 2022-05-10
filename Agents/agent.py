#!/usr/bin/env python3

# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2022 Bjarni Dagur Thor Karason <bjarni@bjarnithor.com>

from abc import ABCMeta, abstractmethod


# Defines an abstract base class that all agents inherit.
class Agent(metaclass=ABCMeta):
    def __init__(self):
        raise NotImplementedError

    @abstractmethod
    def get_move(self, env):
        raise NotImplementedError

    @abstractmethod
    def train(self, samples):
        raise NotImplementedError

    @abstractmethod
    def predict(self, state):
        raise NotImplementedError

    @abstractmethod
    def save_checkpoint(self, checkpoint_path):
        raise NotImplementedError

    @abstractmethod
    def load_checkpoint(self, checkpoint_path):
        raise NotImplementedError
