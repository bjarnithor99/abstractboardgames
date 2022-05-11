from ..Lexer.TokenTypes import EOI, TextPosition, Token

class MatchFailed(Exception):
    pass


class AbstractParser:
    def __init__(self, text: str) -> None:
        self.text = text
        self.tokens: list[Token] = []
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

    def matchToken(self, tokenType, value=None):
        token: Token = self.peak(1)
        if not(type(token) == tokenType and (token.value == value or value is None)):
            typeStr = lambda type: str(type).split('.')[-1][:-2]
            raise MatchFailed(f'{str(token.pos)} Tried to match {typeStr(tokenType) + " " + ("" if value is None else value)} but got {typeStr(type(token)) + " " + str(token.value)}')
        self.skip(1)
        return token

    def matchStar(self, matchFunction):
        matchedItemArr = []
        while True:
            try:
                item = matchFunction()
                matchedItemArr.append(item)
            except MatchFailed:
                break
        return matchedItemArr