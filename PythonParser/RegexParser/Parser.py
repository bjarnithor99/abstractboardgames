from __future__ import annotations
from os import execv



class Token:
    def __init__(self, value) -> None:
        self.value = value

class Symbol(Token):
    validArr =  ['(', ')', ',']

class Operator(Token):
    validArr = ['*', '|', '?', '+']

class Word(Token):
    pass

class Integer(Token):
    pass

class End(Token):
    pass


def lex(languageStr: str) -> list[Token]:
    languageStr = languageStr.replace(' ', '')
    tokens = []

    languageStr += 'END'
    i = 0
    char = languageStr[i]
    while languageStr[i:i+3] != 'END':
        if char in '-1234567890':
            integerStr = ''
            while char in '-1234567890':
                integerStr += char
                i += 1
                char = languageStr[i]
            tokens.append(Integer(int(integerStr)))

        elif char in Symbol.validArr:
            tokens.append(Symbol(char))
            i += 1
            char = languageStr[i]

        elif char in Operator.validArr:
            tokens.append(Operator(char))
            i += 1
            char = languageStr[i]

        elif char.lower() in 'qwertyuiopasdfghjklzxcvbnm':
            wordStr = ''
            while char in 'qwertyuiopasdfghjklzxcvbnm':
                wordStr += char
                i += 1
                char = languageStr[i]
            tokens.append(Word(wordStr))
        else:
            raise ValueError(f"Invalid character {char} place {i}")
    return tokens + [End(None)]*3

            
class SyntaxTree:
    def __init__(self, rootNode: SyntaxTreeNode) -> None:
        self.rootNode = rootNode

    def __str__(self) -> str:
        return str(self.rootNode)

class SyntaxTreeNode:
    pass

class FunctionCall(SyntaxTreeNode):
    def __init__(self, name: str, arguments: list[SyntaxTreeNode]) -> None:
        super().__init__()
        self.name = name
        self.arguments: list[SyntaxTreeNode] = arguments


class BinarySymbol(SyntaxTreeNode):
    symbols = ['|']
    def __init__(self, child1: SyntaxTreeNode, child2: SyntaxTreeNode) -> None:
        super().__init__()
        self.child1 = child1
        self.child2 = child2

    def __str__(self, indent=0) -> str:
        returnStr = ''
        returnStr += '\t'*(indent) + str(type(self)).split('.')[1].split("'")[0] + '\n'
        returnStr += '\t'*(indent) + self.child1.__str__(indent+1) + '\n'
        returnStr += '\t'*(indent) + self.child2.__str__(indent+1)
        return returnStr

class Union(BinarySymbol):
    pass

class Concatination(BinarySymbol):
    pass

class UnarySymbol(SyntaxTreeNode):
    symbols = ['*', '?', '+']
    def __init__(self, child: SyntaxTreeNode) -> None:
        super().__init__()
        self.child = child

    def __str__(self, indent=0) -> str:
        returnStr = ''
        returnStr += '\t'*(indent) + str(type(self)).split('.')[1].split("'")[0] + '\n'
        returnStr += '\t'*(indent) + self.child.__str__(indent+1)
        return returnStr




class Star(UnarySymbol):
    pass

class QuestionMark(UnarySymbol):
    pass

class Plus(UnarySymbol):
    pass

class Letter(SyntaxTreeNode):
    def __init__(self, dx, dy, pre) -> None:
        super().__init__()
        self.value = (dx, dy, pre)

    def __str__(self, indent=0) -> str:
        returnStr = ''
        returnStr += '\t'*(indent) + f'{str(self.value)}'
        return returnStr


class MatchFailed(Exception):
    pass

class Parser:
    def __init__(self, tokens: list[Token]) -> None:
        self.i = 0
        self.storeArr = []
        self.tokens = tokens

    def store(self):
        self.storeArr.append(self.i)

    def revert(self):
        self.i = self.storeArr.pop()
    
    def parse(self) -> SyntaxTree:
        return SyntaxTree(self.matchProgram())

    def next(self) -> Token:
        value = self.tokens[self.i]
        self.i += 1
        return value

    def peak(self, peakLength: int) -> Token:
        # peakLength 1 for next one
        return self.tokens[self.i + peakLength - 1]


    def matchProgram(self) -> SyntaxTreeNode:
        try:
            return self.matchConcatination()
        except MatchFailed:
            pass

        try:
            return self.matchStatement()
        except MatchFailed:
            pass

        raise MatchFailed   

        

    def matchConcatination(self) -> SyntaxTreeNode:
        i = self.i

        try:
            stmt = self.matchStatement()
        except MatchFailed:
            raise MatchFailed

        try:
            concat = self.matchConcatination()
            return Concatination(stmt, concat)
        except:
            pass


        try:
            stmt2 = self.matchStatement()
            return Concatination(stmt, stmt2)
        except:
            pass

        self.i = i
        raise MatchFailed

        
    def matchStatement(self):
        unaryStmt = self.matchUnaryStatment()

        nextToken = self.peak(1)
        if isinstance(nextToken, Operator):
            if nextToken.value in BinarySymbol.symbols:
                i = self.i
                self.next()
                try:
                    rightStmt = self.matchStatement()

                    binarySymbolToConstructor = {
                        '|': Union
                    }

                    binaryNodeCon = binarySymbolToConstructor[nextToken.value]
                    return binaryNodeCon(unaryStmt, rightStmt)

                except MatchFailed:
                    pass

                self.i = i


        return unaryStmt


    def matchUnaryStatment(self):
        coreStmt = self.matchCoreStatement()

        nextToken = self.peak(1)
        if nextToken.value in UnarySymbol.symbols:
                self.next()

                unarySymbolToConstructor = {
                    '*': Star,
                    '?': QuestionMark,
                    '+': Plus
                }

                unaryNodeCon = unarySymbolToConstructor[nextToken.value]
                return unaryNodeCon(coreStmt)

        return coreStmt


    def matchCoreStatement(self):
        try:
            return self.matchFunctionCall()
        except MatchFailed:
                pass

        i = self.i
        token = self.next()
        if isinstance(token, Symbol) and token.value == '(':
            try:
                program = self.matchProgram()
                token = self.next()
                if isinstance(token, Symbol) and token.value == ')':
                    return program
            except MatchFailed:
                pass
            
        self.i = i

        return self.matchLetter()


    def matchFunctionCall(self):
        i = self.i
        nexToken = self.next()
        if not isinstance(nexToken, Word):
            self.i = i
            raise MatchFailed
        functionName = nexToken.value
        nexToken = self.next()
        if not (isinstance(nexToken, Symbol) and nexToken.value == '('):
            self.i = i
            raise MatchFailed

        try:
            funArguments = self.matchFunctionArguments()
            nexToken = self.next()
            if not (isinstance(nexToken, Symbol) and nexToken.value == ')'):
                self.i = i
                raise MatchFailed
            return FunctionCall(functionName, funArguments)
        except MatchFailed:
            pass

        self.i = i
        raise MatchFailed
        


    def matchFunctionArguments(self) -> list[SyntaxTreeNode]:
        raise MatchFailed #todo

    def matchVariable(self):
        raise MatchFailed #todo

    def matchLetter(self) -> Letter:
        correctSymbols = ((1,'('), (3,','), (5,','), (7,')'))
        for i, symbolValue in correctSymbols:
            token = self.peak(i)
            if not (isinstance(token, Symbol) and token.value == symbolValue):
                raise MatchFailed

        correctTokens = ((2, Integer), (4, Integer), (6, Word))
        for i, tokenType in correctTokens:
            token = self.peak(i)
            if not (type(token) == tokenType):
                raise MatchFailed

        # Success!
        letter = Letter(self.peak(2).value, self.peak(4).value, self.peak(6).value)
        for i in range(7):
            self.next()

        return letter
    

    



if __name__ == "__main__":
    #tokens = lex('(9512478963,9999)*|(1,3,empty)*|((((((1,3,empty)*)*)*)*)*)*')#lex('(1,0,empty)*(1,0,enemy)?')
    #['(', 1, ',', 0, ',', 'empty', ')', '*', '(', 1, ',', 0, ',', 'enemy', ')', '?']
    #print(list(map(lambda x: (x.value), tokens)))

    tokens = lex('(0,0,b)* | (1,2,em)')

    ast = Parser(tokens).parse()
    print(ast)
