from __future__ import annotations
from ...Lexer.TokenTypes import Token


class RegexTree:
    def __init__(self, rootNode: SyntaxTreeNode) -> None:
        self.rootNode = rootNode

    def __str__(self) -> str:
        return 'REGEX TREE:\n'+str(self.rootNode)

class SyntaxTreeNode:
    def indent(self, string: str) -> str:
        return '\t' + '\n\t'.join(string.split('\n'))

class FunctionCall(SyntaxTreeNode):
    def __init__(self, name: str, arguments: list[IntegerExpression]) -> None:
        self.name: str = name
        self.arguments: list[IntegerExpression] = arguments

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
        returnStr +=  str(self.child1) + '\n'
        returnStr +=  str(self.child2)
        return self.indent(returnStr)

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
        returnStr +=  str(self.child)
        return self.indent(returnStr)




class Star(UnarySymbol):
    pass

class QuestionMark(UnarySymbol):
    pass

class Plus(UnarySymbol):
    pass

class Letter(SyntaxTreeNode):
    def __init__(self, dx, dy, pre, effect=None) -> None:
        self.dx = dx
        self.dy = dy
        self.pre = pre
        self.effect: FunctionCall = effect

    def __str__(self) -> str:
        returnStr = ''
        returnStr +=  f'({str(self.dx)}; {str(self.dy)}; {str(self.pre)})'
        if self.effect is not None:
            returnStr += '{' + str(self.effect) + '}'
        return self.indent(returnStr)

class IntegerExpression(SyntaxTreeNode):
    def __init__(self, tokenList: list[Token]) -> None:
        self.tokenList: list[Token] = tokenList

    def __str__(self) -> str:
        return ''.join([str(token.value) for token in self.tokenList])

class Variable(SyntaxTreeNode):
    def __init__(self, name: str) -> None:
        self.name = name
        