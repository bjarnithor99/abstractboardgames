#!/usr/bin/env python3

# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2022 Bjarni Dagur Thor Karason <bjarni@bjarnithor.com>

from .agent import Agent


@Agent.register
class TUIAgent:
    def __init__(self):
        pass

    def get_move(self, env):
        available_moves = env.generate_moves()
        move_num = 0
        for move in available_moves:
            print(f"{move_num})", end=" ")
            for step in move:
                print(f"({step.x}, {step.y}){{{step.side_effect.get_name()}}}", end=" ")
            print()
            move_num += 1

        chosen_move = None
        if available_moves:
            while chosen_move is None:
                chosen_num = input("Select move number: ")
                try:
                    chosen_move = available_moves[int(chosen_num)]
                except ValueError:
                    pass
        return chosen_move
