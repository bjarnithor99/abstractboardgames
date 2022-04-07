from __future__ import annotations
from .Parser.Parser import Parser
from .Parser.ASTType import *
from .StateMachine.Types import *
from .Lexer.TokenTypes import *
from .StateMachine.Main import RegexToDFA
from copy import deepcopy

Position = tuple[int, int]
# C:/Users/gudmu/AppData/Local/Programs/Python/Python310/python.exe -m NewFolderStruct.GameEngine
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


#The variable names 'this' and 'Board' have special meaning, don't write
#them if you don't know what you are doing
class GameEngine:
    def __init__(self, fileName) -> None:
        f = open(f'NewFolderStruct/TestFiles/{fileName}')
        txt = ''.join(f.readlines())
        program: Program = Parser(txt).generateAST().root

        self.players = program.players.names
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
        self.pieceRules: dict[str, DFAStateMachine] = dict()
        self.pieceRules['None'] = None
        self.boardPieces: dict[str, BoardPiece] = dict()

        #Resolving rules
        for rule in program.rules.ruleArr:
            visitFunc = {
                Predicate: self.visitPredicate,
                Effect: self.visitEffect,
                Macro: self.visitMacro,
                PieceRule: self.visitPieceRule,
                VariableDeclaration: self.visitVariableDeclaration
            }[type(rule)]
            visitFunc(rule)

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


    #--- Resolve rules functions ---
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
        dfa = RegexToDFA(compiledRegex)
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

    def undoMove(self):
        moveLength = self.moveStack.pop()
        for i in range(moveLength):
            self.undoLetter()


    def evaluatePredicate(self, predicate: str, location: Position, player: str):
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
        else:
            returnValue = {
                'enemy': (self.getTile(x,y) is not None) and (self.getTile(x,y).player!=player),
                'empty': self.getTile(x,y) is None,
                'true': True
            }[predicate]
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
    def getPlayerMoves(self, player: str) -> list[Move]:
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
                maxSize = max(len(str(self.variables['Board'][x][y])),0)
        returnStr = ''
        for y in range(len(self.variables['Board'][0])-1,-1,-1):
            for x in range(len(self.variables['Board'])):
                piece = self.variables['Board'][x][y]
                if piece is None:
                    temp = 'e'
                else:
                    temp = piece.name
                returnStr += temp.rjust(12) + ' '
            returnStr += '\n'
        return returnStr

    def run(self):
        playersTurnIndex = 0
        while True:
            playersTurn = self.players[playersTurnIndex]
            moves = self.getPlayerMoves(playersTurn)
            print(self)
            for i, move in enumerate(moves):
                print(i, str(move))
            userInput = input(f'{playersTurn} input move:')
            if userInput.lower() == 'q':
                break
            if userInput.lower() == 'u':
                playersTurnIndex = (playersTurnIndex-1)%len(self.players)
                self.undoMove()
                continue

            indexOfMove = int(userInput)
            move = moves[indexOfMove]
            self.playMove(move)
            playersTurnIndex = (playersTurnIndex+1)%len(self.players)


if __name__ == '__main__':
    gm = GameEngine('demo3.game')
    gm.run()