from .NfaToDfa import NfaToDfaConverter
from .StateMachineGenerator import Generator
from ..Parser.RegexParser.ASTType import RegexTree


def RegexToDFA(regex: RegexTree, debug=False):
    nfa = Generator(regex).createStatMachine()
    dfa = NfaToDfaConverter(nfa).createDFA()

    if debug:
        debugStr = ''
        debugStr += f'\nNFA created:\n{str(nfa)}\n'
        debugStr += f'\nDFA created:\n{str(dfa)}\n'
        return dfa, debugStr
    else:
        return dfa