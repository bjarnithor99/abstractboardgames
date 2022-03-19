from .ASTType import Concatination, FunctionCall, Letter, Union, Star, Plus, QuestionMark, IntegerExpression
from ...Lexer.TokenTypes import *
from .ASTType import RegexTree
from ...Lexer.Lexer import Lexer


class MatchFailed(Exception):
    pass

class Parser:
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
        return regex

    def matchRegex(self):
        return RegexTree(self.matchBinaryStmt())

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
            self.matchToken(Symbol, '(')
            dx = self.matchIntegerExpression()
            self.matchToken(Delimiter)
            dy = self.matchIntegerExpression()
            self.matchToken(Delimiter)


            wordOrFunCall = None
            try:
                funCall = self.matchFunctionCall()
                wordOrFunCall = funCall
            except MatchFailed:
                try:
                    predicate = self.matchToken(Word).value
                    wordOrFunCall = predicate
                except MatchFailed:
                    pass
            if wordOrFunCall is None:
                raise MatchFailed
            self.matchToken(Symbol, ')')

            try:
                self.matchToken(Symbol, '{')
            except MatchFailed:
                return Letter(dx, dy, wordOrFunCall)

            effect = self.matchFunctionCall()
            self.matchToken(Symbol, '}')
            return Letter(dx, dy, wordOrFunCall, effect)



        except MatchFailed as error:
            self.i = backupI
            raise error

    def matchIntegerExpression(self):
        numberOfOpenScopes = 0
        def validExpToken(token: Token):
            nonlocal numberOfOpenScopes
            if token.value in ('(', ')'):
                numberOfOpenScopes += {'(':1, ')':-1}[token.value]
            return (isinstance(token, Word) or\
            isinstance(token, Integer) or\
            token.value in '+-*/().' or\
            token.value == '==') and\
            numberOfOpenScopes != -1
        i = 1
        token = self.peak(i)
        expression = []
        if validExpToken(token):
            expression.append(token)
            i += 1
            token = self.peak(i)
        else:
            raise MatchFailed(token.pos)

        while validExpToken(token):
            expression.append(token)
            i += 1
            token = self.peak(i)
        if numberOfOpenScopes > 0:
            raise MatchFailed(token.pos)
        self.skip(i-1)
        return IntegerExpression(expression)
        


if __name__ == "__main__":
    #text = '(0,0,b(1,2,23)){promote(1,2,3)}(1,1+x*x,empty)?(3,4+x*x,empty)+|(6,8,jon)|(x,y,bob)'
    text = '(0,0,a){promote(1,2,3)}'
    ast = Parser(text).parse()
    print(ast)