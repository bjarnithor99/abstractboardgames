from __future__ import annotations
from copy import deepcopy

from .GameEngine import GameEngine
from ..PythonParser.Parser.IntegerExpressionParser.ASTType import SyntaxTreeNode as IntegerExpressionTreeNode
from ..PythonParser.Parser.IntegerExpressionParser.ASTType import Variable as IntegerExpressionVariable
from ..PythonParser.IntegerExpression.Evaluator import evaluateIntegerExpression
from ..PythonParser.IntegerExpression.Preprocessor import resolveConstants, writeVariablesToConstants
from ..PythonParser.Parser.ASTType import *
from ..PythonParser.Parser.Parser import Parser
from ..PythonParser.StateMachine.Main import RegexToDFA
from ..PythonParser.StateMachine.Types import *
from ..PythonParser.Parser.IntegerExpressionParser.ASTType import *


binaryOpToFunc = {
    "and": lambda a, b: a and b,
    "or": lambda a, b: a or b,
    "&&": lambda a, b: a and b,
    "||": lambda a, b: a or b,
    "!=": lambda a, b: a != b,
    "==": lambda a, b: a == b,
    "+": lambda a, b: a + b,
    "-": lambda a, b: a - b,
    "*": lambda a, b: a * b,
    "/": lambda a, b: a / b,
    "//": lambda a, b: a // b,
    "%": lambda a, b: a % b,
}

unaryOpToFunc = {"not": lambda a: not a, "!": lambda a: not a, "-": lambda a: -a}

Position = tuple[int, int]


class Move:
    def __init__(self, letterArr: list[Letter], startPost: Position) -> None:
        self.startPos: Position = startPost
        self.letterArr: list[Letter] = letterArr

    def __str__(self):
        x, y = self.startPos
        returnStr = f"Move ({x},{y})["
        for letter in self.letterArr:
            returnStr += f"({letter.dx}, {letter.dy})" + (
                ("{" + f"{letter.effect}" + "}") if letter.effect is not None else ""
            )
        return returnStr + "]"


class BoardPiece:
    def __init__(self, stateMachine: DFAStateMachine, name: str, player: str) -> None:
        self.stateMachine: DFAStateMachine = stateMachine
        self.name: str = name
        self.player: str = player


EngineBoard = list[list[BoardPiece | None]]

import os

DebugFolder = os.path.join(os.path.dirname(os.path.abspath(__file__)), "./DebugInfo/")
TestFileFolder = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "../PythonParser/TestFiles/"
)

# The variable names 'this', 'Board', 'enemy', 'empty' and 'true' have special meaning, don't write
# them if you don't know what you are doing


class GameState:
    pass


class Win(GameState):
    def __init__(self, playerName: str) -> None:
        self.playerName: str = playerName

    def __str__(self) -> str:
        return f"Player {self.playerName} won!"


class Draw(GameState):
    def __str__(self) -> str:
        return "Draw"


class Unresolved:
    def __str__(self) -> str:
        return "Unresolved"


# Implements GameEngine
@GameEngine.register
class PythonGameEngine(GameEngine):
    def __hash__(self):
        player = self.playersTurn()
        hashStr = ""
        X = len(self.variables["Board"])
        Y = len(self.variables["Board"][0])
        for x in range(X):
            for y in range(Y):
                hashStr += f"{x}{y}"
                tile = self.getTile(x, y)
                if tile is None:
                    hashStr += "0"
                else:
                    name = tile.name
                    hashStr += str(self.pieceNameToHash[name])
        hashStr += str(self.playersTurn())
        hashID = hash(hashStr)
        return hashID

    def debugParsing(self, fileName, program: Program):
        f = open(DebugFolder + "ParsingInfo.txt", "w+")
        debugStr = f"{fileName} parsed as:\n{str(program)}"
        f.write(debugStr)
        f.close()

    def debugRegexCompiling(self):
        f = open(DebugFolder + "RegexInfo.txt", "w+")
        f.write(self.debugRegexStr)
        f.close()

    def __init__(self, fileName, debug=False) -> None:
        self.debug = debug
        f = open(TestFileFolder + fileName)
        txt = "".join(f.readlines())
        program: Program = Parser(txt).generateAST().root

        if self.debug:
            self.debugParsing(fileName, program)

        self.players = program.players.names
        self.playersTurnIndex = 0
        # resolve pieceToPlayerDict
        self.piecesToPlayer: dict[str, str] = dict()
        for piece in program.pieces.pieceArr:
            name, player = piece.pieceName, piece.playerName
            self.piecesToPlayer[name] = player

        # resolve variables/ stateMachines/ macros/ preds/ effects
        self.effects: dict[str, Effect] = dict()
        self.functions: dict[str, Function] = dict()

        self.variables: dict[str, int] = dict()
        self.setThis({"x": 0, "y": 0, "playerName": None})
        self.variables["Board"] = [
            [None for _ in range(program.board.y)] for _ in range(program.board.x)
        ]

        self.macros: dict[str, Macro] = dict()
        self.pieceRules: dict[str, DFAStateMachine] = dict()
        self.pieceRules["None"] = None
        self.boardPieces: dict[str, BoardPiece] = dict()
        self.victoryConditions: dict[str, Victory] = dict()

        # Resolving rules
        self.debugRegexStr = ""

        variableDeclarations = []
        for rule in program.rules.ruleArr:
            if type(rule) == VariableDeclaration:
                # load globals after compile
                variableDeclarations.append(rule)
            else:
                visitFunc = {
                    Function: self.visitFunction,
                    Effect: self.visitEffect,
                    Macro: self.visitMacro,
                    PieceRule: self.visitPieceRule,
                    Victory: self.visitVictory,
                }[type(rule)]
                visitFunc(rule)

        for variableDeclaration in variableDeclarations:
            self.visitVariableDeclaration(variableDeclaration)

        if self.debug:
            self.debugRegexCompiling()

        # Load pieces into board
        for y in range(program.board.y):
            for x in range(program.board.x):
                pieceName = program.board.board[x + program.board.x * y]
                if pieceName == "None":
                    self.setTile(x, program.board.y - y - 1, None)
                else:
                    dfa = self.pieceRules[pieceName]
                    playerName = self.piecesToPlayer[pieceName]
                    self.setTile(
                        x,
                        program.board.y - y - 1,
                        BoardPiece(dfa, pieceName, playerName),
                    )

        # Memory for undo
        self.changesStack: list[
            tuple[Letter, Position, BoardPiece, EngineBoard | None]
        ] = []
        self.moveStack: list[int] = []

        # Hash for pieces
        self.pieceNameToHash = dict()
        for i, piece in enumerate(program.pieces.pieceArr):
            name, player = piece.pieceName, piece.playerName
            self.pieceNameToHash[name] = i + 1

    # --- Resolve rules functions ---
    def visitVictory(self, victoryCondition: Victory):
        if victoryCondition.name not in self.players:
            raise Exception(
                f"Invalid victory condition:\n{str(victoryCondition)}\n Player {victoryCondition.name} does not exist!"
            )
        self.victoryConditions[victoryCondition.name] = victoryCondition

    def visitFunction(self, function: Function):
        if type(function.logic) == RegexTree:
            regex = self.compileRegex(
                function.logic
            )  # DOES NOT ALLOW FOR ARGUMENTS FOR THOSE PREDICATES
            dfa = RegexToDFA(regex)
            function.logic = dfa
            self.functions[function.name] = function
        elif type(function.logic) == IntegerExpressionTree:
            self.functions[function.name] = function
        else:
            raise Exception(f"Invalid type! {type(function.logic)}")

    def visitEffect(self, effect: Effect):
        self.effects[effect.name] = effect

    def visitMacro(self, macro: Macro):
        self.macros[macro.name] = macro

    def visitIntegerExpression(
        self, integerExpression: IntegerExpressionTree, env: list[str:int] = {}
    ):
        intExp = writeVariablesToConstants(integerExpression, self.variables)
        intExp = resolveConstants(intExp)
        integerExpression.rootNode = intExp.rootNode

    def visitPieceRule(self, pieceRule: PieceRule):
        try:
            compiledRegex = self.compileRegex(pieceRule.regex)
        except Exception as e:
            print(e)
            raise Exception(
                f"{str(e)}\nFailed to compile regex for {pieceRule.pieceName}:\n{str(pieceRule.regex)}"
            )
        dfa, debugStr = RegexToDFA(compiledRegex, debug=True)
        if self.debug:
            self.debugRegexStr += f"\n\n--Compiling regex for {pieceRule.pieceName}:\n{str(pieceRule.regex)}\n"
            self.debugRegexStr += f"\nResolved regex:\n{str(compiledRegex)}\n"
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
                # Predicates and effects arguments need to be compiled too
                self.visitIntegerExpression(letter.pre)
                if letter.effect is not None:
                    for arg in letter.effect.arguments:
                        self.visitIntegerExpression(arg)

                return Letter(dx, dy, letter.pre, effect=letter.effect)
            elif type(regexNode) == FunctionCall:
                funCall: FunctionCall = regexNode
                if funCall.name not in self.macros:
                    raise Exception(
                        f"Macro {funCall.name} called before initialization"
                    )
                macro = self.macros[funCall.name]
                argumentVariables = self.matchFunCallWithArguments(funCall, macro)
                return self.runFunctionInEnvironment(
                    lambda: self.compileRegex(macro.regex).rootNode, argumentVariables
                )
            else:
                raise Exception(f"uncaught regexNode type {type(regexNode)}")

        # clean regex
        return RegexTree(visitor(regex.rootNode))

    # --- Resolve rule functions end ---

    # --- Helper functions ---
    def getTile(self, x: int, y: int) -> BoardPiece | None:
        return self.variables["Board"][x][y]

    def setTile(self, x: int, y: int, newTileValue: BoardPiece | None):
        self.variables["Board"][x][y] = newTileValue

    def runFunctionInEnvironment(self, func, environmnet: dict[str, int]):
        # load argumentVariables into self.variables
        storeForUnload = dict()
        for key in environmnet:
            if key in self.variables:
                storeForUnload[key] = self.variables[key]
            self.variables[key] = environmnet[key]

        value = func()

        # unload argumentVariables from self.variables
        for key in environmnet:
            del self.variables[key]
            if key in storeForUnload:
                self.variables[key] = storeForUnload[key]

        return value

    def matchFunCallWithArguments(
        self, funCall: FunctionCall, funType: Macro | Effect
    ) -> dict[str, int]:
        if len(funType.arguments) != len(funCall.arguments):
            raise Exception(
                f"{str(funType)} {funCall.name} called with incorrect number of paramaters"
            )
        argumentVariables = dict()
        for i, key in enumerate(funType.arguments):
            argumentVariables[key] = self.resolveIntegerExpression(funCall.arguments[i])
        return argumentVariables

    def resolveIntegerExpression(
        self, integerExpression: IntegerExpressionTree, lookUpPiece=True
    ) -> int | BoardPiece:
        # If integer expression is loading a state machine
        rootNode = integerExpression.rootNode
        if (
            lookUpPiece
            and type(rootNode) == IntegerExpressionVariable
            and rootNode.name in self.pieceRules
        ):
            pieceName = rootNode.name
            dfa = self.pieceRules[pieceName]
            if dfa is None:
                return None
            else:
                playerName = self.piecesToPlayer[pieceName]
                return BoardPiece(dfa, pieceName, playerName)

        this = self.getThis()
        x = this["x"]
        y = this["y"]
        playerName = this["playerName"]

        specialPredicates = {
            "enemy": (self.getTile(x, y) is not None)
            and (self.getTile(x, y).player != playerName),
            "empty": self.getTile(x, y) is None,
            "true": True,
        }
        for pieceName in self.piecesToPlayer:
            specialPredicates[pieceName] = (self.getTile(x, y) is not None) and (
                self.getTile(x, y).name == pieceName
            )

        def visit(node: SyntaxTreeNode) -> int:
            if type(node) == BinaryOperator:
                binaryOp: BinaryOperator = node
                a: int = visit(binaryOp.child1)
                b: int = visit(binaryOp.child2)
                return binaryOpToFunc[binaryOp.operator](a, b)

            elif type(node) == UnaryOperator:
                unaryOp: UnaryOperator = node
                a: int = visit(unaryOp.child)
                return unaryOpToFunc[unaryOp.operator](a)

            elif type(node) == Integer:
                integer: Integer = node
                return integer.value

            elif type(node) == Variable:
                variable: Variable = node
                if variable.secondName is not None:
                    return self.variables[variable.name][variable.secondName]
                else:
                    return self.variables[variable.name]

            elif type(node) == IndexedVariable:
                indexedVariable: IndexedVariable = node
                name = indexedVariable.name
                indexableObjToValue = self.variables[name]
                indices = indexedVariable.indices[::-1]
                while len(indices) != 0:
                    indexableObjToValue = indexableObjToValue[visit(indices.pop())]
                return indexableObjToValue

            elif type(node) == FunctionCall:
                funCall: FunctionCall = node
                name = funCall.name
                args = funCall.arguments

                function: Function = self.functions[name]

                if len(function.arguments) != len(funCall.arguments):
                    raise Exception(
                        f"{str(function)} {funCall.name} called with incorrect number of paramaters"
                    )
                argumentVariables = dict()
                for i, key in enumerate(function.arguments):
                    argumentVariables[key] = visit(funCall.arguments[i])

                f = lambda: visit(function.logic)
                return self.runFunctionInEnvironment(f, argumentVariables)
            elif type(node) == DFAStateMachine:
                dfa: DFAStateMachine = node
                return len(self.getStateMachineMoves(dfa, (x, y), walkPiece=False)) != 0
            elif type(node) == IntegerExpressionTree:
                expr: IntegerExpressionTree = node
                return self.resolveIntegerExpression(expr, lookUpPiece=lookUpPiece)
            else:
                raise Exception(f"Invalid type: {type(node)}!")

        f = lambda: visit(integerExpression.rootNode)
        value = self.runFunctionInEnvironment(f, specialPredicates)
        # print(integerExpression.rootNode, value)
        return value
        # return visit(integerExpression.rootNode)

    def setThis(self, thisVariables: dict[str:int]):
        self.variables["this"] = thisVariables

    def getThis(self):
        return self.variables["this"]

    # --- Helper functions end ---

    # --- Functions for calculating move effect ---
    def playLetter(self, letter: Letter, location: Position, walkPiece=True):
        x, y = location
        dx, dy = letter.dx, letter.dy

        thingUnderMoveLocation = self.getTile(x + dx, y + dy)
        piece = self.getTile(x, y)
        if walkPiece:
            self.setTile(x, y, None)
            self.setTile(x + dx, y + dy, piece)

        currentVariables: dict[str:int] = None
        if letter.effect is not None:
            currentVariables = deepcopy(self.variables)
            self.setThis({"x": x + dx, "y": y + dy, "playerName": piece.player})
            self.runEffect(letter.effect)

        self.changesStack.append(
            (letter, location, thingUnderMoveLocation, currentVariables)
        )

    def undoLetter(self, walkPiece=True):
        letter, location, thingUnderMoveLocation, oldVariables = self.changesStack.pop()

        if oldVariables is not None:
            self.variables = oldVariables

        if walkPiece:
            x, y = location
            dx, dy = letter.dx, letter.dy
            self.setTile(x, y, self.getTile(x + dx, y + dy))
            self.setTile(x + dx, y + dy, thingUnderMoveLocation)

    def playMove(self, move: Move):
        self.moveStack.append(len(move.letterArr))
        x, y = move.startPos
        dx, dy = 0, 0
        for letter in move.letterArr:
            x += dx
            y += dy
            dx, dy = letter.dx, letter.dy
            self.playLetter(letter, (x, y))
        self.playersTurnIndex = (self.playersTurnIndex + 1) % len(self.players)

    def execute_move(self, move):
        return self.playMove(move)

    def undoMove(self):
        moveLength = self.moveStack.pop()
        for i in range(moveLength):
            self.undoLetter()
        self.playersTurnIndex = (self.playersTurnIndex - 1) % len(self.players)

    def undo_move(self):
        return self.undoMove()

    def runEffect(self, effectCall: FunctionCall):
        EffectName = effectCall.name
        effectType = self.effects[EffectName]

        if EffectName not in self.effects:
            raise Exception(f"Effect {EffectName} called before initialization")
        effectType = self.effects[EffectName]
        argumentVariables = self.matchFunCallWithArguments(effectCall, effectType)

        # run assignments
        def runAssignments():
            for assignmnet in effectType.assignments:
                self.runAssignment(assignmnet)

        self.runFunctionInEnvironment(runAssignments, argumentVariables)

    def runAssignment(self, assignment: Assignment):
        try:
            variables = self.variables

            indexes = [
                assignment.variableName,
                *[
                    self.resolveIntegerExpression(indexExpr)
                    for indexExpr in assignment.indexes
                ],
            ]
            indexes = indexes[::-1]

            while len(indexes) > 1:
                variables = variables[indexes.pop()]

            variables[indexes.pop()] = self.resolveIntegerExpression(
                assignment.newValue
            )
        except Exception as error:
            print("Error running assignment:")
            print(assignment)
            print("Reason:")
            print(error)
            raise error

    # --- Functions for calculating move effect end ---

    # --- Calculating legal moves ---#
    def getPlayerMoves(self) -> list[Move]:
        player = self.playersTurn()
        moves: list[Move] = []
        for x in range(len(self.variables["Board"])):
            for y in range(len(self.variables["Board"][0])):
                tile = self.getTile(x, y)
                if tile is None or tile.player != player:
                    continue
                piece: BoardPiece = tile
                # self.setThis({'x':x, 'y':y, 'playerName': player})
                pieceMoves = self.getStateMachineMoves(piece.stateMachine, (x, y))
                moves.extend(pieceMoves)
        return moves

    def generate_moves(self):
        return self.getPlayerMoves()

    def getStateMachineMoves(
        self, stateMachine: DFAStateMachine, location: Position, walkPiece=True
    ) -> list[Move]:
        x, y = location
        boardPiece = self.getTile(x, y)
        player = boardPiece.player
        self.setThis({"x": x, "y": y, "playerName": player})

        def getStateMoves(state: State, location: Position) -> list[list[Letter]]:
            x, y = location
            boardPiece = self.getTile(x, y)
            if boardPiece is not None:
                # Effect killed its own piece, abort!!!! >:(
                player = boardPiece.player
            else:
                player = self.getThis()["playerName"]
            moveList = []

            for transition in state.outTransitions:
                letter = transition.letter
                dx, dy = letter.dx, letter.dy
                newLocation = (x + dx, y + dy)

                inRange = (0 <= x + dx < len(self.variables["Board"])) and (
                    0 <= y + dy < len(self.variables["Board"][0])
                )
                self.setThis({"x": x + dx, "y": y + dy, "playerName": player})
                if not (
                    inRange
                    and self.resolveIntegerExpression(letter.pre, lookUpPiece=False)
                ):
                    continue

                if transition.end.final:
                    moveList.append([letter])

                self.playLetter(letter, location, walkPiece=walkPiece)

                movesFromThisTransition = [
                    [letter, *move]
                    for move in getStateMoves(transition.end, newLocation)
                ]
                moveList.extend(movesFromThisTransition)

                self.undoLetter(walkPiece=walkPiece)

            return moveList

        moves = [
            Move(letterArr, location)
            for letterArr in getStateMoves(stateMachine.start, location)
        ]
        return moves

    # --- Calculating legal moves end ---#

    def __str__(self):
        maxSize = 0
        for y in range(len(self.variables["Board"][0]) - 1, -1, -1):
            for x in range(len(self.variables["Board"])):
                tile = self.getTile(x, y)
                maxSize = max(len("e" if tile is None else tile.name), maxSize)
        returnStr = f"Players turn: {self.playersTurn()}\n"
        for y in range(len(self.variables["Board"][0]) - 1, -1, -1):
            for x in range(len(self.variables["Board"])):
                piece = self.getTile(x, y)
                if piece is None:
                    temp = "e"
                else:
                    temp = piece.name
                returnStr += temp.rjust(maxSize) + " "
            returnStr += "\n"
        return returnStr

    def getGameState(self) -> GameState:
        winner = "E:)-|-<"
        numberOfWins = 0
        for player in self.players:
            victoryCondition = self.victoryConditions[player]
            self.setThis({"x": 0, "y": 0, "playerName": None})
            if self.resolveIntegerExpression(victoryCondition.expression):
                numberOfWins += 1
                winner = player

        if numberOfWins == 1:
            return Win(winner)
        elif numberOfWins > 1 or len(self.getPlayerMoves()) == 0:
            return Draw()
        elif numberOfWins == 0:
            return Unresolved()

    def game_over(self):
        return not (type(self.getGameState()) in [Unresolved])

    def playersTurn(self):
        return self.players[self.playersTurnIndex]

    def get_current_player(self):
        return self.playersTurn()

    def get_first_player(self):
        return self.players[0]

    def reset(self):
        while len(self.moveStack) != 0:
            self.undoMove()

    def get_environment_representation(self):
        raise NotImplementedError

    def get_white_score(self):
        raise NotImplementedError

    def jsonify(self: GameEngine):
        gameEngine = self
        board = [
            [piece.name if piece is not None else "None" for piece in line]
            for line in gameEngine.variables["Board"]
        ]
        moves = (
            "["
            + ",".join(
                [
                    self.gameEngineMoveToJSON(move)
                    for move in gameEngine.getPlayerMoves()
                ]
            )
            + "]"
        )
        message = f'{{"board":  {str(board)}, "moves": {moves}}}'
        return simpleObjToJson(message)

    def gameEngineMoveToJSON(self, move: Move):
        letters = (
            "["
            + ",".join(
                [self.gameEngineLetterToJSON(letter) for letter in move.letterArr]
            )
            + "]"
        )
        return simpleObjToJson(
            f'{{"start":  {str(move.startPos)}, "letters": {letters}}}'
        )

    def gameEngineLetterToJSON(self, letter: Letter):
        return simpleObjToJson(
            {
                "dx": letter.dx,
                "dy": letter.dy,
                "effect": str(letter.effect).replace("(", "").replace(")", ""),
            }
        )

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

            if type(move) == str and move in "qu":
                if move == "q":
                    break
                elif move == "u":
                    self.undoMove()
                    continue

            if self.debug:
                print("Player ", player, "on board")
                print(self)
                print("Picked move:", move)
            self.playMove(move)

            gameState = self.getGameState()
            if type(gameState) in (Draw, Win):
                if self.debug:
                    print(gameState)
                return gameState


def simpleObjToJson(simpleObj):
    # simpleObj array[simpleObj], tuple[simpleObj], string or int
    replaceInfo = {"(": "[", ")": "]", "'": '"'}

    returnStr = str(simpleObj)
    for key in replaceInfo:
        returnStr = returnStr.replace(key, replaceInfo[key])
    return returnStr


"""
python -m ru_final_project.GameEngine.PythonGameEngine
"""
from ..Agents.browser_gui_agent import BrowserGUIAgent
from ..Agents.random_agent import RandomAgent

if __name__ == "__main__":
    print("hello world")
    ge = PythonGameEngine("chess.game", debug=False)
    agents = []
    agents.append(BrowserGUIAgent(8080))
    agents.append(RandomAgent())
    # agents.append(TUIAgent())
    # agents.append(RandomAgent())
    result = ge.run(agents)
    print(result)
