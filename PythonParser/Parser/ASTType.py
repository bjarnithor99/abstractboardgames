from __future__ import annotations
from .RegexParser.ASTType import *


class MatchFailed(Exception):
    pass

class AST:
    def __init__(self, root: ASTNode) -> None:
        self.root: ASTNode = root

    def __str__(self):
        return str(self.root)


class ASTNode:
    def indent(self, string: str) -> str:
        return '\t' + '\n\t'.join(string.split('\n'))

class Program(ASTNode):
    def __init__(self, players: Players, pieces: Pieces, rules: Rules, board: Board) -> None:
        self.players: Players = players
        self.pieces: Pieces = pieces
        self.rules: Rules = rules
        self.board: Board = board

    def __str__(self):
        returnStr = ''
        returnStr += 'Players:' +'\n'
        returnStr += '\t' + str(self.players)+'\n'
        returnStr += 'Pieces:'+'\n'
        returnStr += '\t' +str(self.pieces)+'\n'
        returnStr += 'Rules:'+'\n'
        returnStr += self.indent(str(self.rules)) + '\n'
        returnStr += 'Board:'+'\n'
        returnStr += str(self.board)
        return returnStr
        
class Players(ASTNode):
    def __init__(self, names: list[str]) -> None:
        self.names: list[str] = names

    def __str__(self):
        return str(self.names)


class Pieces(ASTNode):
    def __init__(self, pieceArr: list[Piece]) -> None:
        self.pieceArr: list[Piece] = pieceArr

    def __str__(self):
        return ','.join([str(piece) for piece in self.pieceArr])

class Piece(ASTNode):
    def __init__(self, pieceName: str, playerName: str) -> None:
        self.pieceName: str = pieceName
        self.playerName: str = playerName

    def __str__(self) -> str:
        return f"PIECE({self.pieceName},{self.playerName})"

    

class Rules(ASTNode):
    def __init__(self, ruleArr: list[Rule]) -> None:
        self.ruleArr: list[Rule] = ruleArr

    def __str__(self):
        return '\n'.join([str(rule) for rule in self.ruleArr])

class Rule(ASTNode):
    pass

class Predicate(Rule):
    def __init__(self, name: str, arguments: list[str], expression: str) -> None:
        self.name: str = name
        self.arguments: list[str] = arguments
        self.expression: str = expression

    def __str__(self) -> str:
        argumentStr = ",".join(self.arguments)
        returnStr = f'PREDICATE {self.name}({argumentStr}):\n\t'
        returnStr += str(self.expression)
        return returnStr

class Victory(Rule):
    def __init__(self, name: str, expression: str) -> None:
        self.name: str = name
        self.expression: str = expression

    def __str__(self) -> str:
        returnStr = f'VICTORY {self.name}:\n\t'
        returnStr += str(self.expression)
        return returnStr

class Effect(Rule):
    def __init__(self, name: str, arguments: list[str], assignments: list[Assignment]) -> None:
        self.name: str = name
        self.arguments: list[str] = arguments
        self.assignments: list[Assignment] = assignments

    def __str__(self) -> str:
        argumentStr = ",".join(self.arguments)
        returnStr = f'EFFECT {self.name}({argumentStr}):\n'
        for assignment in self.assignments:
            returnStr += '\t' + str(assignment) + '\n'
        return returnStr[:-1]

class Assignment(ASTNode):
    def __init__(self, variableName: str, indexes: list[IntegerExpression], newValue: IntegerExpression) -> None:
        self.variableName: str = variableName
        self.indexes: list[IntegerExpression] = indexes
        self.newValue: IntegerExpression = newValue

    def __str__(self):
        indexStr = ''.join([f'[{expr}]' for expr in self.indexes])
        returnStr = f'{self.variableName}{indexStr} = {str(self.newValue)}'
        return returnStr

    

class Macro(Rule):
    def __init__(self, name: str, arguments: list[str], regex: RegexTree) -> None:
        self.name: str = name
        self.arguments: list[str] = arguments
        self.regex: RegexTree = regex

    def __str__(self):
        argumentStr = ",".join(self.arguments)
        returnStr = f'MACRO {self.name}({argumentStr}):\n'
        returnStr += str(self.regex)
        return returnStr

        

class PieceRule(Rule):
    def __init__(self, pieceName: str, regex: RegexTree) -> None:
        self.pieceName: str = pieceName
        self.regex: RegexTree = regex

    def __str__(self):
        returnStr = f'RULE {self.pieceName}:\n'
        returnStr += str(self.regex)
        return returnStr

class VariableDeclaration(Rule):
    def __init__(self, name:str, value: IntegerExpression) -> None:
        self.name: str = name
        self.value: IntegerExpression = value

    def __str__(self):
        returnStr = f'VARIABLE {self.name} = {str(self.value)}'
        return returnStr
        
        

class Board(ASTNode):
    def __init__(self, x: int, y: int, board: list[str]) -> None:
        self.x: int = x
        self.y: int = y
        self.board: list[str] = board

    def __str__(self):
        returnStr = f'\tx:{self.x}, y:{self.y}\n'

        for y in range(self.y):
            returnStr += '\t'
            for x in range(self.x):
                i = y*self.x + x
                returnStr += str(self.board[i]) + ', '
            returnStr = returnStr[:-2] + '\n'
        return returnStr



class Regex(ASTNode):
    def __init__(self, regexSyntaxTree: RegexTree) -> None:
        self.regexSyntaxTree: RegexTree = regexSyntaxTree