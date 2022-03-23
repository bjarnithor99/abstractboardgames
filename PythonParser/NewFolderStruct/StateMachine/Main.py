from .NfaToDfa import NfaToDfaConverter
from .StateMachineGenerator import Generator
from ..Parser.RegexParser.ASTType import RegexTree


def RegexToDFA(regex: RegexTree):
    nfa = Generator(regex).createStatMachine()
    dfa = NfaToDfaConverter(nfa).createDFA()
    return dfa