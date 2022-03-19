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
        while self.i < len(self.text) and self.text[self.i] in '\t\n ':
            self.character += 1
            if self.text[self.i] == '\n':
                self.character = 0
                self.line += 1
            self.i += 1
            self.hitBreak = True

        if self.i < len(self.text):
            self.i += 1
            self.character += 1
            return self.text[self.i-1]
        else:
            return None


    def lex(self, debug=False) -> list[Token]:
        tokens: list[Token] = []

        char = self.next()
        while char is not None:
            pos = TextPosition(self.line, self.character)

            #match int
            if char in alpha:
                intStr = char
                char = self.next()
                while char is not None and char in alpha and not self.hitBreak:
                    intStr += char
                    char = self.next()
                tokens.append(Integer(int(intStr), pos))


            #match word
            elif char in beta:
                wordStr = char
                char = self.next()
                while char is not None and (char in beta or char in alpha) and not self.hitBreak:
                    wordStr += char
                    char = self.next()
                tokens.append(Word(wordStr, pos))

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

            #match op
            elif char in Operator.values:
                if char == '=':
                    char = self.next()
                    if not self.hitBreak and char == '=':
                        tokens.append(Operator('==', pos))
                        char = self.next()
                    else:
                        tokens.append(Operator('=', pos))
                else:
                    tokens.append(Operator(char, pos))
                    char = self.next()

            else:
                raise LexingFailed(f'{pos}, Problem: {char}')

        if debug:
            print(self.text)
            for token in tokens:
                print('\t', token.pos, type(token), token.value)

        return tokens