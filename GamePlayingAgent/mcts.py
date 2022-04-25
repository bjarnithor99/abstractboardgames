#!/usr/bin/env python3

# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2022 Bjarni Dagur Thor Karason <bjarni@bjarnithor.com>


from math import sqrt
import numpy as np
from tqdm import tqdm


def ucb_score(parent, child):
    prior_score = child.prior * sqrt(parent.visit_count) / (child.visit_count + 1)
    value_score = -child.value() if child.visit_count else 0
    return prior_score + value_score


class MCTSNode:
    def __init__(self, prior):
        self.prior = prior
        self.visit_count = 0
        self.value_sum = 0
        self.expanded = False
        self.children = {}
        self.state = None

    def value(self):
        return 0 if not self.visit_count else self.value_sum / self.visit_count

    def select_move(self, temperature):
        visit_counts = np.array([child.visit_count for child in self.children.values()])
        moves = list(self.children.keys())

        if not moves:
            return None

        if temperature == 0:
            move = moves[np.argmax(visit_counts)]
        elif temperature == np.inf:
            move = np.random.choice(moves)
        else:
            visit_count_distribution = visit_counts ** (1 / temperature)
            visit_count_distribution /= sum(visit_count_distribution)
            move_idx = np.random.choice(len(moves), p=visit_count_distribution)
            move = moves[move_idx]

        return move

    def select_child(self):
        best_score = -np.inf
        best_move = None
        best_child = None

        for move, child in self.children.items():
            score = ucb_score(self, child)
            if score > best_score:
                best_score = score
                best_move = move
                best_child = child

        return best_move, best_child

    def expand(self, env):
        moves = env.generate_moves()
        for move in moves:
            self.children[tuple(move)] = MCTSNode(prior=1 / len(moves))
        self.state = env.get_environment_representation()
        self.expanded = True

    def __str__(self):
        ret = f"Visit count: {self.visit_count}, value sum: {self.value_sum}, #children: {len(self.children)}"
        return ret


class MCTS:
    def __init__(self, env, model):
        self.env = env
        self.model = model

    def backpropagate(self, path, score):
        for node in reversed(path):
            node.value_sum += score
            score *= -1
            node.visit_count += 1

    def run(self, n_simulations):
        root = MCTSNode(prior=0)
        root.expand(self.env)

        for _ in range(n_simulations):
            node = root
            search_path = [node]

            while node is not None and node.expanded:
                move, node = node.select_child()
                if move is not None:
                    search_path.append(node)
                    self.env.execute_move(move)

            self.env.check_terminal_conditions()

            if not self.env.variables.game_over and node is not None:
                node.expand(self.env)

            score = self.model.predict(self.env.get_environment_representation())
            self.backpropagate(search_path, score)

            # TODO: Copy environment at start instead?
            while search_path[-1] != root:
                self.env.undo_move()
                search_path.pop()

        return root
