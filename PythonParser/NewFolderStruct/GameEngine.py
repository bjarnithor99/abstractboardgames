from .Parser.Parser import Parser
from .Parser.ASTType import *
from .StateMachine.Types import *

class GameEngine:
    def __init__(self, fileName) -> None:
        f = open(f'NewFolderStruct/TestFiles/{fileName}')
        txt = ''.join(f.readlines())
        self.ast = Parser(txt).generateAST()
        print(self.ast)


        self.variables: dict[str, IntegerExpression]= dict()
        self.pieceRules: dict[str, DFAStateMachine] = dict()



if __name__ == '__main__':
    GameEngine('demo1.game')