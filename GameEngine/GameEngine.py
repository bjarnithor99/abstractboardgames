from abc import ABCMeta, abstractmethod


class GameEngine(metaclass=ABCMeta):
    @abstractmethod
    def get_environment_representation(self):
        pass

    @abstractmethod
    def execute_move(self, move):
        pass

    @abstractmethod
    def undo_move(self):
        return self.undoMove()

    @abstractmethod
    def game_over(self):
        pass

    @abstractmethod
    def get_white_score(self):
        pass

    @abstractmethod
    def get_first_player(self):
        pass

    @abstractmethod
    def get_current_player(self):
        pass

    @abstractmethod
    def reset(self):
        pass

    @abstractmethod
    def jsonify(self):
        pass

    @abstractmethod
    def generate_moves(self):
        pass

    @abstractmethod
    def getPlayerMoves(self):
        pass

    @abstractmethod
    def undoMove(self):
        pass

    @abstractmethod
    def playMove(self, move):
        pass

    @abstractmethod
    def playersTurn(self):
        pass
