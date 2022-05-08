from .ASTType import Concatination, FunctionCall, Letter, Union, Star, Plus, QuestionMark
from ...Lexer.TokenTypes import *
from .ASTType import RegexTree
from ...Lexer.Lexer import Lexer
from ..IntegerExpressionParser.Parser import Parser as IntegerExpressionParser
from ..ASTType import MatchFailed
class Parser(IntegerExpressionParser):
    def __init__(self, regex: str | list[Token]) -> None:
        self.tokens: list[Token] = []
        if type(regex) == str:
            self.tokens = Lexer(regex).lex()
        else:
            self.tokens = regex
        self.i = 0

    def peak(self, peakLength: int) -> Token:
        if self.i + peakLength -1 >= len(self.tokens):
            return EOI(None, TextPosition(-1,-1))
        return self.tokens[self.i + peakLength -1]

    def skip(self, skipLength: int) -> None:
        self.i += skipLength

    def matchToken(self, tokenType, value=None):
        token: Token = self.peak(1)
        if not(type(token) == tokenType and (token.value == value or value is None)):
            raise MatchFailed()
        self.skip(1)
        return token

    def parse(self) -> RegexTree:
        regex = self.matchRegex()
        #self.matchToken(EOI)
        return RegexTree(regex)

    def matchRegex(self):
        return self.matchBinaryStmt()

    def matchBinaryStmt(self):

        concat = self.matchConcat()
        backupI = self.i
        try:
            self.matchToken(Operator, '|')
            concat2 = self.matchBinaryStmt()
            return Union(concat, concat2)
        except MatchFailed:
            self.i = backupI
            return concat


    def matchConcat(self):
        unary = self.matchUnaryStmt()
        concatList = [unary]
        while True:
            try:
                otherUnary = self.matchUnaryStmt()
                concatList.append(otherUnary)
            except MatchFailed:
                break

        returnNode = concatList.pop()
        while len(concatList) > 0:
            returnNode = Concatination(concatList.pop(), returnNode)
        return returnNode

    def matchUnaryStmt(self):
        coreStmt = self.matchCoreStmt()
        token = self.peak(1)
        if isinstance(token, Operator) and token.value in ('*', '?', '+'):
            unarySymbol = self.matchToken(Operator).value
            unarySymbolToConstructor = {
                    '*': Star,
                    '?': QuestionMark,
                    '+': Plus
                }
            return unarySymbolToConstructor[unarySymbol](coreStmt)
        else:
            return coreStmt


    def matchCoreStmt(self):
        try:
            return self.matchFunctionCall()
        except MatchFailed:
            pass

        try:
            backupI = self.i
            self.matchToken(Symbol, '(')
            regex = self.matchRegex()
            self.matchToken(Symbol, ')')
            return regex
        except MatchFailed:
            self.i = backupI

        try:
            return self.matchLetter()
        except MatchFailed:
            pass

        raise MatchFailed(self.peak(1).pos)

    def matchFunctionCall(self):
        backupI = self.i
        try:
            name = self.matchToken(Word).value
            self.matchToken(Symbol, '(')
            arguments = []
            try:
                arguments = self.matchArguments()
            except MatchFailed:
                pass
            self.matchToken(Symbol, ')')
            return FunctionCall(name, arguments)

        except MatchFailed:
            self.i = backupI
            raise MatchFailed(self.peak(1).pos)

    def matchArguments(self):
        backupI = self.i
        try:
            arg1 = self.matchIntegerExpression()
            argList = [arg1]
            while True:
                try:
                    #self.matchToken(Symbol, ',')
                    self.matchToken(Delimiter)
                except MatchFailed:
                    break

                arg = self.matchIntegerExpression()
                argList.append(arg)
        except MatchFailed as error:
            self.i = backupI
            raise error
        return argList

    def matchLetter(self):
        backupI = self.i
        try:
            self.matchToken(Symbol, '[')
            dx = self.matchIntegerExpression()
            self.matchToken(Delimiter)
            dy = self.matchIntegerExpression()
            self.matchToken(Delimiter)
            expr = self.matchIntegerExpression()
            self.matchToken(Symbol, ']')

            try:
                self.matchToken(Symbol, '{')
            except MatchFailed:
                return Letter(dx, dy, expr)

            effect = self.matchFunctionCall()
            self.matchToken(Symbol, '}')
            return Letter(dx, dy, expr, effect)



        except MatchFailed as error:
            self.i = backupI
            raise error
        

#C:/Users/gudmu/AppData/Local/Programs/Python/Python310/python.exe -m NewFolderStruct.Parser.RegexParser.Parser
if __name__ == "__main__":
    text = '(0,0,b(1,2,23)){promote(1,2,3)}(1,1+x*x,empty)?(3,4+x*x,empty)+|(6,8,jon)|(x,y,bob)'
    #text = '''RookMacro() | BishipMacro();'''
    ast = Parser(text).parse()
    print(ast)