from __future__ import annotations
from ...Lexer.TokenTypes import Token


def indent(string: str) -> str:
    return '\t' + '\n\t'.join(string.split('\n'))

class RegexTree:
    def __init__(self, rootNode: SyntaxTreeNode) -> None:
        self.rootNode = rootNode

    def __str__(self) -> str:
        return 'REGEX TREE:\n'+indent(str(self.rootNode))

    

class SyntaxTreeNode:
    pass

class FunctionCall(SyntaxTreeNode):
    def __init__(self, name: str, arguments: list[IntegerExpressionTree]) -> None:
        self.name: str = name
        self.arguments: list[IntegerExpressionTree] = arguments

    def getChildren(self) -> list[IntegerExpressionTree]:
        return self.arguments

    def setChildren(self, children: list[IntegerExpressionTree]):
        self.arguments = children

    def __str__(self) -> str:
        return f'{self.name}({",".join([str(intExp) for intExp in self.arguments])})'


class BinarySymbol(SyntaxTreeNode):
    symbols = ['|']
    def __init__(self, child1: SyntaxTreeNode, child2: SyntaxTreeNode) -> None:
        self.child1 = child1
        self.child2 = child2

    def __str__(self) -> str:
        returnStr = ''
        returnStr +=  str(type(self)).split('.')[-1].split("'")[0] + '\n'
        childStr =  str(self.child1) + '\n'
        childStr +=  str(self.child2)
        return returnStr + indent(childStr)

class Union(BinarySymbol):
    pass

class Concatination(BinarySymbol):
    pass

class UnarySymbol(SyntaxTreeNode):
    symbols = ['*', '?', '+']
    def __init__(self, child: SyntaxTreeNode) -> None:
        self.child = child

    def __str__(self) -> str:
        returnStr = ''
        returnStr +=  str(type(self)).split('.')[-1].split("'")[0] + '\n'
        childStr =  str(self.child)
        return returnStr + indent(childStr)




class Star(UnarySymbol):
    pass

class QuestionMark(UnarySymbol):
    pass

class Plus(UnarySymbol):
    pass


from ..IntegerExpressionParser.ASTType import IntegerExpressionTree
class Letter(SyntaxTreeNode):
    def __init__(self, dx, dy, pre, effect=None) -> None:
        self.dx: int = dx
        self.dy: int = dy
        self.pre: FunctionCall | IntegerExpressionTree = pre
        self.effect: FunctionCall = effect

    def __str__(self) -> str:
        returnStr = ''
        returnStr +=  f'({str(self.dx)}; {str(self.dy)}; {str(self.pre)})'
        if self.effect is not None:
            returnStr += '{' + str(self.effect) + '}'
        return returnStr

    def __eq__(self, other: Letter) -> bool:
        return str(self) == str(other)

    def __hash__(self):
        return hash(str(self))

class Variable(SyntaxTreeNode):
    def __init__(self, name: str) -> None:
        self.name = name
        