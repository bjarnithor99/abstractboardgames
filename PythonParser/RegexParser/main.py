from .Parser import lex, Parser
from .StateMachineGenerator2 import Generator, StateMachine
from .NfaToDfa import NfaToDfaConverter


def createSM(description: str) -> StateMachine:
	tokens = lex(description)
	abs = Parser(tokens).parse()
	nfa = Generator(abs).createStatMachine()
	dfa = NfaToDfaConverter(nfa).createDFA()
	return dfa