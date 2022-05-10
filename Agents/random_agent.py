#!/usr/bin/env python3

# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2022 Bjarni Dagur Thor Karason <bjarni@bjarnithor.com>

import numpy as np
from .agent import Agent


# An agent that plays randomly.
@Agent.register
class RandomAgent(Agent):
    def __init__(self):
        pass

    def get_move(self, env):
        available_moves = env.generate_moves()

        if not available_moves:
            return None

        move_idx = np.random.choice(len(available_moves))
        return available_moves[move_idx]
