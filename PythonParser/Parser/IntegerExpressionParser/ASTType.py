from __future__ import annotations
from ...Lexer.TokenTypes import Token


def indent(string: str) -> str:
    return '\t' + '\n\t'.join(string.split('\n'))



INDENT = False

class IntegerExpressionTree:
    def __init__(self, rootNode: SyntaxTreeNode) -> None:
        self.rootNode = rootNode

    def getChildren(self) -> list[SyntaxTreeNode]:
        return self.rootNode

    def setChildren(self, children: list[SyntaxTreeNode]):
        self.rootNode = children[0]

    def __str__(self) -> str:
        return ('Integer Expression Tree:\n'+indent(str(self.rootNode))) if INDENT else f'EXP[{str(self.rootNode)}]'

class SyntaxTreeNode:
    def getChildren(self) -> list[SyntaxTreeNode]:
        pass

    def setChildren(self, children: list[SyntaxTreeNode]):
        pass


class BinaryOperator(SyntaxTreeNode):
    symbols = ['*', '+', '-', '/', '//', '%', 'and', 'or', '!=', '==', '&&', '||']
    def __init__(self, child1: SyntaxTreeNode, child2: SyntaxTreeNode, operator: str) -> None:
        self.child1 = child1
        self.child2 = child2
        self.operator: str = operator

    def getChildren(self) -> list[SyntaxTreeNode]:
        return [self.child1, self.child2]

    def setChildren(self, children: list[SyntaxTreeNode]):
        self.child1 = children[0]
        self.child2 = children[1]

    def __str__(self) -> str:
        if not INDENT:
            return f'{str(self.child1)} {self.operator} {str(self.child2)}'
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

    def getChildren(self) -> list[SyntaxTreeNode]:
        return [self.child]

    def setChildren(self, children: list[SyntaxTreeNode]):
        self.child = children[0]

    def __str__(self) -> str:
        if not INDENT:
            return f'{self.operator} {str(self.child)}'
        returnStr = ''
        returnStr +=  self.operator + '\n'
        childStr =  str(self.child)
        return returnStr + indent(childStr)


class Integer(SyntaxTreeNode):
    def __init__(self, value: int) -> None:
        self.value: int = value

    def getChildren(self) -> list[SyntaxTreeNode]:
        return []

    def __str__(self):
        return str(self.value)

class Variable(SyntaxTreeNode):
    def __init__(self, name: str, secondName: (str | None) = None) -> None:
        self.name: str = name
        self.secondName: (str | None) = secondName

    def getChildren(self) -> list[SyntaxTreeNode]:
        return []

    def __str__(self):
        return self.name + (('.' + self.secondName) if self.secondName is not None else '')

class IndexedVariable(SyntaxTreeNode):
    def __init__(self, name: str, indices: list[SyntaxTreeNode]) -> None:
        self.name: str = name
        self.indices: list[SyntaxTreeNode] = indices

    def getChildren(self) -> list[SyntaxTreeNode]:
        return self.indices

    def setChildren(self, children: list[SyntaxTreeNode]):
        self.indices = children

    def __str__(self):
        return self.name + ''.join([f'[{str(i)}]' for i in self.indices])
        