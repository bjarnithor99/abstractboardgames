
#from .ASTType import *
#from . import ASTType
from . import ASTType2
from .ASTType2 import *
from ..Lexer.Lexer import Lexer
from ..Lexer.TokenTypes import Word, EOI, EOL, Integer, Symbol, Operator, Delimiter, TextPosition, Token
from .RegexParser.Parser import Parser as RegexParser
from .ParserType import MatchFailed, AbstractParser

#\(([^=|,]*?),([^=|,]*?),([^=|,]*?)\)
#[$1,$2,$3]

# Joining the classes, allowing LanguageParser to use 
# RegexParser.matchRegex() and RegexParser.matchIntegerExpression() (RegexParser as self)
class ProgramParser(RegexParser):
    #The general rule of the match functions is that if the match failed 
    #the self.i needs to be unchanged when the function exits

    def generateAST(self, debug=False) -> ASTType2.ProgramAST:
        self.tokens: list[Token] = Lexer(self.text).lex(debug=debug)
        self.i = 0
        program: Program = self.matchProgram()
        return ASTType2.ProgramAST(program)

        
    def matchProgram(self) -> ASTType2.ProgramAST:
        backupI = self.i
        try:
            players: list[Player] = self.matchPlayers()
            pieces: list[Piece] = self.matchPieces()
            definitions: list[Definition] = self.matchStar(self.matchDefinition)
            turns: list[Turn] = self.matchStar(self.matchTurn)
            victories: list[Victory] = self.matchStar(self.matchVictory)
            board: Board = self.matchBoard()
            self.matchToken(EOI)
        except MatchFailed as error:
            self.i = backupI
            raise error

        return Program(players, pieces, definitions, turns, victories, board)


    def matchPlayers(self) -> list[Player]:
        names: list[str] = []

        token = self.peak(1)
        if not(type(token) == Word and token.value == "players"):
            raise MatchFailed(token.pos)

        token = self.peak(2)
        if not(type(token) == Operator and token.value == "="):
            raise MatchFailed(token.pos)

        token = self.peak(3)
        if not(type(token) == Word):
            raise MatchFailed(token.pos)

        names.append(token.value)
        i = 4
        token = self.peak(i)
        while type(token) == Delimiter:
            token = self.peak(i+1)
            if not(type(token) == Word):
                raise MatchFailed(token.pos)
            names.append(token.value)
            i += 2
            token = self.peak(i)
        
        if not(type(token) == EOL):
            raise MatchFailed(token.pos)
        
        #Success
        self.skip(i)
        players: list[Player] = [Player(name) for name in names]
        return players


    def matchPieces(self) -> list[Piece]:
        backupI = self.i

        try:
            token = self.next()
            if not(type(token) == Word and token.value == "pieces"):
                raise MatchFailed(token.pos)

            token = self.next()
            if not(type(token) == Operator and token.value == "="):
                raise MatchFailed(token.pos)

            pieces: list[Piece] = []
            piece: Piece = self.matchPiece()
            pieces.append(piece)

            token = self.next()
            while type(token) == Delimiter:
                piece: Piece = self.matchPiece()
                pieces.append(piece)
                token = self.next()

            if not(type(token) == EOL):
                raise MatchFailed(token.pos)
            return pieces

        except MatchFailed as error:
            self.i = backupI
            raise error


    def matchPiece(self) -> Piece:
        token = self.peak(1)
        if not(type(token) == Word):
            raise MatchFailed(token.pos)

        pieceName = token.value

        token = self.peak(2)
        if not(type(token) == Symbol and token.value == "("):
            raise MatchFailed(token.pos)

        token = self.peak(3)
        if not(type(token) == Word):
            raise MatchFailed(token.pos)

        playerName = token.value

        token = self.peak(4)
        if not(type(token) == Symbol and token.value == ")"):
            raise MatchFailed(token.pos)

        #Success!
        self.skip(4)
        return Piece(pieceName, playerName)

    def matchDefinition(self) -> Definition:
        backupI = self.i
        try:
            keyword = self.matchToken(Word).value
            keywordToMatchFunc = {
                'regex': self.matchRegexDefinition,
                'function': self.matchFunctionDefinition,
                'effect': self.matchEffectDefinition,
                'variable': self.matchVariableDefinition,
            }
            if keyword not in keywordToMatchFunc:
                raise MatchFailed(self.peak(0).pos)
            rule = keywordToMatchFunc[keyword]()
            return rule
        except MatchFailed as error:
            self.i = backupI
            raise error

    def matchVariableDefinition(self) -> VariableDefinition:
        backupI = self.i
        try:
            name = self.matchToken(Word).value
            self.matchToken(Operator, "=")
            value = self.matchIntegerExpression()
            self.matchToken(EOL)
            return VariableDefinition(name, value)
        except MatchFailed as error:
            self.i = backupI
            raise error
        

    def matchFunctionDefinition(self) -> Function:
        backupI = self.i
        try:
            name = self.matchToken(Word).value
            self.matchToken(Symbol, "(")
            arguments = []
            try:
                arguments = self.matchCommaSperatedWords()
            except MatchFailed:
                pass
            self.matchToken(Symbol, ")")


            expOrRegex = None
            backupI2 = self.i
            try:
                self.matchToken(Symbol, "{")
                self.matchToken(Word, "return")
                exp = self.matchIntegerExpression()
                self.matchToken(EOL)
                self.matchToken(Symbol, "}")

                expOrRegex = exp
            except MatchFailed:
                self.i = backupI2

            backupI2 = self.i
            try:
                self.matchToken(Operator, "=")
                regex = RegexTree(self.matchRegex())

                expOrRegex = regex
            except MatchFailed:
                self.i = backupI2

            if expOrRegex is None:
                raise MatchFailed(self.peak(1).pos)

            self.matchToken(EOL)
            return Function(name, arguments, expOrRegex)
        except MatchFailed as error:
            self.i = backupI
            raise error

    def matchEffectDefinition(self) -> EffectDefinition:
        backupI = self.i
        try:
            name = self.matchToken(Word).value
            self.matchToken(Symbol, "(")
            arguments = []
            try:
                arguments = self.matchCommaSperatedWords()
            except MatchFailed:
                pass
            self.matchToken(Symbol, ")")
            self.matchToken(Symbol, "{")
            assignments: list[Assignment] = []
            while True:
                try:
                    assignment: Assignment = self.matchAssignment()
                    assignments.append(assignment)
                except MatchFailed:
                    break
            self.matchToken(Symbol, "}")
            self.matchToken(EOL)
            return EffectDefinition(name, arguments, assignments)
        except MatchFailed as error:
            self.i = backupI
            raise error

    def matchAssignment(self) -> Assignment:
        backupI = self.i
        try:
            name = self.matchToken(Word).value

            indexList: list[IntegerExpressionTree] = []
            while True:
                try:
                    self.matchToken(Symbol, '[')
                except MatchFailed:
                    break
                expr = self.matchIntegerExpression()
                indexList.append(expr)
                self.matchToken(Symbol, ']')

            self.matchToken(Operator, '=')
            value = self.matchIntegerExpression()
            self.matchToken(EOL)

            return Assignment(name, indexList, value)
        except MatchFailed as error:
            self.i = backupI
            raise error

    def matchRegexDefinition(self) -> RegexDefinition:
        backupI = self.i
        try:
            name = self.matchToken(Word).value
            self.matchToken(Symbol, "(")
            arguments = []
            try:
                arguments = self.matchCommaSperatedWords()
            except MatchFailed:
                pass
            self.matchToken(Symbol, ")")
            self.matchToken(Operator, "=")
            regex = RegexTree(self.matchRegex())
            self.matchToken(EOL)
            return RegexDefinition(name, arguments, regex)
        except MatchFailed as error:
            self.i = backupI
            raise error

    def matchCommaSperatedWords(self) -> list[str]:
        backupI = self.i
        wordArr = []
        try:
            piece = self.matchToken(Word).value
            wordArr.append(piece)
            while True:
                try:
                    self.matchToken(Delimiter)
                except MatchFailed:
                    break
                piece = self.matchToken(Word).value
                wordArr.append(piece)
        except MatchFailed:
            self.i = backupI
            raise MatchFailed
        return wordArr

    def matchTurn(self) -> Turn:
        backupI = self.i
        try:
            self.matchToken(Word, 'turn')
            name = self.matchToken(Word).value
            self.matchToken(Operator, '=')
            regex =  RegexTree(self.matchRegex())
            self.matchToken(EOL)
            return Turn(name, regex)
        except MatchFailed as error:
            self.i = backupI
            raise error

    def matchVictory(self) -> Victory:
        backupI = self.i
        try:
            self.matchToken(Word, 'victory')
            name = self.matchToken(Word).value
            self.matchToken(Symbol, "{")
            self.matchToken(Word, "return")
            exp = self.matchIntegerExpression()
            self.matchToken(EOL)
            self.matchToken(Symbol, "}")
            self.matchToken(EOL)
            return Victory(name, exp)
        except MatchFailed as error:
            self.i = backupI
            raise error


    def matchBoard(self) -> Board:
        backupI = self.i
        try:
            boardArr: list[str] = []
            #Board size
            self.matchToken(Word, "board_size")
            self.matchToken(Operator, "=")
            x = self.matchToken(Integer).value
            self.matchToken(Delimiter)
            y = self.matchToken(Integer).value
            self.matchToken(EOL)

            #Board
            self.matchToken(Word, "board")
            self.matchToken(Operator, "=")
            boardArr = self.matchCommaSperatedWords()
            self.matchToken(EOL)

        except MatchFailed as error:
            self.i = backupI
            raise error
        #print(x, y, boardArr)
        return Board(x, y, boardArr)

'''
python -m ru_final_project.PythonParser.Parser.Parser2
'''

if __name__ == "__main__":
    f = open("ru_final_project/PythonParser/TestFiles/chess.game")
    text = ''.join(f.readlines())
    print(text)
    ast: ProgramAST = ProgramParser(text).generateAST(debug=True)
    print(ast)

