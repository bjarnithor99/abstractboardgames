class TextPosition:
    def __init__(self, line: int, character: int) -> None:
        self.line = line
        self.character = character

    def __str__(self) -> str:
        return f'Line:{self.line}, Character:{self.character}'

class Token:
    def __init__(self, value, pos: TextPosition) -> None:
        self.value = value
        self.pos = pos
    
    def __str__(self) -> str:
        return str(type(self)).split('.')[-1].split("'")[0] + f" '{str(self.value)}'"

class Comment(Token):
    pass

class Integer(Token):
    #numbers
    pass

class Word(Token):
    #Strings that don't start with numbers
    pass

class Delimiter(Token):
    # comma ,
    pass

class EOL(Token):
    #End of line ;
    pass

class Symbol(Token):
    values = '()[]{}'

class Operator(Token):
    values = '!|=*+-/%?.'

class EOI(Token):
    pass