from __future__ import annotations

from ...PythonParser.Parser.ASTType import Function, FunctionCall

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


def visit(node: SyntaxTreeNode, environment: Environment) -> int:
    if type(node) == BinaryOperator:
        binaryOp: BinaryOperator = node
        a: int = visit(binaryOp.child1, environment)
        b: int = visit(binaryOp.child2, environment)
        return binaryOpToFunc[binaryOp.operator](a, b)
    
    elif type(node) == UnaryOperator:
        unaryOp: UnaryOperator = node
        a: int = visit(unaryOp.child, environment)
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
    
    elif type(node) == IndexedVariable:
        indexedVariable: IndexedVariable = node
        name = indexedVariable.name
        indexableObjToValue = environment[name]
        indices = indexedVariable.indices[::-1]
        while len(indices) != 0:
            indexableObjToValue = indexableObjToValue[visit(indices.pop(), environment)]
        return indexableObjToValue

    elif type(node) == FunctionCall:
        raise Exception(f"Cant compile FunctionCall {str(node)}")
    else:
        raise Exception(f"Invalid type in integer expression! {type(node)}")



def evaluateIntegerExpression(intExpr: SyntaxTreeNode, environment: Environment):
    return visit(intExpr, environment)
    
'''
C:/Users/gudmu/AppData/Local/Programs/Python/Python310/python.exe -m ru_final_project.PythonParser.NewFolderStruct.IntegerExpression.Evaluator
'''      

if __name__ == '__main__':
    text = '1+2*3+5 and 4==x'
    ast = Parser(text).generateAST()
    print(evaluateIntegerExpression(ast, {'x':5}))