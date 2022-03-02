from sys import set_asyncgen_hooks
from .Parser import Letter, Parser, lex
from .StateMachineGenerator2 import Generator, NFAStateMachine, State
from copy import deepcopy


class DFAStateMachine(NFAStateMachine):
    def __str__(self) -> str:
        returnStr = '\n'
        toVisit = [self.start]; finals = []; visited = []
        returnStr += 'Start '
        while len(toVisit) > 0:
            i = toVisit.pop()
            if id(i) in visited:
                continue
            visited.append(id(i))
            if i.final:
                finals.append(i)
            returnStr += str(id(i)) + ' has children:\n'
            for transition in i.outTransitions:
                toVisit.append(transition.end)
                returnStr += f'\t self -> {transition.letter} -> {id(transition.end)}\n'
        returnStr += f'The final states are: {[id(state) for state in finals]}\n'
        for i, key in enumerate(visited):
            returnStr = returnStr.replace(str(key), f'state:{str(i+1)}')
        return returnStr

class NfaToDfaConverter:
    def __init__(self, nfa: NFAStateMachine) -> None:
        self.nfa = nfa
        self.newStates: dict[int, State] = dict()
        self._setStateIDs()

    def createDFA(self) -> DFAStateMachine:
        sm = DFAStateMachine()
        sm.start = self.createAndConnectStateGroup([self.nfa.start])
        return sm

    def createAndConnectStateGroup(self, stateGroup: list[State]) -> State:
        epsilonGroup = self.getEpsilonGroup(stateGroup)
        epsilonGroupID = self.getGroupID(epsilonGroup)
        if epsilonGroupID in self.newStates:
            return self.newStates[epsilonGroupID]
        newState = State()
        self.newStates[epsilonGroupID] = newState
        if any([state is self.nfa.final for state in epsilonGroup]):
            newState.final = True

        for letter in self.getLettersOfGroup(epsilonGroup):
            letterGroup = self.getLetterGroupOfGroup(epsilonGroup, letter)
            letterState = self.createAndConnectStateGroup(letterGroup)
            newState.addTransition(letterState, letter)

        return newState



    def getEpsilonGroup(self, stateGroup: list[State]) -> list[State]:
        epsilonGroup = [*stateGroup]
        visited = []
        toVisit = [*stateGroup]
        while len(toVisit) > 0:
            i = toVisit.pop()
            if id(i) in visited:
                continue
            visited.append(id(i))

            epsilonGroup.append(i)

            for nextState in [transition.end for transition in i.outTransitions if transition.letter is None]:
                toVisit.append(nextState)
        return epsilonGroup

    def getGroupID(self, stateGroup: list[State]) -> int:
        ID = 0
        for state in stateGroup:
            ID += (2**state.ID)
        return ID


    def getLettersOfGroup(self, stateGroup: list[State]) -> list[Letter]:
        letters = []
        for i in stateGroup:
            for transition in i.outTransitions:
                if transition.letter not in letters and transition.letter is not None:
                    letters.append(transition.letter)
        return letters

    def getLetterGroupOfGroup(self, stateGroup: list[State], letter: Letter) -> list[State]:
        temp = []
        letterGroup = []
        for i in stateGroup:
            for nextState in [transition.end for transition in i.outTransitions if transition.letter == letter]:
                if id(nextState) not in temp:
                    letterGroup.append(nextState)
                    temp.append(id(nextState))
        return letterGroup

    def _setStateIDs(self):
        start = self.nfa.start

        visited = []
        toVisit = [start]
        stateID = 0
        while len(toVisit) > 0:
            i = toVisit.pop()
            if id(i) in visited:
                continue
            visited.append(id(i))

            i.ID = stateID
            stateID += 1

            for nextState in [transition.end for transition in i.outTransitions]:
                toVisit.append(nextState)



if __name__ == "__main__":
    tokens = lex('((2,1,empty)*(1,2,empty))+')
    abs = Parser(tokens).parse()
    nfa = Generator(abs).createStatMachine()
    print(nfa)
    dfa = NfaToDfaConverter(nfa).createDFA()
    print(dfa)