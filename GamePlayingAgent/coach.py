#!/usr/bin/env python3

# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2022 Bjarni Dagur Thor Karason <bjarni@bjarnithor.com>

import logging
import copy
from tqdm import tqdm
from .tournament import Tournament


class Coach:
    def __init__(self, env, agent):
        self.log = logging.getLogger(__name__)
        self.env = env
        self.training_agent = agent
        self.old_agent = copy.deepcopy(self.training_agent)
        self.ITERATION_STORE_CAPACITY = 3
        self.iteration_store = []

    def generate_episode(self):
        self.env.reset()
        states = []
        states.append(
            (self.env.get_environment_representation(), self.env.current_player)
        )
        while not self.env.game_over():
            # move = self.training_agent.get_move(self.env, random_prop=0.20)
            move = self.training_agent.get_move(self.env, temperature=1)
            if move is not None:
                self.env.execute_move(move)
                states.append(
                    (self.env.get_environment_representation(), self.env.current_player)
                )
        score = self.env.white_score()
        return [
            (state[0], [score * ((-1) ** (self.env.first_player() != state[1]))])
            for state in states
        ]

    def train_agent(self, n_iterations, episodes_per_iteration):
        for iteration in range(n_iterations):
            self.log.info("Iteration %d/%d", iteration + 1, n_iterations)

            samples = []
            for _ in tqdm(range(episodes_per_iteration), desc="Generating episodes"):
                samples += self.generate_episode()
            self.iteration_store.append(samples)

            if len(self.iteration_store) > self.ITERATION_STORE_CAPACITY:
                self.iteration_store.pop(0)

            training_samples = []
            for iteration_samples in self.iteration_store:
                training_samples.extend(iteration_samples)

            self.training_agent.save_checkpoint("./before_training.pth")
            # self.old_agent.load_checkpoint("./before_training.pth")
            self.training_agent.train(training_samples, n_epochs=25)

            self.log.info("Starting tournament between new agent and old agent")
            tournament = Tournament(self.env, self.training_agent, self.old_agent)
            (
                new_white_wins,
                new_black_wins,
                draws,
                old_white_wins,
                old_black_wins,
            ) = tournament.play(2)

            print(f"{new_white_wins=}")
            print(f"{new_black_wins=}")
            print(f"{draws=}")
            print(f"{old_white_wins=}")
            print(f"{old_black_wins=}")
