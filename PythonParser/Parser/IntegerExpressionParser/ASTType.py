from __future__ import annotations
from ...Lexer.TokenTypes import Token


def indent(string: str) -> str:
    return '\t' + '\n\t'.join(string.split('\n'))

class IntegerExpressionTree:
    def __init__(self, rootNode: SyntaxTreeNode) -> None:
        self.rootNode = rootNode

    def __str__(self) -> str:
        return 'Integer Expression Tree:\n'+indent(str(self.rootNode))

class SyntaxTreeNode:
    pass

class BinaryOperator(SyntaxTreeNode):
    symbols = ['*', '+', '-', '/', '//', '%', 'and', 'or', '!=', '==', '&&', '||']
    def __init__(self, child1: SyntaxTreeNode, child2: SyntaxTreeNode, operator: str) -> None:
        self.child1 = child1
        self.child2 = child2
        self.operator: str = operator

    def __str__(self) -> str:
        returnStr = ''
        returnStr +=  self.operator + '\n'
        childStr =  str(self.child1) + '\n'
        childStr +=  str(self.child2)
        return returnStr + indent(childStr)

class UnaryOperator(SyntaxTreeNode):
    symbols = ['not', '!', '-']
    def __init__(self, child: SyntaxTreeNode, operator: str) -> None:
        self.child = child
        self.operator: str = operator

    def __str__(self) -> str:
        returnStr = ''
        returnStr +=  self.operator + '\n'
        childStr =  str(self.child)
        return returnStr + indent(childStr)


class Integer(SyntaxTreeNode):
    def __init__(self, value: int) -> None:
        self.value: int = value

    def __str__(self):
        return str(self.value)

class Variable(SyntaxTreeNode):
    def __init__(self, name: str, secondName: (str | None) = None) -> None:
        self.name: str = name
        self.secondName: (str | None) = secondName

    def __str__(self):
        return self.name + (('.' + self.secondName) if self.secondName is not None else '')
        