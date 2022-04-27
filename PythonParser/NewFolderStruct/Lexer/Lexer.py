from .TokenTypes import *
alpha = '1234567890'
beta = 'qwertyuiopasdfghjklzxcvbnm'; beta = beta + beta.upper()
class LexingFailed(Exception):
    pass

class Lexer:
    def __init__(self, text: str) -> None:
        self.text: str = text
        self.i: int = 0
        self.line: int = 1
        self.character: int = 0

    def next(self) -> (str | None):
        self.hitBreak = False
        self.hitNewline = False
        while self.i < len(self.text) and self.text[self.i] in '\t\n ':
            self.character += 1
            if self.text[self.i] == '\n':
                self.character = 0
                self.line += 1
                self.hitNewline = True
            self.i += 1
            self.hitBreak = True

        if self.i < len(self.text):
            self.i += 1
            self.character += 1
            return self.text[self.i-1]
        else:
            return None

    def opIsNext(self) -> bool:
        try:
            temp = self.i, self.line, self.character
            self.matchOp()
            self.i, self.line, self.character = temp
            return True
        except LexingFailed:
            return False


    def matchOp(self) -> str:
        temp = self.i-1, self.line, self.character
        
        for op in Operator.values:
            self.i, self.line, self.character = temp
            matched = True
            for charOp in op:
                char = self.next()
                if char == charOp and not self.hitBreak:
                    continue
                else:
                    matched = False
                    break
            if matched:
                return op
            else:
                continue
        
        self.i, self.line, self.character = temp
        self.i += 1
        raise LexingFailed()
        
        

    def lex(self, debug=False) -> list[Token]:
        tokens: list[Token] = []

        char = self.next()
        while char is not None:
            pos = TextPosition(self.line, self.character)

            #match op
            if self.opIsNext():
                opStr = self.matchOp()
                tokens.append(Operator(opStr, pos))
                char = self.next()

            #match int
            elif char in alpha:
                intStr = char
                char = self.next()
                while char is not None and char in alpha and not self.hitBreak:
                    intStr += char
                    char = self.next()
                tokens.append(Integer(int(intStr), pos))

            #match delim
            elif char == ',':
                tokens.append(Delimiter(char, pos))
                char = self.next()

            #match eol
            elif char == ';':
                tokens.append(EOL(char, pos))
                char = self.next()

            #match symbol
            elif char in Symbol.values:
                tokens.append(Symbol(char, pos))
                char = self.next()
            
            #match comment
            elif char == '#':
                commentStr = ''
                char = self.next()
                while not self.hitNewline and char is not None:
                    commentStr += char
                    char = self.next()
                #tokens.append(Comment(commentStr, pos))
            
            #match word
            elif char in beta:
                wordStr = char
                char = self.next()
                while char is not None and (char in beta or char in alpha or char == '_') and not self.hitBreak:
                    wordStr += char
                    char = self.next()
                tokens.append(Word(wordStr, pos))

            else:
                raise LexingFailed(f'{pos}, Problem: {char}')

        if debug:
            print(self.text)
            for token in tokens:
                print('\t', token.pos, type(token), token.value)

        return tokens

def printTokens(tokens: list[Token]):
    for i in range(len(tokens)):
        print(i, tokens[i])

'''
C:/Users/gudmu/AppData/Local/Programs/Python/Python310/python.exe -m ru_final_project.PythonParser.NewFolderStruct.Lexer.Lexer
'''
if __name__ == "__main__":
    f = open("ru_final_project/PythonParser/NewFolderStruct/TestFiles/chess.game")
    text = ''.join(f.readlines())
    print(text)
    tokens = Lexer(text).lex(debug=True)
    printTokens(tokens)