#!/usr/bin/env python3

# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2022 Bjarni Dagur Thor Karason <bjarni@bjarnithor.com>

import logging
import coloredlogs
import torch
from tqdm import tqdm
from ..Agents.alphabeta_agent import AlphaBetaAgent
from ..Agents.mcts_agent import MCTSAgent
from ..Agents.random_agent import RandomAgent
from .coach import Coach
from ..GameEngine.CPPGameEngine import CPPGameEngine

# A simple tournament to evaluate an agents.
def tournament(engine, players, n_games):
    white_won, draws, black_won = 0, 0, 0

    for _ in tqdm(range(n_games)):
        engine.reset()
        move_count = 0
        while not engine.game_over():
            move = players[move_count % 2].get_move(engine)
            if move is not None:
                engine.execute_move(move)
                move_count += 1

        if engine.get_white_score() == 1:
            white_won += 1
        elif engine.get_white_score() == 0:
            draws += 1
        elif engine.get_white_score() == -1:
            black_won += 1
        else:
            raise ValueError(f"Unexpected white score {env.get_white_score()}")

    return white_won, draws, black_won


# A script to train an agent and evaluate it before and after training. Prints relevant statistics.
if __name__ == "__main__":
    log = logging.getLogger(__name__)
    coloredlogs.install(level="INFO")

    log.info("Creating game engine")
    engine = CPPGameEngine(
        "/home/bjarni/HR/T-404-LOKA/cpp/games/tictactoe/tictactoe.game"
    )

    log.info("Creating agent")
    sample_input = torch.tensor(
        engine.get_environment_representation(), dtype=torch.float
    )
    agent = MCTSAgent(sample_input, 50)
    # agent = AlphaBetaAgent(sample_input, 5)

    log.info("Pre-training evaluation")
    b_w_wins, b_w_draws, b_w_loss = tournament(engine, [agent, RandomAgent()], 50)
    b_b_loss, b_b_draws, b_b_wins = tournament(engine, [RandomAgent(), agent], 50)

    log.info("Beginning training")
    coach = Coach(engine, agent)
    coach.train_agent(10, 20)

    log.info("Post-training evaluation")
    a_w_wins, a_w_draws, a_w_loss = tournament(engine, [agent, RandomAgent()], 50)
    a_b_loss, a_b_draws, a_b_wins = tournament(engine, [RandomAgent(), agent], 50)

    log.info("Results")
    log.info(
        f"Pre-training as white (wins, draw, loss): {b_w_wins} {b_w_draws} {b_w_loss}"
    )
    log.info(
        f"Pre-training as black (wins, draw, loss): {b_b_wins} {b_b_draws} {b_b_loss}"
    )
    log.info(
        f"Post-training as white (wins, draw, loss): {a_w_wins} {a_w_draws} {a_w_loss}"
    )
    log.info(
        f"Post-training as black (wins, draw, loss): {a_b_wins} {a_b_draws} {a_b_loss}"
    )
