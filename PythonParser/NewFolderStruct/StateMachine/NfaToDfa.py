from .Types import *
from ..Parser.ASTType import Letter


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