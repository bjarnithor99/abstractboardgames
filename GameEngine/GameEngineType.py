from __future__ import annotations


class GameEngine:
    def get_environment_representation(self):
        pass

    def execute_move(self, move: Move):
        pass

    def undo_move(self):
        return self.undoMove()

    def game_over(self):
        pass

    def get_white_score(self):
        pass

    def get_first_player(self):
        pass

    def get_current_player(self):
        pass

    def reset(self):
        pass

    def jsonify(self):
        pass

    def generate_moves(self) -> list[Move]:
        pass


    #CamelCaseFunctions
    def getPlayerMoves(self):
        return self.generate_moves()

    def undoMove(self):
        return self.undo_move()

    def playMove(self, move):
        return self.execute_move(move)

    def playersTurn(self):
        return self.get_current_player()

class Move:
    pass

class GameState:
    pass

class Win(GameState):
    def __init__(self, playerName: str) -> None:
        self.playerName: str = playerName

    def __str__(self) -> str:
        return f'Player {self.playerName} won!'

class Draw(GameState):
    def __str__(self) -> str:
        return 'Draw'

class Unresolved:
    def __str__(self) -> str:
        return 'Unresolved'
    
        