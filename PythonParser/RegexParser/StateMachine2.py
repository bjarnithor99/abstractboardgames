from __future__ import annotations
class Transition:
    def __init__(self, start: State, end: State, letter) -> None:
        self.start: State = start
        self.end: State = end
        self.letter = letter

    def __eq__(self, other: Transition) -> bool:
        return (self.start is other.start) and (self.end is other.end) and (self.letter == other.letter)
        

class State:
    def __init__(self) -> None:
        self.inTransitions: list[Transition] = []
        self.outTransitions: list[Transition] = []
        self.final = False

    def addTransition(self, other: State, letter):
        transition = Transition(self, other, letter)
        self.outTransitions.append(transition)
        other.inTransitions.append(transition)

class NFAStateMachine:
    def __init__(self) -> None:
        self.start = State()
        self.final = self.start

    def mergeStates(self, stateA: State, stateB: State):
        inTransitions: list[Transition] = [*stateA.inTransitions]
        for transition in stateB.inTransitions:
            tStart, tLetter = transition.start, transition.letter
            if not any([(t.start is tStart and t.letter == tLetter) for t in inTransitions]):
                inTransitions.append(transition)


        outTransitions: list[Transition] = [*stateA.outTransitions]
        for transition in stateB.outTransitions:
            tEnd, tLetter = transition.end, transition.letter
            if not any([(t.end is tEnd and t.letter == tLetter) for t in outTransitions]):
                outTransitions.append(transition)


        self.removeState(stateA)
        self.removeState(stateB)

        newState = State()
        # Move final and start
        if stateA is self.final or stateB is self.final:
            self.final = newState
        if stateA is self.start or stateB is self.start:
            self.start = newState

        for transition in inTransitions:
            start, letter = transition.start, transition.letter
            if start is stateA or start is stateB:
                start = newState
            self.addTransition(start, newState, letter)

        for transition in outTransitions:
            end, letter = transition.end, transition.letter
            if end is stateA or end is stateB:
                end = newState
            
            if Transition(newState, newState, letter) not in newState.inTransitions:
                self.addTransition(newState, end, letter)
            
        

    def addTransition(self, stateA: State, stateB: State, letter):
        transition = Transition(stateA, stateB, letter)
        stateA.outTransitions.append(transition)
        stateB.inTransitions.append(transition)

    def removeState(self, state: State):
        for transition in state.inTransitions:
            start: State = transition.start
            try:
                start.outTransitions.remove(transition)
            except ValueError:
                pass

        for transition in state.outTransitions:
            end: State = transition.end
            try:
                end.inTransitions.remove(transition)
            except ValueError:
                pass


    def getStatesInTransitions(self, state: State) -> list[Transition]:
        return state.inTransitions

    def getStatesOutTransitions(self, state: State) -> list[Transition]:
        return state.outTransitions


    def addState(self) -> State:
        return State()

    def __str__(self) -> str:
        returnStr = '\n'
        toVisit = [self.start]
        visited = []
        returnStr += 'Start '
        while len(toVisit) > 0:
            i = toVisit.pop()
            if id(i) in visited:
                continue
            visited.append(id(i))
            returnStr += str(id(i)) + ' has children:\n'
            for transition in i.outTransitions:
                toVisit.append(transition.end)
                returnStr += f'\t self -> {transition.letter} -> {id(transition.end)}\n'
        returnStr += f'The final state is: {id(self.final)}\n'
        for i, key in enumerate(visited):
            returnStr = returnStr.replace(str(key), f'state:{str(i+1)}')
        return returnStr


if __name__ == '__main__':
    nfa = NFAStateMachine()
    state = nfa.addState()
    nfa.addTransition(nfa.start, nfa.final, 'a')
    nfa.addTransition(nfa.start, state, 'b')

   # print(nfa)

    nfa.mergeStates(nfa.final, state)
    #print(nfa)

    nfa.mergeStates(nfa.final, nfa.final)
    #print(nfa)

    nfa2 = NFAStateMachine()
    a = nfa2.addState()
    b = nfa2.addState()
    c = nfa2.addState()

    nfa2.addTransition(nfa2.start, a, 'a')
    nfa2.addTransition(nfa2.start, b, 'a')
    nfa2.addTransition(a, c, 'a')
    nfa2.addTransition(b, c, 'a')
    nfa2.final = c
    print(nfa2)

    nfa2.mergeStates(nfa2.start, nfa2.final)
    print(nfa2)