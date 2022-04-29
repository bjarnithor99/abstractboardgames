#!/usr/bin/env python3

# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2022 Bjarni Dagur Thor Karason <bjarni@bjarnithor.com>

import logging
import coloredlogs
import torch
from tqdm import tqdm
import json

# from .breakthrough.breakthrough_agent_alphabeta import BreakthroughAgent
from .breakthrough.breakthrough_agent_mcts import BreakthroughAgent
from .tui_agent import TUIAgent
from .random_agent import RandomAgent
from .coach import Coach
from ..PythonParser.NewFolderStruct.Agents import BrowserGUIAgent
from ..cpp.python_bindings import python_bindings

if __name__ == "__main__":
    log = logging.getLogger(__name__)
    coloredlogs.install(level="INFO")

    log.info("Parsing game")
    parser = python_bindings.Parser(
        "/home/bjarni/HR/T-404-LOKA/cpp/games/breakthrough_small.game"
    )
    parser.parse()
    env = parser.get_environment()

    log.info("Creating agent and coach")
    sample_input = torch.tensor(env.get_environment_representation(), dtype=torch.float)
    agent = BreakthroughAgent(sample_input)
    agent.load_checkpoint("/home/bjarni/HR/T-404-LOKA/GamePlayingAgent/best_mcts.pth")
    # coach = Coach(env, agent)

    # log.info("Beginning training")
    # coach.train_agent(10, 20)

    browser_agent = BrowserGUIAgent(port=8002)
    browser_agent.get_move = browser_agent.getMove
    # players = [browser_agent, agent]
    players = [agent, browser_agent]

    # players = [agent, RandomAgent()]
    # players = [TUIAgent(), agent]
    # players = [TUIAgent(), agent]
    # players = [agent, TUIAgent()]
    win_cnt = 0
    loss_cnt = 0
    draw_cnt = 0
    # for _ in tqdm(range(50)):
    while True:
        env.reset()
        move_count = 0
        while not env.game_over():
            print(f"\n\n\n{env.current_player}'s move")
            env.print()

            move = players[move_count % 2].get_move(env)
            if move is not None:
                env.execute_move(move)
                move_count += 1
        if env.white_score() == 1:
            print("White win")
            print("Black loss")
            win_cnt += 1
        elif env.white_score() == 0:
            print("Draw")
            draw_cnt += 1
        elif env.white_score() == -1:
            print("Black win")
            print("White loss")
            loss_cnt += 1
    print(win_cnt, draw_cnt, loss_cnt)
