from typing import Tuple
from .GameEngine import GameEngine, Move, TUIAgent, Won, Draw, Unresolved, Agent


class MinMaxAI(Agent):
    def __init__(self, gameEngine: GameEngine) -> None:
        self.gameEngine: GameEngine = gameEngine

    def getMove(self, moves: list[Move]) -> Move:
        self.count = 0
        self.memo = dict()
        bestMove, bestScore = moves[0], -18719872399887
        #print('FINDING BEST MOVE')
        for move in moves:
            score = self.evalMove(move)
            #print(move, score)
            if score> bestScore:
                bestMove = move
                bestScore = score
        print('MinMaxAI looked at', self.count, 'final positions')
        return bestMove

    def evalMove(self, move, depth=4) -> int:
        self.gameEngine.playMove(move)

        boardID = hash(self.gameEngine)
        if boardID in self.memo:
            score = self.memo[boardID]
        elif depth == 0 or type(self.gameEngine.getGameState()) != Unresolved:
            score = self.evalBoard()
        else:
            score = max([-self.evalMove(move, depth-1) for move in self.gameEngine.getPlayerMoves()])
        self.memo[boardID] = score
        self.gameEngine.undoMove()
        return score

    def evalBoard(self) -> int:
        self.count += 1
        gameState = self.gameEngine.getGameState()
        if type(gameState) == Won:
            win: Won = gameState
            if win.playerName == self.player:
                score = 100
            else:
                score =  -100
        else:
            score = 0
        #print(self.gameEngine,'score:', score)
        return score

class MinMaxAI2(Agent):
    def __init__(self, gameEngine: GameEngine) -> None:
        self.gameEngine: GameEngine = gameEngine

    def getMove(self, moves: list[Move]) -> Move:
        self.count = 0
        self.memo = dict()
        bestMove, bestScore = self.bestMoveAndScore(6)
        print('MinMaxAI2 looked at', self.count, 'final positions')
        return bestMove

    def bestMoveAndScore(self, depth) -> Tuple[Move, int]:
        boardID = hash(self.gameEngine)
        if boardID in self.memo:
            return self.memo[boardID]
    
        moves = self.gameEngine.getPlayerMoves()
        bestMove, bestScore = moves[0], -1000
        for move in moves:
            self.gameEngine.playMove(move)

            if depth == 0 or type(self.gameEngine.getGameState()) != Unresolved:
                score = (-self.evalBoard())+depth
            else:
                bestEnemyMove, bestEnemyScore = self.bestMoveAndScore(depth-1)
                score = -bestEnemyScore

            if score > bestScore:
                bestMove, bestScore = move, score

            self.gameEngine.undoMove()
        #print('\nAI2 Thinks\n', self.gameEngine,'\nHas the score', score,'\n')
        self.memo[boardID] = (bestMove, bestScore)
        return bestMove, bestScore

    def evalBoard(self) -> int:
        self.count += 1
        gameState = self.gameEngine.getGameState()
        if type(gameState) == Won:
            win: Won = gameState
            if win.playerName == self.gameEngine.playersTurn():
                score = 100
            else:
                score = -100
        else:
            score = 0
        return score

if __name__ == "__main__":
    #gameEngine = GameEngine('SimpleGame.game', debug=False)
    w,b,d = 0,0,0
    for i in range(1):
        gameEngine = GameEngine('MediumBreakThrough.game', debug=False)
        #gameEngine = GameEngine('SimpleGame.game', debug=False)
        result = gameEngine.run([TUIAgent(gameEngine), MinMaxAI2(gameEngine)])
        if type(result) == Draw:
            d += 1
        elif type(result) ==  Won:
            win: Won = result
            if win.playerName == 'white':
                w += 1
            else:
                b += 1
    print(w,b,d)
    #gameEngine.run([TUIAgent(gameEngine), MinMaxAI(gameEngine)])