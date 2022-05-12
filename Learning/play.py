#!/usr/bin/env python3

# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2022 Bjarni Dagur Thor Karason <bjarni@bjarnithor.com>

import logging
import coloredlogs
import torch
from tqdm import tqdm
from ..Agents.alphabeta_agent import AlphaBetaAgent
from ..Agents.browser_gui_agent import BrowserGUIAgent
from ..Agents.mcts_agent import MCTSAgent
from ..Agents.random_agent import RandomAgent
from ..cpp.python_bindings import python_bindings
from ..GameEngine.CPPGameEngine import CPPGameEngine


if __name__ == "__main__":
    log = logging.getLogger(__name__)
    coloredlogs.install(level="INFO")

    log.info("Creating game engine")
    engine = CPPGameEngine("/home/bjarni/HR/T-404-LOKA/cpp/games/tictactoe/tictactoe.game")

    log.info("Creating agent")
    sample_input = torch.tensor(engine.get_environment_representation(), dtype=torch.float)
    agent = MCTSAgent(sample_input, 1000)
    agent.load_checkpoint("/home/bjarni/HR/checkpoints/best_mcts_tictactoe.pth")

    browser_agent = BrowserGUIAgent(port=8002)

    players = [browser_agent, agent]
    while True:
        log.info("New game")
        engine.reset()
        move_number = 0
        while not engine.game_over():
            move = players[move_number % 2].get_move(engine)
            if move is not None:
                engine.execute_move(move)
                move_number += 1
        if engine.get_white_score() == 1:
            log.info("White wins")
        elif engine.get_white_score() == 0:
            log.info("Draw")
        elif engine.get_white_score() == -1:
            log.info("Black wins")
        else:
            log.info(f"Unexpected score {engine.get_white_score()}")
