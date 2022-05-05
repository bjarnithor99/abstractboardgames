from __future__ import annotations

from ..PythonParser.Parser.IntegerExpressionParser.ASTType import SyntaxTreeNode as IntegerExpressionTreeNode
from ..PythonParser.Parser.IntegerExpressionParser.ASTType import IntegerExpressionTree
from ..PythonParser.Parser.IntegerExpressionParser.ASTType import Variable as IntegerExpressionVariable
from ..PythonParser.IntegerExpression.Evaluator import evaluateIntegerExpression
from ..PythonParser.Parser.Parser import Parser
from ..PythonParser.Parser.ASTType import *
from ..PythonParser.StateMachine.Types import *
from ..PythonParser.Lexer.TokenTypes import *
from ..PythonParser.StateMachine.Main import RegexToDFA

from ..PythonParser.IntegerExpression.Preprocessor import resolveConstants, writeVariablesToConstants
from copy import deepcopy

Position = tuple[int, int]
class Move:
    def __init__(self, letterArr: list[Letter], startPost: Position) -> None:
        self.startPos: Position = startPost
        self.letterArr: list[Letter] = letterArr

    def __str__(self):
        x,y = self.startPos
        returnStr = f'Move ({x},{y})['
        for letter in self.letterArr:
            returnStr += f'({letter.dx}, {letter.dy})' + (('{'+f'{letter.effect}'+'}') if letter.effect is not None else '')
        return returnStr + ']'


class BoardPiece:
    def __init__(self, stateMachine: DFAStateMachine, name: str, player: str) -> None:
        self.stateMachine: DFAStateMachine = stateMachine
        self.name: str = name
        self.player: str = player

EngineBoard = list[list[BoardPiece | None]]

import os
DebugFolder = os.path.join(
    os.path.dirname(os.path.abspath(__file__)),'./DebugInfo/'
)
TestFileFolder = os.path.join(
    os.path.dirname(os.path.abspath(__file__)),"../PythonParser/TestFiles/"
)

#The variable names 'this', 'Board', 'enemy', 'empty' and 'true' have special meaning, don't write
#them if you don't know what you are doing

#Implements GameEngine
from .GameEngineType import GameEngine, Win, Draw, Unresolved, GameState
class PythonGameEngine(GameEngine):

    
    def __hash__(self):
        player = self.playersTurn()
        hashStr = ''
        X = len(self.variables['Board']); Y = len(self.variables['Board'][0])
        for x in range(X):
            for y in range(Y):
                hashStr += f'{x}{y}'
                tile = self.getTile(x, y)
                if tile is None:
                    hashStr += '0'
                else:
                    name = tile.name
                    hashStr += str(self.pieceNameToHash[name])
        hashStr += str(self.playersTurn())
        hashID = hash(hashStr)
        return hashID
        



    def debugParsing(self, fileName, program: Program):
        f = open(DebugFolder + 'ParsingInfo.txt', 'w+')
        debugStr = f'{fileName} parsed as:\n{str(program)}'
        f.write(debugStr)
        f.close()

    def debugRegexCompiling(self):
        f = open(DebugFolder + 'RegexInfo.txt', 'w+')
        f.write(self.debugRegexStr)
        f.close()

    def __init__(self, fileName, debug=False) -> None:
        self.debug = debug
        f = open(TestFileFolder + fileName)
        txt = ''.join(f.readlines())
        program: Program = Parser(txt).generateAST().root

        if self.debug:
            self.debugParsing(fileName, program)

        self.players = program.players.names
        self.playersTurnIndex = 0
        #resolve pieceToPlayerDict
        self.piecesToPlayer: dict[str, str] = dict()
        for piece in program.pieces.pieceArr:
            name, player = piece.pieceName, piece.playerName
            self.piecesToPlayer[name] = player


        #resolve variables/ stateMachines/ macros/ preds/ effects
        self.effects: dict[str, Effect] = dict()
        self.predicates: dict[str, Predicate] = dict()
        self.variables: dict[str, int] = dict()
        self.macros: dict[str, Macro] = dict()
        self.pieceRules: dict[str, DFAStateMachine] = dict(); self.pieceRules['None'] = None
        self.boardPieces: dict[str, BoardPiece] = dict()
        self.victoryConditions: dict[str, Victory] = dict()

        #Resolving rules
        self.debugRegexStr = ''

        variableDeclarations = []
        for rule in program.rules.ruleArr:
            if type(rule) == VariableDeclaration:
                #load globals after compile
                variableDeclarations.append(rule)
            else:
                visitFunc = {
                    Predicate: self.visitPredicate,
                    Effect: self.visitEffect,
                    Macro: self.visitMacro,
                    PieceRule: self.visitPieceRule,
                    Victory: self.visitVictory
                }[type(rule)]
                visitFunc(rule)


        for variableDeclaration in variableDeclarations:
            self.visitVariableDeclaration(variableDeclaration)

        if self.debug:
            self.debugRegexCompiling()

        #Load pieces into board
        self.variables['Board'] = [[None for _ in range(program.board.y)] for _ in range(program.board.x)]
        for y in range(program.board.y):
            for x in range(program.board.x):
                pieceName = program.board.board[x+program.board.x*y]
                if pieceName == 'None':
                    self.setTile(x, program.board.y-y-1, None)
                else:
                    dfa = self.pieceRules[pieceName]
                    playerName = self.piecesToPlayer[pieceName] 
                    self.setTile(x, program.board.y-y-1, BoardPiece(dfa, pieceName, playerName))



        #Memory for undo
        self.changesStack: list[tuple[Letter, Position, BoardPiece, EngineBoard | None]] = []
        self.moveStack: list[int] = []

        #Hash for pieces
        self.pieceNameToHash = dict()
        for i, piece in enumerate(program.pieces.pieceArr):
            name, player = piece.pieceName, piece.playerName
            self.pieceNameToHash[name] = i+1


    #--- Resolve rules functions ---
    def visitVictory(self, victoryCondition: Victory):
        if victoryCondition.name not in self.players:
            raise Exception(f'Invalid victory condition:\n{str(victoryCondition)}\n Player {victoryCondition.name} does not exist!')
        self.victoryConditions[victoryCondition.name] = victoryCondition

    def visitPredicate(self, predicate: Predicate):
        self.predicates[predicate.name] = predicate

    def visitEffect(self, effect: Effect):
        self.effects[effect.name] = effect

    def visitMacro(self, macro: Macro):
        self.macros[macro.name] = macro

    def visitIntegerExpression(self, integerExpression: IntegerExpressionTree, env: list[str:int] = {}):
        intExp = writeVariablesToConstants(integerExpression, self.variables)
        intExp = resolveConstants(intExp)
        integerExpression.rootNode = intExp.rootNode
    
    def visitPieceRule(self, pieceRule: PieceRule):
        try:
            compiledRegex = self.compileRegex(pieceRule.regex)
        except Exception as e:
            print(e)
            raise Exception(f'{str(e)}\nFailed to compile regex for {pieceRule.pieceName}:\n{str(pieceRule.regex)}')
        dfa, debugStr = RegexToDFA(compiledRegex, debug=True)
        if self.debug:
            self.debugRegexStr += f'\n\n--Compiling regex for {pieceRule.pieceName}:\n{str(pieceRule.regex)}\n'
            self.debugRegexStr += f'\nResolved regex:\n{str(compiledRegex)}\n'
            self.debugRegexStr += debugStr
        self.pieceRules[pieceRule.pieceName] = dfa

    def visitVariableDeclaration(self, variableDeclaration: VariableDeclaration):
        value: int = self.resolveIntegerExpression(variableDeclaration.value)
        name = variableDeclaration.name
        self.variables[name] = value

    # Resolves integer expressions and unwraps all macros
    def compileRegex(self, regex: RegexTree) -> RegexTree:
        def visitor(regexNode: SyntaxTreeNode) -> SyntaxTreeNode:
            if isinstance(regexNode, UnarySymbol):
                child = visitor(regexNode.child)
                return type(regexNode)(child)
            elif isinstance(regexNode, BinarySymbol):
                child1 = visitor(regexNode.child1)
                child2 = visitor(regexNode.child2)
                return type(regexNode)(child1, child2)
            elif type(regexNode) == Letter:
                # Resolve integer expression
                letter: Letter = regexNode
                dx = self.resolveIntegerExpression(letter.dx)
                dy = self.resolveIntegerExpression(letter.dy) 
                #Predicates and effects arguments need to be compiled too
                if isinstance(letter.pre, IntegerExpressionTree):
                    self.visitIntegerExpression(letter.pre)
                elif isinstance(letter.pre, Predicate):
                    for arg in letter.pre.arguments:
                        self.visitIntegerExpression(letter.pre)
                if letter.effect is not None:
                    for arg in letter.effect.arguments:
                        self.visitIntegerExpression(arg)

                return Letter(dx, dy, letter.pre, effect=letter.effect)
            elif type(regexNode) == FunctionCall:
                funCall: FunctionCall = regexNode
                if funCall.name not in self.macros:
                    raise Exception(f'Macro {funCall.name} called before initialization')
                macro = self.macros[funCall.name]
                argumentVariables = self.matchFunCallWithArguments(funCall, macro)
                return self.runFunctionInEnvironment(lambda: self.compileRegex(macro.regex).rootNode, argumentVariables)
            else:
                raise Exception(f'uncaught regexNode type {type(regexNode)}')


        # clean regex
        return RegexTree(visitor(regex.rootNode))
    #--- Resolve rule functions end ---



    #--- Helper functions ---
    def getTile(self, x: int, y: int) -> BoardPiece | None:
        return self.variables['Board'][x][y]

    def setTile(self, x: int, y: int, newTileValue: BoardPiece | None):
        self.variables['Board'][x][y] = newTileValue


    def runFunctionInEnvironment(self, func, environmnet: dict[str, int]):
        #load argumentVariables into self.variables
        storeForUnload = dict()
        for key in environmnet:
            if key in self.variables:
                storeForUnload[key] = self.variables[key]
            self.variables[key] = environmnet[key]

        value = func()

        #unload argumentVariables from self.variables
        for key in environmnet:
            del self.variables[key]
            if key in storeForUnload:
                self.variables[key] = storeForUnload[key]

        return value

    def matchFunCallWithArguments(self, funCall: FunctionCall, funType: Macro | Predicate | Effect) -> dict[str, int]:
        if len(funType.arguments) != len(funCall.arguments):
            raise Exception(f'{str(funType)} {funCall.name} called with incorrect number of paramaters')
        argumentVariables = dict()
        for i, key in enumerate(funType.arguments):
            argumentVariables[key] = self.resolveIntegerExpression(funCall.arguments[i])
        return argumentVariables


    def resolveIntegerExpression(self, integerExpression: IntegerExpressionTree) -> int | BoardPiece:
        #If integer expression is loading a state machine
        rootNode = integerExpression.rootNode
        if type(rootNode) == IntegerExpressionVariable and rootNode.name in self.pieceRules:
            pieceName = rootNode.name
            dfa = self.pieceRules[pieceName]
            if dfa is None:
                return None
            else:
                playerName = self.piecesToPlayer[pieceName]
                return BoardPiece(dfa, pieceName, playerName)
        
        try:
            return evaluateIntegerExpression(integerExpression.rootNode, self.variables)
        except Exception as error:
            print('Error evaluating integer expression:')
            print(integerExpression)
            print('Reason:')
            print(error)
            raise error

    def setThis(self, thisVariables: dict[str: int]):
        self.variables['this'] = thisVariables

    #--- Helper functions end ---



    #--- Functions for calculating move effect ---
    def playLetter(self, letter: Letter, location: Position):
        x, y = location
        dx, dy = letter.dx, letter.dy

        self.setThis({'x': x+dx, 'y': y+dy})

        thingUnderMoveLocation = self.getTile(x+dx, y+dy)

        piece = self.getTile(x, y)
        self.setTile(x,y,None)
        self.setTile(x+dx, y+dy, piece)

        currentVariables: dict[str: int] = None
        if letter.effect is not None:
            currentVariables = deepcopy(self.variables)
            self.runEffect(letter.effect)

        self.changesStack.append((letter, location, thingUnderMoveLocation, currentVariables))
    

    def undoLetter(self):
        letter, location, thingUnderMoveLocation, oldVariables = self.changesStack.pop()

        if oldVariables is not None:
            self.variables = oldVariables

        x, y = location
        dx, dy = letter.dx, letter.dy
        self.setTile(x,y, self.getTile(x+dx, y+dy))
        self.setTile(x+dx, y+dy, thingUnderMoveLocation)

        self.setThis({'x': x, 'y': y})


    def playMove(self, move: Move):
        self.moveStack.append(len(move.letterArr))
        x, y = move.startPos
        dx, dy = 0, 0
        for letter in move.letterArr:
            x += dx; y += dy
            dx, dy = letter.dx, letter.dy
            self.playLetter(letter, (x, y))
        self.playersTurnIndex = (self.playersTurnIndex+1)%len(self.players)

    def undoMove(self):
        moveLength = self.moveStack.pop()
        for i in range(moveLength):
            self.undoLetter()
        self.playersTurnIndex = (self.playersTurnIndex-1)%len(self.players)


    def evaluatePredicate(self, predicate: FunctionCall | IntegerExpressionTreeNode, location: Position, player: str):
        x, y = location
        inRange = (0<=x<len(self.variables['Board'])) and (0<=y<len(self.variables['Board'][0]))
        if not inRange:
            return False
        if type(predicate) == FunctionCall:
            preCall: FunctionCall = predicate
            if preCall.name not in self.predicates:
                raise Exception(f'Predicate {preCall.name} called but does not exist')
            predicateType = self.predicates[preCall.name]
            argumentVariables = self.matchFunCallWithArguments(preCall, predicateType)
            f = lambda: self.resolveIntegerExpression(predicateType.expression)
            returnValue = self.runFunctionInEnvironment(f, argumentVariables)
        elif isinstance(predicate, IntegerExpressionTree):
            specialPredicates = {
                'enemy': (self.getTile(x,y) is not None) and (self.getTile(x,y).player!=player),
                'empty': self.getTile(x,y) is None,
                'true': True
            }
            f = lambda: self.resolveIntegerExpression(predicate)
            returnValue = self.runFunctionInEnvironment(f, specialPredicates)
        else:
            raise Exception(f"Invalid predicate type {type(predicate)}!")

        return returnValue 

    def runEffect(self, effectCall: FunctionCall):
        EffectName = effectCall.name
        effectType = self.effects[EffectName]

        if EffectName not in self.effects:
            raise Exception(f'Effect {EffectName} called before initialization')
        effectType = self.effects[EffectName]
        argumentVariables = self.matchFunCallWithArguments(effectCall, effectType)

        #run assignments
        def runAssignments():
            for assignmnet in effectType.assignments:
                self.runAssignment(assignmnet)

        self.runFunctionInEnvironment(runAssignments, argumentVariables)


    def runAssignment(self, assignment: Assignment):
        try:
            variables = self.variables

            indexes = [assignment.variableName, *[self.resolveIntegerExpression(indexExpr) for indexExpr in assignment.indexes]]
            indexes = indexes[::-1]

            while len(indexes) > 1:
                variables = variables[indexes.pop()]

            variables[indexes.pop()] = self.resolveIntegerExpression(assignment.newValue)
        except Exception as error:
            print('Error running assignment:')
            print(assignment)
            print('Reason:')
            print(error)
            raise error
     #--- Functions for calculating move effect end ---



     #--- Calculating legal moves ---#
    def getPlayerMoves(self) -> list[Move]:
        player = self.playersTurn()
        moves: list[Move] = []
        for x in range(len(self.variables['Board'])):
            for y in range(len(self.variables['Board'][0])):
                tile = self.getTile(x,y)
                if tile is None or tile.player != player:
                    continue
                piece: BoardPiece = tile
                pieceMoves = self.getBoardPieceMoves(piece.stateMachine, (x,y))
                moves.extend(pieceMoves)
        return moves


    def getBoardPieceMoves(self, stateMachine: DFAStateMachine, location: Position) -> list[Move]:
        def getStateMoves(state: State, location: Position) -> list[list[Letter]]:
            x, y = location
            boardPiece = self.getTile(x,y)
            if boardPiece is None:
                #Effect killed its own piece, abort!!!! >:(
                return []
            player = boardPiece.player
            moveList = []
            
            for transition in state.outTransitions:
                letter = transition.letter
                dx, dy = letter.dx, letter.dy
                self.setThis({'x': x+dx, 'y': y+dy})
                newLocation = (x+dx, y+dy)

                if not self.evaluatePredicate(letter.pre, newLocation, player):
                    continue

                if transition.end.final:
                    moveList.append([letter])

                self.playLetter(letter, location)

                movesFromThisTransition = [[letter, *move] for move in getStateMoves(transition.end, newLocation)]
                moveList.extend(movesFromThisTransition)

                self.undoLetter()

            return moveList

        moves = [Move(letterArr, location) for letterArr in getStateMoves(stateMachine.start, location)]
        return moves
     #--- Calculating legal moves end ---#

    def __str__(self):
        maxSize = 0
        for y in range(len(self.variables['Board'][0])-1,-1,-1):
            for x in range(len(self.variables['Board'])):
                tile = self.getTile(x,y)
                maxSize = max(len('e' if tile is None else tile.name), maxSize)
        returnStr = f'Players turn: {self.playersTurn()}\n'
        for y in range(len(self.variables['Board'][0])-1,-1,-1):
            for x in range(len(self.variables['Board'])):
                piece = self.getTile(x,y)
                if piece is None:
                    temp = 'e'
                else:
                    temp = piece.name
                returnStr += temp.rjust(maxSize) + ' '
            returnStr += '\n'
        return returnStr

    def getGameState(self) -> GameState:
        winner = 'E:)-|-<'
        numberOfWins = 0
        for player in self.players:
            victoryCondition = self.victoryConditions[player]
            if self.resolveIntegerExpression(victoryCondition.expression):
                numberOfWins += 1
                winner = player
        
        if numberOfWins == 1:
            return Win(winner)
        elif numberOfWins > 1 or len(self.getPlayerMoves()) == 0:
            return Draw()
        elif numberOfWins == 0:
            return Unresolved()

    def playersTurn(self):
        return self.players[self.playersTurnIndex]

    def reset(self):
        while len(self.moveStack) != 0:
            self.undoMove()

    def jsonify(self: GameEngine):
        gameEngine = self
        board = [[piece.name if piece is not None else 'None' for piece in line] for line in gameEngine.variables["Board"]]
        moves = '[' + ','.join([self.gameEngineMoveToJSON(move) for move in gameEngine.getPlayerMoves()]) + ']'
        message = f'{{"board":  {str(board)}, "moves": {moves}}}'
        return simpleObjToJson(message)

    def gameEngineMoveToJSON(self, move: Move):
        letters = '[' + ','.join([self.gameEngineLetterToJSON(letter) for letter in move.letterArr])+ ']'
        return simpleObjToJson(f'{{"start":  {str(move.startPos)}, "letters": {letters}}}')

    def gameEngineLetterToJSON(self, letter: Letter):
        return simpleObjToJson({'dx': letter.dx, 'dy':letter.dy, 'effect': str(letter.effect).replace('(','').replace(')','')})

    def run(self, agents: list[Agent]) -> GameState:
        self.playersTurnIndex = 0

        playerToAgent: dict[str, Agent] = dict()
        for i, player in enumerate(self.players):
            playerToAgent[player] = agents[i]
            agents[i].player = player
        
        while True:
            player = self.playersTurn()
            moves = self.getPlayerMoves()
            move = playerToAgent[player].getMove(self)

            if type(move) == str and move in 'qu':
                if move == 'q':
                    break
                elif move == 'u':
                    self.undoMove()
                    continue


            if self.debug:
                print('Player ', player, 'on board')
                print(self)
                print('Picked move:', move)
            self.playMove(move)
            #print(self)

            gameState = self.getGameState()
            if type(gameState) in (Draw, Win):
                if self.debug:
                    print(gameState)
                return gameState

def simpleObjToJson(simpleObj):
    # simpleObj array[simpleObj], tuple[simpleObj], string or int
    replaceInfo = {
        '(': '[',
        ')': ']',
        "'": '"'
    }

    returnStr = str(simpleObj)
    for key in replaceInfo:
        returnStr = returnStr.replace(key, replaceInfo[key])
    return returnStr

'''
python -m ru_final_project.GameEngine.PythonGameEngine
'''
from ..Agents.Agents import BrowserGUIAgent, RandomAgent, TUIAgent
if __name__ == '__main__':
    print('hello world')
    ge = PythonGameEngine('chess.game', debug=True)
    agents = []
    agents.append(BrowserGUIAgent(8080))
    #agents.append(TUIAgent())
    agents.append(RandomAgent())
    result = ge.run(agents)
    print(result)