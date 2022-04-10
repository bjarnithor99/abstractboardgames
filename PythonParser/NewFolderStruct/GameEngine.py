from __future__ import annotations
from tkinter.messagebox import RETRY
from .Parser.Parser import Parser
from .Parser.ASTType import *
from .StateMachine.Types import *
from .Lexer.TokenTypes import *
from .StateMachine.Main import RegexToDFA
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

class GameState:
    pass

class Won(GameState):
    def __init__(self, playerName: str) -> None:
        self.playerName: str = playerName

class Draw(GameState):
    pass

class Unresolved:
    pass

class BoardPiece:
    def __init__(self, stateMachine: DFAStateMachine, name: str, player: str) -> None:
        self.stateMachine: DFAStateMachine = stateMachine
        self.name: str = name
        self.player: str = player

EngineBoard = list[list[BoardPiece | None]]


#The variable names 'this', 'Board', 'enemy', 'empty' and 'true' have special meaning, don't write
#them if you don't know what you are doing
DebugFolder = 'NewFolderStruct/DebugInfo/'
class GameEngine:
    
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
        f = open(f'NewFolderStruct/TestFiles/{fileName}')
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
        for rule in program.rules.ruleArr:
            visitFunc = {
                Predicate: self.visitPredicate,
                Effect: self.visitEffect,
                Macro: self.visitMacro,
                PieceRule: self.visitPieceRule,
                VariableDeclaration: self.visitVariableDeclaration,
                Victory: self.visitVictory
            }[type(rule)]
            visitFunc(rule)

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
    
    def visitPieceRule(self, pieceRule: PieceRule):
        try:
            compiledRegex = self.compileRegex(pieceRule.regex)
        except Exception as e:
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
                #Resolve integer expression
                letter: Letter = regexNode
                dx = self.resolveIntegerExpression(letter.dx)
                dy = self.resolveIntegerExpression(letter.dy)
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


        #clean regex
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


    def resolveIntegerExpression(self, integerExpression: IntegerExpression) -> int | BoardPiece:
        #If integer expression is loading a state machine
        token = integerExpression.tokenList[0]
        if len(integerExpression.tokenList) == 1 and type(token) == Word and token.value in self.pieceRules:
            pieceName = token.value
            dfa = self.pieceRules[pieceName]
            if dfa is None:
                return None
            else:
                playerName = self.piecesToPlayer[pieceName]
                return BoardPiece(dfa, pieceName, playerName)

        #Else it is returning an int
        i = 0
        expStr = ''
        while i < len(integerExpression.tokenList):
            token = integerExpression.tokenList[i]
            if isinstance(token, Word):
                word: Word = token
                if i + 2 < len(integerExpression.tokenList):
                    dotToken = integerExpression.tokenList[i+1]
                    secondWordToken = integerExpression.tokenList[i+2]
                    if type(dotToken) == Operator and dotToken.value == '.' and type(secondWordToken) == Word:
                        i += 3
                        expStr += str(self.variables[word.value][secondWordToken.value])
                        continue
                i += 1
                expStr += str(self.variables[word.value])
                continue

            else:
                i += 1
                expStr += str(token.value)
                continue
        return eval(expStr)

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


    def evaluatePredicate(self, predicate: FunctionCall | IntegerExpression, location: Position, player: str):
        if type(predicate) == FunctionCall and predicate.name == 'BlackBackLane':
            pass
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
        elif type(predicate) == IntegerExpression:
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
        variables = self.variables

        indexes = [assignment.variableName, *[self.resolveIntegerExpression(indexExpr) for indexExpr in assignment.indexes]]
        indexes = indexes[::-1]

        while len(indexes) > 1:
            variables = variables[indexes.pop()]

        variables[indexes.pop()] = self.resolveIntegerExpression(assignment.newValue)
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
            vicotryCondition = self.victoryConditions[player]
            if self.resolveIntegerExpression(vicotryCondition.expression):
                numberOfWins += 1
                winner = player
        
        if numberOfWins == 1:
            return Won(winner)
        elif numberOfWins > 1 or len(self.getPlayerMoves()) == 0:
            return Draw()
        elif numberOfWins == 0:
            return Unresolved()

    def playersTurn(self):
        return self.players[self.playersTurnIndex]

    def run(self, agents: list[Agent]) -> GameState:
        self.playersTurnIndex = 0

        playerToAgent: dict[str, Agent] = dict()
        for i, player in enumerate(self.players):
            playerToAgent[player] = agents[i]
            agents[i].player = player
        
        while True:
            player = self.playersTurn()
            moves = self.getPlayerMoves()
            move = playerToAgent[player].getMove(moves)

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
            if type(gameState) in (Draw, Won):
                resultStr = "Game over"
                if type(gameState) == Draw:
                    resultStr += ', the game was a draw.'
                elif type(gameState) == Won:
                    win: Won = gameState
                    resultStr += f', player {win.playerName} Won!'
                if self.debug:
                    print(resultStr)
                return gameState


class Agent:
    def __init__(self, gameEngine: GameEngine) -> None:
        self.player: str = None

    def getMove(self, moves: list[Move]) -> Move:
        return moves[0]

class TUIAgent(Agent):
    def __init__(self, gameEngine: GameEngine) -> None:
        self.gameEngine = gameEngine

    def getMove(self, moves: list[Move]) -> Move:
        print(self.gameEngine)
        for i, move in enumerate(moves):
            print(i, str(move))

        def validInput(input: str):
            return (userInput.isdigit() and int(userInput) in range(0, len(moves))) or input.lower() in 'qu'

        userInput = input(f'{self.gameEngine.playersTurn()} input move:')
        while not validInput(userInput):
            print(f"Invalid input!")
            userInput = input(f'{self.gameEngine.playersTurn()} input move:')
    
        return moves[int(userInput)] if userInput.isdigit() else userInput


'''
C:/Users/gudmu/AppData/Local/Programs/Python/Python310/python.exe -m NewFolderStruct.GameEngine
'''
if __name__ == '__main__':
    gameFileName = 'SimpleBreakThrough.game'
    #gm = GameEngine('demo3.game')
    gm = GameEngine(gameFileName, debug=True)
    gm.run([TUIAgent(gm), TUIAgent(gm)])