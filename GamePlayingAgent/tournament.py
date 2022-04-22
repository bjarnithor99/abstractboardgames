#!/usr/bin/env python3

# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2022 Bjarni Dagur Thor Karason <bjarni@bjarnithor.com>

from tqdm import tqdm


class Tournament:
    def __init__(self, env, new_agent, old_agent):
        self.env = env
        self.new_agent = new_agent
        self.old_agent = old_agent
        self.RANDOM_PROP = 0.2

    def _play(self, white_agent, black_agent):
        move_count = 0
        players = [white_agent, black_agent]
        self.env.reset()
        while not self.env.variables.game_over:
            move = players[move_count % 2].get_move(self.env)
            if move is not None:
                self.env.execute_move(move)
                move_count += 1
            else:
                self.env.check_terminal_conditions()
                assert self.env.variables.game_over
        return self.env.variables.white_score

    def play(self, n_games):
        new_white_wins = 0
        new_black_wins = 0
        draws = 0
        old_white_wins = 0
        old_black_wins = 0

        for _ in tqdm(range(n_games // 2), desc="new (white) vs old (black)"):
            v = self._play(self.new_agent, self.old_agent)
            if v == 1:
                new_white_wins += 1
            elif v == 0:
                draws += 1
            else:
                old_black_wins += 1

        for _ in tqdm(range(n_games // 2), desc="old (white) vs new (black)"):
            v = self._play(self.old_agent, self.new_agent)
            if v == 1:
                old_white_wins += 1
            elif v == 0:
                draws += 1
            else:
                new_black_wins += 1

        return new_white_wins, new_black_wins, draws, old_white_wins, old_black_wins
