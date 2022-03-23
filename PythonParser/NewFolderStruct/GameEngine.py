from __future__ import annotations
from .Parser.Parser import Parser
from .Parser.ASTType import *
from .StateMachine.Types import *
from .Lexer.TokenTypes import *


class GameEngine:
    def __init__(self, fileName) -> None:
        f = open(f'NewFolderStruct/TestFiles/{fileName}')
        txt = ''.join(f.readlines())
        program: Program = Parser(txt).generateAST().root

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
        for rule in program.rules.ruleArr:
            self.resolveRule(rule)

        #put board into variables
        self.board: list[list[BoardPiece | None]] = [[None for _ in range(program.board.y)] for _ in range(program.board.x)]
        for y in range(program.board.y):
            for x in range(program.board.x):
                pieceName = program.board.board[x+program.board.x*y]
                if pieceName == 'None':
                    self.board[x][program.board.y-y-1] = None
                else:
                    dfa = self.pieceRules[pieceName]
                    playerName = self.piecesToPlayer[pieceName]
                    self.board[x][program.board.y-y-1] = BoardPiece(dfa, pieceName, playerName)


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
            if isinstance(regexNode, UnarySymbol):
                regexNode.child = visitor(regexNode.child)
                return regexNode
            elif isinstance(regexNode, BinarySymbol):
                regexNode.child1 = visitor(regexNode.child1)
                regexNode.child2 = visitor(regexNode.child2)
                return regexNode
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
                if len(macro.arguments) != len(funCall.arguments):
                    raise Exception(f'Macro {funCall.name} called with incorrect number of paramaters')
                argumentVariables = dict()
                for i, key in enumerate(macro.arguments):
                    argumentVariables[key] = self.resolveIntegerExpression(funCall.arguments[i])
                return self.resolveMacros(macro.regex, argumentVariables).rootNode
            else:
                raise Exception(f'uncaught regexNode type {type(regexNode)}')

        #load argumentVariables into self.variables
        storeForUnload = dict()
        for key in argumentVariables:
            if key in self.variables:
                storeForUnload[key] = self.variables[key]
            self.variables[key] = argumentVariables[key]

        #clean regex
        regexTreeWithoutVariablesAndMacros = RegexTree(visitor(regex.rootNode))

        #unload argumentVariables from self.variables
        for key in argumentVariables:
            del self.variables[key]
            if key in storeForUnload:
                self.variables[key] = storeForUnload[key]
            
        return regexTreeWithoutVariablesAndMacros

    def visitPieceRule(self, pieceRule: PieceRule):
        compiledRegex = self.resolveMacros(pieceRule.regex)
        from .StateMachine.Main import RegexToDFA
        dfa = RegexToDFA(compiledRegex)
        self.pieceRules[pieceRule.pieceName] = dfa

    def visitVariableDeclaration(self, variableDeclaration: VariableDeclaration):
        value: int = self.resolveIntegerExpression(variableDeclaration.value)
        name = variableDeclaration.name
        self.variables[name] = value

    def resolveIntegerExpression(self, integerExpression: IntegerExpression) -> int:
        if type(integerExpression) == int:
            return integerExpression
        expStr = ''
        for token in integerExpression.tokenList:
            if isinstance(token, Word):
                expStr += str(self.variables[token.value])
            else:
                expStr += str(token.value)
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
            indexOfMove = int(input(f'{playersTurn} input move:'))
            move = moves[indexOfMove]
            self.playMove(move)
            playersTurnIndex = (playersTurnIndex+1)%len(self.players)

    def getPlayerMoves(self, player: str) -> list[Move]:
        moves: list[Move] = []
        for x in range(len(self.board)):
            for y in range(len(self.board[0])):
                piece = self.board[x][y]
                if piece is None or piece.player != player:
                    continue
                print('evaling:\n', piece.stateMachine)
                pieceMoves = self.getBoardPieceMoves(piece.stateMachine.start, (x,y))
                moves.extend(pieceMoves)
        return moves

            

    def getBoardPieceMoves(self, state: State, location: Position) -> list[Move]:
        def rec(state: State, location: Position) -> list[list[Letter]]:
            x, y = location
            boardPiece = self.board[x][y]
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

                thingUnderMoveLocation = self.board[x+dx][y+dy]
                self.board[x+dx][y+dy] = self.board[x][y]
                self.board[x][y] = None

                if letter.effect is not None:
                    #save the entire board and variables
                    #play the effect
                    pass

                movesFromThisTransition = [[letter, *move] for move in rec(transition.end, newLocation)]
                moveList.extend(movesFromThisTransition)

                if letter.effect is not None:
                    #revert the entire board and variables
                    pass

                self.board[x][y] = self.board[x+dx][y+dy]
                self.board[x+dx][y+dy] = thingUnderMoveLocation

            return moveList

        return [Move(letterArr, location) for letterArr in rec(state, location)]





            

    def playMove(self, move: Move):
        pass

    def evaluatePredicate(self, predicate: str, location: Position, player: str):
        x, y = location
        inRange = (0<=x<len(self.board)) and (0<=y<=len(self.board[0]))
        returnValue =  inRange and {
            'enemy': (self.board[x][y] is not None) and (self.board[x][y].player!=player),
            'empty': self.board[x][y] is None,
            'true': True
        }[predicate]
        return returnValue

    def runEffect(self, effect: Effect):
        pass

    def __str__(self):
        returnStr = ''
        for y in range(len(self.board[0])-1,-1,-1):
            for x in range(len(self.board)):
                piece = self.board[x][y]
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

if __name__ == '__main__':
    gm = GameEngine('demo2.game')
    gm.run()