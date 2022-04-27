from __future__ import annotations
from ..Parser.IntegerExpressionParser.ASTType import *
from ..Parser.IntegerExpressionParser.Parser import Parser

binaryOpToFunc = {
    'and': lambda a, b: a and b,
    'or': lambda a, b: a or b,
    '&&': lambda a, b: a and b,
    '||': lambda a, b: a or b,
    '!=': lambda a, b: a != b,
    '==': lambda a, b: a == b,
    '+': lambda a, b: a + b,
    '-': lambda a, b: a - b,
    '*': lambda a, b: a * b,
    '/':lambda a, b: a / b,
    '//':lambda a, b: a // b, 
    '%':lambda a, b: a % b
}

unaryOpToFunc = {
    'not': lambda a: not a,
    '!': lambda a: not a,
    '-': lambda a: -a
}

Environment = dict[str, int]
Environment = dict[str, int | Environment]


def visitor(node: SyntaxTreeNode, environment: Environment) -> int:
    if type(node) == BinaryOperator:
        binaryOp: BinaryOperator = node
        a: int = visitor(binaryOp.child1, environment)
        b: int = visitor(binaryOp.child2, environment)
        return binaryOpToFunc[binaryOp.operator](a, b)
    
    elif type(node) == UnaryOperator:
        unaryOp: UnaryOperator = node
        a: int = visitor(unaryOp.child, environment)
        return unaryOpToFunc[unaryOp.operator](a)
    
    elif type(node) == Integer:
        integer: Integer = node
        return integer.value

    elif type(node) == Variable:
        variable: Variable = node
        if variable.secondName is not None:
            return environment[variable.name][variable.secondName]
        else:
            return environment[variable.name]


def evaluateIntegerExpression(intExpr: SyntaxTreeNode, environment: Environment):
    return visitor(intExpr, environment)
    
'''
C:/Users/gudmu/AppData/Local/Programs/Python/Python310/python.exe -m ru_final_project.PythonParser.NewFolderStruct.IntegerExpression.Evaluator
'''      

if __name__ == '__main__':
    text = '1+2*3+5 and 4==x'
    ast = Parser(text).generateAST()
    print(evaluateIntegerExpression(ast, {'x':5}))