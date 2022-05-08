from ru_final_project.PythonParser.Parser.RegexParser.ASTType import FunctionCall
from ..Parser.IntegerExpressionParser.ASTType import *
def writeVariablesToConstants(integerExpression: IntegerExpressionTree, env: dict[str: int]) -> IntegerExpressionTree:
    def visitor(node: SyntaxTreeNode, env: dict[str: int]):
        children = []
        for child in node.getChildren():
            children.append(visitor(child, env))
        node.setChildren(children)
        if isinstance(node, Variable) and (node.secondName is None) and (node.name in env):
            return Integer(env[node.name])
        return node
    return IntegerExpressionTree(visitor(integerExpression.rootNode, env))


from .Evaluator import evaluateIntegerExpression
def resolveConstants(integerExpression: IntegerExpressionTree) -> IntegerExpressionTree:
    def visitor(node: SyntaxTreeNode):
        children = []
        for child in node.getChildren():
            children.append(visitor(child))
        node.setChildren(children)
        if all([isinstance(child, Integer) for child in node.getChildren()]) and not (isinstance(node, Variable) or isinstance(node, FunctionCall)):
            return Integer(evaluateIntegerExpression(node, {}))
        return node
    return IntegerExpressionTree(visitor(integerExpression.rootNode))



'''
python -m ru_final_project.PythonParser.IntegerExpression.Preprocessor
'''
if __name__ == "__main__":
    from ...PythonParser.Parser.IntegerExpressionParser.Parser import Parser
    ast = Parser('x+y*3 + 8').generateAST()
    print(ast)
    ast = writeVariablesToConstants(ast, {'xi': 9, 'y': 200})
    print(ast)
    ast = resolveConstants(ast)
    print(ast)
    
    