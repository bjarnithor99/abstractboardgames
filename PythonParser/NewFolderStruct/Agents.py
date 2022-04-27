import time
from typing import Tuple
from .GameEngine import GameEngine, Move, Unresolved, Won

class Agent:
    def __init__(self) -> None:
        pass

    def getMove(self, gameEngine: GameEngine) -> Move:
        return gameEngine.getPlayerMoves()[0]


from random import choice
class RandomAgent(Agent):
    def getMove(self, gameEngine: GameEngine) -> Move:
        return choice(gameEngine.getPlayerMoves())


class TUIAgent(Agent):
    def getMove(self, gameEngine: GameEngine) -> Move:
        print(gameEngine)
        moves = gameEngine.getPlayerMoves()
        for i, move in enumerate(moves):
            print(i, str(move))

        def validInput(input: str):
            return (userInput.isdigit() and int(userInput) in range(0, len(moves))) or input.lower() in 'qu'

        userInput = input(f'{gameEngine.playersTurn()} input move:')
        while not validInput(userInput):
            print(f"Invalid input!")
            userInput = input(f'{gameEngine.playersTurn()} input move:')
    
        return moves[int(userInput)] if userInput.isdigit() else userInput

from ...BrowserAgent.AgentService import AgentService
class BrowserGUIAgent(Agent):
    def __init__(self, port: int) -> None:
        self.agentService = AgentService(port)

    def getMove(self, gameEngine: GameEngine) -> Move:
        return self.agentService.getMove(gameEngine)

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
        self.count = 0
        self.memo = dict()
        bestMove, bestScore = self.bestMoveAndScore(gameEngine, self.thinkingTime)
        print('MinMaxAI2 looked at', self.count, 'final positions')
        return bestMove

    def bestMoveAndScore(self, gameEngine: GameEngine, thinkingTime) -> Tuple[Move, int]:
        boardID = hash(gameEngine)
        if boardID in self.memo:
            return self.memo[boardID]
        self.count += 1

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
        self.count += 1
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
C:/Users/gudmu/AppData/Local/Programs/Python/Python310/python.exe -m ru_final_project.PythonParser.NewFolderStruct.Agents
'''
if __name__ == '__main__':
    #gameFileName = 'SimpleBreakThrough.game'
    #gameFileName = 'MediumBreakThrough.game'
    gameFileName = 'chess.game'
    gm = GameEngine(gameFileName, debug=True)
    result = gm.run([BrowserGUIAgent(port=8082), MinMaxAI(thinkingTime=1)])
    #result = gm.run([BrowserGUIAgent(port=8082), RandomAgent()])
    #result = gm.run([BrowserGUIAgent(port=8082), BrowserGUIAgent(port=8081)])
    #result = gm.run([MinMaxAI(thinkingTime=5), MinMaxAI(thinkingTime=5)])
    print(result)