from .NfaToDfa import NfaToDfaConverter
from .StateMachineGenerator import Generator
from ..Parser.RegexParser.ASTType import RegexTree


def RegexToDFA(regex: RegexTree):
    nfa = Generator(regex).createStatMachine()
    print('nfa:\n', nfa)
    dfa = NfaToDfaConverter(nfa).createDFA()
    print('dfa:\n', dfa)

    return dfa