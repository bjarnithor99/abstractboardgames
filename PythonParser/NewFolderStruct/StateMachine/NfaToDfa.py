from .Types import *
from ..Parser.ASTType import Letter


class NfaToDfaConverter:
    def __init__(self, nfa: NFAStateMachine) -> None:
        self.nfa = nfa
        self.newStates: dict[int, State] = dict()
        self._setStateIDs()

    def createDFA(self) -> DFAStateMachine:
        sm = DFAStateMachine()
        sm.start = self.createAndConnectStateGroup({self.nfa.start})
        return sm

    def createAndConnectStateGroup(self, stateGroup: set[State]) -> State:
        epsilonGroup = self.getEpsilonGroup(stateGroup)
        epsilonGroupID = self.getGroupID(epsilonGroup)
        if epsilonGroupID in self.newStates:
            return self.newStates[epsilonGroupID]

        newState = State()

        self.newStates[epsilonGroupID] = newState
        if any([state is self.nfa.final for state in epsilonGroup]):
            newState.final = True

        for letter in self.getUniqueLettersOfGroup(epsilonGroup):
            letterGroup = self.getLetterGroupOfGroup(epsilonGroup, letter)
            letterState = self.createAndConnectStateGroup(letterGroup)
            newState.addTransition(letterState, letter)

        return newState



    def getEpsilonGroup(self, stateGroup: set[State]) -> set[State]:
        epsilonGroup = set([*stateGroup])
        visited = []
        toVisit = [*stateGroup]
        while len(toVisit) > 0:
            i = toVisit.pop()
            if id(i) in visited:
                continue
            visited.append(id(i))

            epsilonGroup.add(i)

            for nextState in [transition.end for transition in i.outTransitions if transition.letter is None]:
                toVisit.append(nextState)
        return epsilonGroup

    def getGroupID(self, stateGroup: set[State]) -> int:
        ID = 0
        for state in stateGroup:
            ID += (2**state.ID)
        return ID


    def getUniqueLettersOfGroup(self, stateGroup: set[State]) -> set[Letter]:
        letters = set()
        for i in stateGroup:
            for transition in i.outTransitions:
                if transition.letter not in letters and transition.letter is not None:
                    letters.add(transition.letter)
        return letters

    def getLetterGroupOfGroup(self, stateGroup: set[State], letter: Letter) -> set[State]:
        idsInLetterGroup = []; letterGroup = set()
        for state in stateGroup:
            for nextState in [transition.end for transition in state.outTransitions if transition.letter == letter]:
                if id(nextState) not in idsInLetterGroup:
                    letterGroup.add(nextState)
                    idsInLetterGroup.append(id(nextState))
        return letterGroup

    def _setStateIDs(self):
        start = self.nfa.start

        visited = []
        toVisit = [start]
        stateID = 0
        while len(toVisit) > 0:
            state = toVisit.pop()
            if id(state) in visited:
                continue
            visited.append(id(state))

            state.ID = stateID
            stateID += 1

            for nextState in [transition.end for transition in state.outTransitions]:
                toVisit.append(nextState)

'''
Start state:1 has children:
	 self -> None -> state:12
	 self -> None -> state:2
state:2 has children:
	 self -> None -> state:11
	 self -> None -> state:3
state:3 has children:
	 self -> None -> state:10
	 self -> None -> state:4
state:4 has children:
	 self -> (-1; -1; {enemy}) -> state:5
state:5 has children:
	 self -> None -> state:9
	 self -> None -> state:6
state:6 has children:
	 self -> (0; 0; {this.y!=WhiteY}) -> state:7
state:7 has children:
	 self -> (0; 0; {true}){PromoteToBlackQueen()} -> state:8
	 self -> None -> state:8
state:8 has children:
state:9 has children:
	 self -> (0; 0; {this.y==WhiteY}){BEnd()} -> state:7
state:10 has children:
	 self -> (-1; -1; {empty}) -> state:5
state:11 has children:
	 self -> (0; -1; {empty}) -> state:5
state:12 has children:
	 self -> None -> state:14
	 self -> None -> state:13
state:13 has children:
	 self -> (1; -1; {enemy}) -> state:5
state:14 has children:
	 self -> (1; -1; {empty}) -> state:5
The final state is: state:8
'''

'''
C:/Users/gudmu/AppData/Local/Programs/Python/Python310/python.exe -m NewFolderStruct.StateMachine.NfaToDfa 
'''

if __name__ == '__main__':
    nfa = NFAStateMachine()
    states = [State() for i in range(15)]
    nfa.start = states[1]
    states[1].addTransition(states[12], None)
    states[1].addTransition(states[2], None)

    states[2].addTransition(states[11], None)
    states[2].addTransition(states[3], None)


    states[3].addTransition(states[10], None)
    states[3].addTransition(states[4], None)

    states[4].addTransition(states[5], Letter(-1,-1,'enemy'))

    states[5].addTransition(states[9], None)
    states[5].addTransition(states[6], None)

    states[6].addTransition(states[7], Letter(0,0,'this.y!=WhiteY'))

    states[7].addTransition(states[8], Letter(0,0,'true', 'PromoteToBlackQueen()'))
    states[7].addTransition(states[8], None)

    states[9].addTransition(states[7], Letter(0,0,'this.y==WhiteY', 'BEnd()'))

    states[10].addTransition(states[5], Letter(-1,-1,'empty'))

    states[11].addTransition(states[5], Letter(0,-1,'empty'))

    states[12].addTransition(states[14], None)
    states[12].addTransition(states[13], None)

    states[13].addTransition(states[5], Letter(1,-1,'enemy'))

    states[14].addTransition(states[5], Letter(1,-1,'empty'))

    nfa.final = states[8]
    print(nfa)

    dfa = NfaToDfaConverter(nfa).createDFA()

    print(dfa)

