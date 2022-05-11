from __future__ import annotations
from .RegexParser.ASTType import RegexTree
from .IntegerExpressionParser.ASTType import IntegerExpressionTree


class MatchFailed(Exception):
    pass


class ProgramAST:
    def __init__(self, root: ASTNode) -> None:
        self.root: ASTNode = root

    def __str__(self):
        return 'ProgramAST:\n' + indent(str(self.root))


def indent(string: str) -> str:
    return '\t' + '\n\t'.join(string.split('\n'))

def strList(arr: list[any]) -> str:
    return '\n'.join([str(item) for item in arr])

class ASTNode:
    pass

class Program(ASTNode):
    def __init__(self,
    players: list[Player],
    pieces: list[Piece],
    definitions: list[Definition],
    turns: list[Turn],
    victories: list[Victory],
    board: Board) -> None:
        self.players: list[Player] = players
        self.pieces: list[Piece] = pieces
        self.definitions: list[Definition] = definitions
        self.turns: list[Turn] = turns
        self.victories: list[Victory] = victories
        self.board: Board = board

    def __str__(self):
        returnStr = ''
        for name, obj in (
        ('Players', strList(self.players)),
        ('Pieces', strList(self.pieces)),
        ('Definitions', strList(self.definitions)),
        ('Turns', strList(self.turns)), 
        ('Victories', strList(self.victories)), 
        ('Board', self.board)):
            returnStr += name + ':\n' + indent(str(obj)) + '\n\n'
        
        return returnStr



class Player(ASTNode):
    def __init__(self, name: str) -> None:
        self.name: str = name

    def __str__(self):
        return 'Player: ' + str(self.name)

class Piece(ASTNode):
    def __init__(self, pieceName: str, playerName: str) -> None:
        self.pieceName: str = pieceName
        self.playerName: str = playerName

    def __str__(self) -> str:
        return f"PIECE({self.pieceName},{self.playerName})"




class Definition(ASTNode):
    pass

class Function(Definition):
    def __init__(self, name: str, arguments: list[str], logic: IntegerExpressionTree | RegexTree) -> None:
        self.name: str = name
        self.arguments: list[str] = arguments
        self.logic: IntegerExpressionTree | RegexTree = logic

    def __str__(self) -> str:
        argumentStr = ",".join(self.arguments)
        returnStr = f'Function {self.name}({argumentStr}):\n\t'
        returnStr += str(self.logic)
        return returnStr

class EffectDefinition(Definition):
    def __init__(self, name: str, arguments: list[str], assignments: list[Assignment]) -> None:
        self.name: str = name
        self.arguments: list[str] = arguments
        self.assignments: list[Assignment] = assignments

    def __str__(self) -> str:
        argumentStr = ",".join(self.arguments)
        returnStr = f'Effect {self.name}({argumentStr}):\n'
        for assignment in self.assignments:
            returnStr += '\t' + str(assignment) + '\n'
        return returnStr[:-1]

class Assignment(ASTNode):
    def __init__(self, variableName: str, indexes: list[IntegerExpressionTree], newValue: IntegerExpressionTree) -> None:
        self.variableName: str = variableName
        self.indexes: list[IntegerExpressionTree] = indexes
        self.newValue: IntegerExpressionTree = newValue

    def __str__(self):
        indexStr = ''.join([f'[{expr}]' for expr in self.indexes])
        returnStr = f'{self.variableName}{indexStr} = {str(self.newValue)}'
        return returnStr

    
class RegexDefinition(Definition):
    def __init__(self, name: str, arguments: list[str], regex: RegexTree) -> None:
        self.name: str = name
        self.arguments: list[str] = arguments
        self.regex: RegexTree = regex

    def __str__(self):
        argumentStr = ",".join(self.arguments)
        returnStr = f'Regex {self.name}({argumentStr}):\n'
        returnStr += str(self.regex)
        return returnStr


class VariableDefinition(Definition):
    def __init__(self, name:str, value: IntegerExpressionTree) -> None:
        self.name: str = name
        self.value: IntegerExpressionTree = value

    def __str__(self):
        returnStr = f'VARIABLE {self.name} = {str(self.value)}'
        return returnStr




class Turn(ASTNode):
    def __init__(self, name: str, regex: RegexTree) -> None:
        self.name: str = name
        self.regex: RegexTree = regex

    def __str__(self) -> str:
        returnStr = f'Turn {self.name}:\n'
        returnStr += str(self.regex)
        return returnStr

class Victory(ASTNode):
    def __init__(self, name: str, expression: IntegerExpressionTree) -> None:
        self.name: str = name
        self.expression: IntegerExpressionTree = expression

    def __str__(self) -> str:
        returnStr = f'Victory {self.name}: '
        returnStr += str(self.expression)
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