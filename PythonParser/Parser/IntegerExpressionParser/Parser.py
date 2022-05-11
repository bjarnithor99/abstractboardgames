from .ASTType import *
from ..ASTType import MatchFailed
from ..RegexParser.ASTType import FunctionCall
from ...Lexer.TokenTypes import Delimiter
from ...Lexer.Lexer import Lexer, EOI, TextPosition, Token, Operator, Word, Symbol
from ...Lexer import Lexer as LexerAPI

class Parser():
    def __init__(self, text: str) -> None:
        self.text = text
        self.i = 0

    def next(self) -> Token:
        if self.i >= len(self.tokens):
            return EOI(None, TextPosition(-1,-1))
        self.i += 1
        return self.tokens[self.i - 1]

    def peak(self, peakLength: int) -> Token:
        if self.i + peakLength -1 >= len(self.tokens):
            return EOI(None, TextPosition(-1,-1))
        return self.tokens[self.i + peakLength -1]

    def skip(self, skipLength: int) -> None:
        self.i += skipLength

    def generateAST(self, debug=False) -> IntegerExpressionTree:
        self.tokens: list[Token] = Lexer(self.text).lex(debug=debug)
        self.i = 0
        return self.matchIntegerExpression()

    def matchIntegerExpression(self) -> IntegerExpressionTree:
        return IntegerExpressionTree(self.matchBinaryStatement())

    def matchBinaryStatement(self, priorityLevel=0) -> SyntaxTreeNode:
        leftStmt = {
            0: lambda: self.matchBinaryStatement(1),
            1: lambda: self.matchBinaryStatement(2),
            2: lambda: self.matchBinaryStatement(3),
            3: self.matchUnaryStatment
        }[priorityLevel]()
        backupI = self.i
        try:
            binaryOp = self.matchBinaryOp(priorityLevel)
            rightStmt = self.matchBinaryStatement(priorityLevel)
            return BinaryOperator(leftStmt, rightStmt, binaryOp.value)
        except MatchFailed:
            self.i = backupI
            return leftStmt

    def matchBinaryOp(self, priorityLevel: int) -> Token:
        token = self.peak(1)
        if isinstance(token, Operator) and\
        token.value in {
            0: ['and', 'or', '&&', '||'],
            1: ['!=', '=='],
            2: ['+', '-'],
            3: ['*', '/', '//', '%']
        }[priorityLevel]:
            return self.next()
        raise MatchFailed(token.pos)

    def matchUnaryStatment(self) -> SyntaxTreeNode:
        backupI = self.i
        try:
            unaryOp = self.matchUnaryOp()
            unaryStatement = self.matchUnaryStatment()
            return UnaryOperator(unaryStatement, unaryOp.value)
        except MatchFailed:
            self.i = backupI

        try:
            return self.matchCoreStatment()
        except MatchFailed as error:
            self.i = backupI
            raise error
        
    def matchUnaryOp(self) -> Token:
        token = self.peak(1)
        if isinstance(token, Operator) and token.value in ['not', '!', '-']:
            return self.next()
        raise MatchFailed(token.pos)

    def matchCoreStatment(self) -> SyntaxTreeNode:

        bakcupI = self.i
        try:
            name = self.matchToken(Word).value
            self.matchToken(Symbol, '(')
            args = []
            bakcupI2 = self.i
            while True:
                try:
                    args.append(self.matchIntegerExpression())
                    bakcupI2 = self.i
                    self.matchToken(Delimiter)
                except MatchFailed:
                    self.i = bakcupI2
                    break
            self.matchToken(Symbol, ')')
            return FunctionCall(name, args)
        except MatchFailed:
            self.i = bakcupI
    
        token = self.peak(1)
        if isinstance(token, LexerAPI.Integer):
            return Integer(int(self.next().value))
        

        bakcupI = self.i
        if isinstance(token, Word):
            name = self.next().value

            bakcupI2 = self.i
            try:
                self.matchToken(Operator, '.')
                name2 = self.matchToken(Word).value
                return Variable(name, secondName=name2)
                
            except MatchFailed:
                self.i = bakcupI2


            indices = []
            while True:
                bakcupI2 = self.i
                try:
                    self.matchToken(Symbol, '[')
                    expr = self.matchIntegerExpression().rootNode
                    self.matchToken(Symbol, ']')
                    indices.append(expr)
                except MatchFailed:
                    self.i = bakcupI2
                    break

            if len(indices) != 0:
                return IndexedVariable(name, indices)

            return Variable(name)

        bakcupI = self.i
        try:
            self.matchToken(Symbol, '(')
            integerExpression = self.matchIntegerExpression().rootNode
            self.matchToken(Symbol, ')')
            return integerExpression
        except MatchFailed:
            self.i = bakcupI
        
        raise MatchFailed(self.peak(1).pos)


    def matchToken(self, tokenType, value=None):
        token: Token = self.peak(1)
        if not(type(token) == tokenType and (token.value == value or value is None)):
            raise MatchFailed(token.pos)
        self.skip(1)
        return token
            



'''
python -m ru_final_project.PythonParser.Parser.IntegerExpressionParser.Parser
'''      


if __name__ == "__main__":
    txt = '1*2+3*---jon.y and 3*x==4'
    txt = 'not jon(bob(), 3*4+2)*2 '
    ast1 = Parser(txt).generateAST()
    print(ast1)



            
