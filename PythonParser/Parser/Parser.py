
from .ASTType import *
from . import ASTType
from ..Lexer.Lexer import Lexer
from ..Lexer.TokenTypes import Word, EOI, EOL, Integer, Symbol, Operator, Delimiter, TextPosition
from .RegexParser.Parser import Parser as RegexParser

#C:/Users/gudmu/AppData/Local/Programs/Python/Python310/python.exe -m NewFolderStruct.Parser.Parser
#\(([^=|,]*?),([^=|,]*?),([^=|,]*?)\)
#[$1,$2,$3]
# Joining the classes, allowing LanguageParser to use 
# RegexParser.matchRegex() and RegexParser.matchIntegerExpression() (RegexParser as self)
class Parser(RegexParser):
    #The general rule of the match functions is that if the match failed 
    #the self.i needs to be unchanged when the function exits

    def __init__(self, text: str) -> None:
        self.text = text

    def generateAST(self, debug=False) -> AST:
        self.tokens: list[Token] = Lexer(self.text).lex(debug=debug)
        self.i = 0

        program: ASTType.Program = self.matchProgram()

        return AST(program)

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

        
    def matchProgram(self) -> ASTType.Program:
        backupI = self.i
        try:
            players: Players = self.matchPlayers()
            pieces: Pieces = self.matchPieces()
            rules: Rules = self.matchRules()
            board: Board = self.matchBoard()
            self.matchToken(EOI)
        except MatchFailed as error:
            self.i = backupI
            raise error

        return ASTType.Program(players, pieces, rules, board)


    def matchPlayers(self) -> ASTType.Players:
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
        players: Players = Players(names)
        return players


    def matchPieces(self) -> ASTType.Pieces:
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
            return ASTType.Pieces(pieces)

        except MatchFailed as error:
            self.i = backupI
            raise error


    def matchPiece(self) -> ASTType.Piece:
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
        return ASTType.Piece(pieceName, playerName)


    def matchRules(self) -> ASTType.Rules:
        rules: list[ASTType.Rule] = []
        rule = self.matchRule()
        rules.append(rule)
        while True:
            try:
                token = self.peak(1)
                if not isinstance(token, Word):
                    raise MatchFailed(token.pos)
                keyword = token.value
                if keyword not in ('rule', 'macro', 'predicate', 'effect', 'variable', 'victory'):
                    raise MatchFailed(token.pos)
            except MatchFailed:
                break

            rule = self.matchRule()
            rules.append(rule)

        return ASTType.Rules(rules)

    def matchRule(self) -> ASTType.Rule:
        backupI = self.i

        try:
            token = self.peak(1)
            if not isinstance(token, Word):
                raise MatchFailed(token.pos)
            keyword = token.value
            keywordToMatchFunc = {
                'rule': self.matchPieceRule,
                'macro': self.matchMacro,
                'predicate': self.matchPredicate,
                'effect': self.matchEffect,
                'variable': self.matchVariable,
                'victory': self.matchVictory
            }
            if keyword not in keywordToMatchFunc:
                raise MatchFailed(token.pos)
            self.skip(1)
            rule = keywordToMatchFunc[keyword]()
            return rule
        except MatchFailed as error:
            self.i = backupI
            raise error

    def matchVictory(self) -> ASTType.Victory:
        backupI = self.i
        try:
            name = self.matchToken(Word).value
            self.matchToken(Symbol, "{")
            self.matchToken(Word, "return")
            exp = self.matchIntegerExpression()
            self.matchToken(EOL)
            self.matchToken(Symbol, "}")
            self.matchToken(EOL)
            return ASTType.Victory(name, exp)
        except MatchFailed as error:
            self.i = backupI
            raise error

    def matchVariable(self) -> ASTType.VariableDeclaration:
        backupI = self.i
        try:
            name = self.matchToken(Word).value
            self.matchToken(Operator, "=")
            value = self.matchIntegerExpression()
            self.matchToken(EOL)
            return ASTType.VariableDeclaration(name, value)
        except MatchFailed as error:
            self.i = backupI
            raise error
        

    def matchPredicate(self) -> ASTType.Predicate:
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
            self.matchToken(Word, "return")
            exp = self.matchIntegerExpression()
            self.matchToken(EOL)
            self.matchToken(Symbol, "}")
            self.matchToken(EOL)
            return ASTType.Predicate(name, arguments, exp)
        except MatchFailed as error:
            self.i = backupI
            raise error

    def matchEffect(self) -> ASTType.Effect:
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
            assignments: list[ASTType.Assignment] = []
            while True:
                try:
                    assignment: ASTType.Assignment = self.matchAssignment()
                    assignments.append(assignment)
                except MatchFailed:
                    break
            self.matchToken(Symbol, "}")
            self.matchToken(EOL)
            return ASTType.Effect(name, arguments, assignments)
        except MatchFailed as error:
            self.i = backupI
            raise error

    def matchAssignment(self) -> ASTType.Assignment:
        backupI = self.i
        try:
            name = self.matchToken(Word).value

            indexList: list[IntegerExpression] = []
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

            return ASTType.Assignment(name, indexList, value)
        except MatchFailed as error:
            self.i = backupI
            raise error

    def matchMacro(self) -> ASTType.Macro:
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
            return ASTType.Macro(name, arguments, regex)
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

    def matchPieceRule(self) -> ASTType.PieceRule:
        backupI = self.i
        try:
            name = self.matchToken(Word).value
            self.matchToken(Operator, "=")
            regex = RegexTree(self.matchRegex())
            self.matchToken(EOL)
            return ASTType.PieceRule(name, regex)
        except MatchFailed as error:
            self.i = backupI
            raise error


    def matchBoard(self) -> ASTType.Board:
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
        return ASTType.Board(x, y, boardArr)


    def matchToken(self, tokenType, value=None):
        token: Token = self.peak(1)
        if not(type(token) == tokenType and (token.value == value or value is None)):
            raise MatchFailed(token.pos)
        self.skip(1)
        return token


if __name__ == "__main__":
    f = open("NewFolderStruct/TestFiles/demo1.game")
    text = ''.join(f.readlines())
    print(text)
    ast: AST = Parser(text).generateAST(debug=True)
    print(ast)

