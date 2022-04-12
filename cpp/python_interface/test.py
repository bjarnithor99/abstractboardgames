#!/usr/bin/env python3

# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2022 Bjarni Dagur Thor Karason <bjarni@bjarnithor.com>

import py_interface

def minimax(depth, max_player):
    if env.variables.game_over or depth == 0:
        return env.variables.white_score

    found_moves = env.generate_moves()
    if max_player:
        value = float("-inf")
        for move in found_moves:
            env.execute_move(move)
            value = max(value, minimax(depth - 1, not max_player))
            env.undo_move()
    else:
        value = float("inf")
        for move in found_moves:
            env.execute_move(move)
            value = min(value, minimax(depth - 1, not max_player))
            env.undo_move()
    return value

parser = py_interface.Parser("../games/breakthrough_small.game")
parser.parse()
env = parser.get_environment()

while not env.variables.game_over:
    env.print()

    found_moves = env.generate_moves()
    i = 0
    for move in found_moves:
        print(f"{i})", end=" ")
        for step in move:
            print(f"({step.x}, {step.y}){{{step.side_effect.get_name()}}}", end=" ")
        env.execute_move(move)
        print(" with value", minimax(3, env.current_player == "white"))
        env.undo_move()
        i += 1
    print("u) Undo move")

    done = False
    while not done:
        action = input("Select action: ")
        if action == "u":
            env.undo_move()
            done = True
        else:
            try:
                env.execute_move(found_moves[int(action)])
                done = True
            except ValueError:
                pass

    print("\n\n\n")

env.print()
print("GAME IS OVER!")
print(f"Black score: {env.variables.black_score}")
print(f"White score: {env.variables.white_score}")
