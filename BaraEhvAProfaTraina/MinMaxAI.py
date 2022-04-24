from typing import Tuple
import time
from ..PythonParser.NewFolderStruct.GameEngine import GameEngine, Move, TUIAgent, Won, Draw, Unresolved, Agent

class MinMaxAI(Agent):
    def __init__(self, thinkingTime=2, evalFunction=None) -> None:
        self.evalFunction = evalFunction
        self.thinkingTime = thinkingTime
        self.count = 0

    def calculateTimes(self, gameEngine: GameEngine):
        I = 100
        start = time.time()
        for i in range(I):
            self.evalBoard(gameEngine)
        end = time.time()
        self.evalTime = (end-start)/I


    def getMove(self, gameEngine: GameEngine) -> Move:
        self.calculateTimes(gameEngine)
        #self.count = 0
        self.memo = dict()
        bestMove, bestScore = self.bestMoveAndScore(gameEngine, self.thinkingTime)
        #print('MinMaxAI2 looked at', self.count, 'final positions')
        return bestMove

    def bestMoveAndScore(self, gameEngine: GameEngine, thinkingTime) -> Tuple[Move, int]:
        boardID = hash(gameEngine)
        if boardID in self.memo:
            return self.memo[boardID]
        #self.count += 1

        moves = gameEngine.getPlayerMoves()

        bestMove, bestScore = moves[0], -1000
        timeForEach = thinkingTime/len(moves)
        for move in moves:
            gameEngine.playMove(move)

            if (timeForEach < self.evalTime) or type(gameEngine.getGameState()) != Unresolved:
                score = (-self.evalBoard(gameEngine))+thinkingTime*10
            else:
                bestEnemyMove, bestEnemyScore = self.bestMoveAndScore(gameEngine, timeForEach)
                score = -bestEnemyScore

            if score > bestScore:
                bestMove, bestScore = move, score

            gameEngine.undoMove()
        #print('\nAI2 Thinks\n', self.gameEngine,'\nHas the score', score,'\n')
        self.memo[boardID] = (bestMove, bestScore)
        return bestMove, bestScore

    def evalBoard(self, gameEngine: GameEngine) -> int:
        #self.count += 1
        gameState = gameEngine.getGameState()
        if type(gameState) == Won:
            win: Won = gameState
            if win.playerName == gameEngine.playersTurn():
                score = 100
            else:
                score = -100
        else:
            if self.evalFunction is not None:
                score = self.evalFunction(gameEngine)
            else:
                score = 0
        return score


'''
C:/Users/gudmu/AppData/Local/Programs/Python/Python310/python.exe -m ru_final_project.GamePlayingAgent2.main
'''

if __name__ == "__main__":
    gameEngine = GameEngine('MediumBreakThrough.game', debug=False)
    result = gameEngine.run([TUIAgent(gameEngine), MinMaxAI(gameEngine)])
    print('Game over:', result)