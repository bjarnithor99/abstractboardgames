from __future__ import annotations
from ast import arguments
from re import L
from .Parser.Parser import Parser
from .Parser.ASTType import *
from .StateMachine.Types import *
from .Lexer.TokenTypes import *
from copy import deepcopy

class GameEngine:
    def __init__(self, fileName) -> None:
        f = open(f'NewFolderStruct/TestFiles/{fileName}')
        txt = ''.join(f.readlines())
        program: Program = Parser(txt).generateAST().root
        #print(program)

        self.currentLocation: Position = None

        self.players = program.players.names
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
        self.boardPieces: dict[str, BoardPiece] = dict()

        for rule in program.rules.ruleArr:
            self.resolveRule(rule)
        

        #put board into variables
        self.variables['Board']: EngineBoard = [[None for _ in range(program.board.y)] for _ in range(program.board.x)]
        for y in range(program.board.y):
            for x in range(program.board.x):
                pieceName = program.board.board[x+program.board.x*y]
                if pieceName == 'None':
                    self.variables['Board'][x][program.board.y-y-1] = None
                else:
                    dfa = self.pieceRules[pieceName]
                    playerName = self.piecesToPlayer[pieceName]
                    self.variables['Board'][x][program.board.y-y-1] = BoardPiece(dfa, pieceName, playerName)

        #Load into gloabl variables for effects and predicates
        self.variables['Board'] = self.variables['Board']

        
        self.changesStack: list[tuple[Letter, Position, BoardPiece, EngineBoard | None]] = []
        self.moveStack: list[int] = []

    #--Engine creation functions--

    def resolveRule(self, rule: Rule):
        visitFunc = {
            Predicate: self.visitPredicate,
            Effect: self.visitEffect,
            Macro: self.visitMacro,
            PieceRule: self.visitPieceRule,
            VariableDeclaration: self.visitVariableDeclaration
        }[type(rule)]
        visitFunc(rule)

    def visitPredicate(self, predicate: Predicate):
        self.predicates[predicate.name] = predicate

    def visitEffect(self, effect: Effect):
        self.effects[effect.name] = effect

    def visitMacro(self, macro: Macro):
        self.macros[macro.name] = macro

    def resolveMacros(self, regex: RegexTree, argumentVariables: dict[str: int] = dict()) -> RegexTree:
        def visitor(regexNode: SyntaxTreeNode):
            print('now visitng', RegexTree(regexNode))
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
                argumentVariablesInner = self.matchFunCallWithArguments(funCall, macro)
                resolvedRegexNode = self.resolveMacros(macro.regex, argumentVariablesInner).rootNode
                print('ee',RegexTree(resolvedRegexNode))
                return resolvedRegexNode
            else:
                raise Exception(f'uncaught regexNode type {type(regexNode)}')


        #clean regex
        def f():
            return RegexTree(visitor(regex.rootNode))

        return self.runFunctionWithArguments(f, argumentVariables)

    def matchFunCallWithArguments(self, funCall: FunctionCall, funType: Macro | Predicate | Effect) -> dict[str, int]:
        if len(funType.arguments) != len(funCall.arguments):
            raise Exception(f'{str(funType)} {funCall.name} called with incorrect number of paramaters')
        argumentVariables = dict()
        for i, key in enumerate(funType.arguments):
            argumentVariables[key] = self.resolveRuntimeIntegerExpression(funCall.arguments[i])
        return argumentVariables


    def runFunctionWithArguments(self, func, arguments: dict[str, int]):
        #load argumentVariables into self.variables
        storeForUnload = dict()
        for key in arguments:
            if key in self.variables:
                storeForUnload[key] = self.variables[key]
            self.variables[key] = arguments[key]

        value = func()

        #unload argumentVariables from self.variables
        for key in arguments:
            del self.variables[key]
            if key in storeForUnload:
                self.variables[key] = storeForUnload[key]

        return value

    def visitPieceRule(self, pieceRule: PieceRule):
        compiledRegex = self.resolveMacros(pieceRule.regex)
        from .StateMachine.Main import RegexToDFA
        print(f'compiled regex of {pieceRule.pieceName}:\n', compiledRegex)
        dfa = RegexToDFA(compiledRegex)
        print(f'Its regex:\n', dfa)
        self.pieceRules[pieceRule.pieceName] = dfa

    def visitVariableDeclaration(self, variableDeclaration: VariableDeclaration):
        value: int = self.resolveIntegerExpression(variableDeclaration.value)
        name = variableDeclaration.name
        self.variables[name] = value

    def resolveIntegerExpression(self, integerExpression: IntegerExpression) -> int:
        expStr = ''
        for token in integerExpression.tokenList:
            if isinstance(token, Word):
                expStr += str(self.variables[token.value])
            else:
                expStr += str(token.value)
        return eval(expStr)

    def resolveRuntimeIntegerExpression(self, integerExpression: IntegerExpression) -> int:
        if type(integerExpression) == int:
            return integerExpression
        expStr = ''


        token  = integerExpression.tokenList[0]
        if len(integerExpression.tokenList) == 1 and type(token) == Word and token.value in self.pieceRules:
            pieceName = token.value
            dfa = self.pieceRules[pieceName]
            playerName = self.piecesToPlayer[pieceName]
            return BoardPiece(dfa, pieceName, playerName)

        i = 0
        while i < len(integerExpression.tokenList):
            token = integerExpression.tokenList[i]
            if isinstance(token, Word):
                word: Word = token
                if word.value == 'this' and i+2 < len(integerExpression.tokenList):
                    dot = integerExpression.tokenList[i+1]
                    keyword = integerExpression.tokenList[i+2]
                    if type(dot) == Operator and dot.value == '.' and type(keyword) == Word:
                        if keyword.value in ('x', 'y'):
                            i += 2
                            expStr += str(self.currentLocation[{'x':0, 'y':1}[keyword.value]])
                else:
                    expStr += str(self.variables[word.value])
            else:
                expStr += str(token.value)
            i += 1
        return eval(expStr)


    #--Engine run funcitons--
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

    def getPlayerMoves(self, player: str) -> list[Move]:
        moves: list[Move] = []
        for x in range(len(self.variables['Board'])):
            for y in range(len(self.variables['Board'][0])):
                piece = self.variables['Board'][x][y]
                if piece is None or piece.player != player:
                    continue
                print('evaling:\n', piece.stateMachine)
                pieceMoves = self.getBoardPieceMoves(piece.stateMachine.start, (x,y))
                moves.extend(pieceMoves)
        return moves

            

    def getBoardPieceMoves(self, state: State, location: Position) -> list[Move]:
        def rec(state: State, location: Position) -> list[list[Letter]]:
            x, y = location
            boardPiece = self.variables['Board'][x][y]
            dfa = boardPiece.stateMachine
            player = boardPiece.player

            moveList = []

            
            for transition in state.outTransitions:
                letter = transition.letter
                dx, dy = letter.dx, letter.dy
                newLocation = (x+dx, y+dy)

                if not self.evaluatePredicate(letter.pre, newLocation, player):
                    continue

                if transition.end.final:
                    moveList.append([letter])

                self.playLetter(letter, location)

                movesFromThisTransition = [[letter, *move] for move in rec(transition.end, newLocation)]
                moveList.extend(movesFromThisTransition)

                self.undoLetter()

            return moveList

        moves = [Move(letterArr, location) for letterArr in rec(state, location)]
        return moves


    def playLetter(self, letter: Letter, location: Position):
        x, y = location
        dx, dy = letter.dx, letter.dy
        self.currentLocation = (x+dx, y+dy)

        thingUnderMoveLocation = self.variables['Board'][x+dx][y+dy]
        piece = self.variables['Board'][x][y]
        self.variables['Board'][x][y] = None
        self.variables['Board'][x+dx][y+dy] = piece

        board: EngineBoard = None
        if letter.effect is not None:
            board = deepcopy(self.variables['Board'])
            self.runEffect(letter.effect)

        self.changesStack.append((letter, location, thingUnderMoveLocation, board))
    

    def undoLetter(self):
        letter, location, thingUnderMoveLocation, board = self.changesStack.pop()

        if board is not None:
            self.variables['Board'] = board

        x, y = location
        dx, dy = letter.dx, letter.dy
        self.variables['Board'][x][y] = self.variables['Board'][x+dx][y+dy]
        self.variables['Board'][x+dx][y+dy] = thingUnderMoveLocation

        self.currentLocation = location

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
        x, y = location
        self.currentLocation = (x,y)
        inRange = (0<=x<len(self.variables['Board'])) and (0<=y<len(self.variables['Board'][0]))
        if not inRange:
            return False
        if type(predicate) == FunctionCall:
            preCall: FunctionCall = predicate
            if preCall.name not in self.predicates:
                raise Exception(f'Predicate {preCall.name} called but does not exist')
            predicateType = self.predicates[preCall.name]
            argumentVariables = self.matchFunCallWithArguments(preCall, predicateType)


            def f():
                return self.resolveRuntimeIntegerExpression(predicateType.expression)

            returnValue = self.runFunctionWithArguments(f, argumentVariables)
        else:
            returnValue = {
                'enemy': (self.variables['Board'][x][y] is not None) and (self.variables['Board'][x][y].player!=player),
                'empty': self.variables['Board'][x][y] is None,
                'true': True
            }[predicate]
        return returnValue 

    def runEffect(self, effectCall: FunctionCall):
        EffectName = effectCall.name
        effectType = self.effects[EffectName]

        if EffectName not in self.effects:
            raise Exception(f'Effect {EffectName} called before initialization')
        effectType = self.effects[EffectName]
        if len(effectCall.arguments) != len(effectType.arguments):
            raise Exception(f'Effect {EffectName} called with incorrect number of paramaters')
        argumentVariables = dict()
        for i, key in enumerate(effectType.arguments):
            argumentVariables[key] = self.resolveRuntimeIntegerExpression(effectCall.arguments[i])


        #load argumentVariables into self.variables
        storeForUnload = dict()
        for key in argumentVariables:
            if key in self.variables:
                storeForUnload[key] = self.variables[key]
            self.variables[key] = argumentVariables[key]

        #run assignments
        for assignmnet in effectType.assignments:
            self.runAssignment(assignmnet)

        #unload argumentVariables from self.variables
        for key in argumentVariables:
            del self.variables[key]
            if key in storeForUnload:
                self.variables[key] = storeForUnload[key]



    def runAssignment(self, assignment: Assignment):
        variables = self.variables

        indexes = [assignment.variableName, *[self.resolveRuntimeIntegerExpression(indexExpr) for indexExpr in assignment.indexes]]
        indexes = indexes[::-1]

        while len(indexes) > 1:
            variables = variables[indexes.pop()]

        variables[indexes.pop()] = self.resolveRuntimeIntegerExpression(assignment.newValue)

    def __str__(self):
        returnStr = ''
        for y in range(len(self.variables['Board'][0])-1,-1,-1):
            for x in range(len(self.variables['Board'])):
                piece = self.variables['Board'][x][y]
                if piece is None:
                    returnStr += 'None '
                else:
                    returnStr += piece.name + ' '
            returnStr += '\n'
        return returnStr


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
        return returnStr[:-1] + ']'

    

class BoardPiece:
    def __init__(self, stateMachine: DFAStateMachine, name: str, player: str) -> None:
        self.stateMachine: DFAStateMachine = stateMachine
        self.name: str = name
        self.player: str = player

EngineBoard = list[list[BoardPiece | None]]

if __name__ == '__main__':
    gm = GameEngine('demo3.game')
    gm.run()