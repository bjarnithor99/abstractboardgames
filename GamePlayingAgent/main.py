#!/usr/bin/env python3

# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2022 Bjarni Dagur Thor Karason <bjarni@bjarnithor.com>

import logging
import coloredlogs
import torch
from breakthrough.breakthrough_agent import BreakthroughAgent
from tui_agent import TUIAgent
from coach import Coach
import py_interface


if __name__ == "__main__":
    log = logging.getLogger(__name__)
    coloredlogs.install(level="INFO")

    log.info("Parsing game")
    parser = py_interface.Parser("../cpp/games/breakthrough_tiny.game")
    parser.parse()
    env = parser.get_environment()

    log.info("Creating agent and coach")
    sample_input = torch.tensor(env.get_environment_representation(), dtype=torch.float)
    agent = BreakthroughAgent(sample_input)
    coach = Coach(env, agent)

    log.info("Beginning training")
    coach.train_agent(10, 20)

    # players = [agent, TUIAgent()]
    players = [TUIAgent(), agent]
    while True:
        env.reset()
        move_count = 0
        while not env.variables.game_over:
            print(f"\n\n\n{env.current_player}'s move")
            env.print()

            move = players[move_count % 2].get_move(env)
            if move is not None:
                env.execute_move(move)
                move_count += 1
            else:
                env.check_terminal_conditions()
                assert env.variables.game_over
        print("GAME OVER!")
        print("White score", env.variables.white_score)
        print("Black score", env.variables.black_score)
