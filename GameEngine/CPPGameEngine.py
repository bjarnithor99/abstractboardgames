#!/usr/bin/env python3

# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2022 Bjarni Dagur Thor Karason <bjarni@bjarnithor.com>

from .GameEngine import GameEngine
from ..cpp.python_bindings import python_bindings


@GameEngine.register
class CPPGameEngine(GameEngine):
    def __init__(self, filename):
        parser = python_bindings.Parser(filename)
        parser.parse()
        self.env = parser.get_environment()

    def get_environment_representation(self):
        return self.env.get_environment_representation()

    def generate_moves(self):
        return self.env.generate_moves()

    def execute_move(self, move):
        return self.env.execute_move(move)

    def undo_move(self):
        return self.env.undo_move()

    def game_over(self):
        return self.env.game_over()

    def get_white_score(self):
        return self.env.get_white_score()

    def get_first_player(self):
        return self.env.get_first_player()

    def get_current_player(self):
        return self.env.get_current_player()

    def reset(self):
        return self.env.reset()

    def jsonify(self):
        return self.env.jsonify()

    def getPlayerMoves(self):
        return self.generate_moves()

    def undoMove(self):
        return self.undo_move()

    def playMove(self, move):
        return self.execute_move(move)

    def playersTurn(self):
        return self.get_current_player()

if __name__ == "__main__":
    from ..Agents.browser_gui_agent import BrowserGUIAgent
    from ..Agents.random_agent import RandomAgent
    engine = CPPGameEngine("/home/bjarni/HR/T-404-LOKA/cpp/games/chess/chess.game")
    players = [BrowserGUIAgent(port=8002), RandomAgent()]
    while True:
        print("New game")
        engine.reset()
        move_number = 0
        while not engine.game_over():
            move = players[move_number % 2].get_move(engine)
            if move is not None:
                engine.execute_move(move)
                move_number += 1
        if engine.get_white_score() == 1:
            print("White wins")
        elif engine.get_white_score() == 0:
            print("Draw")
        elif engine.get_white_score() == -1:
            print("Black wins")
        else:
            print(f"Unexpected score {engine.get_white_score()}")
