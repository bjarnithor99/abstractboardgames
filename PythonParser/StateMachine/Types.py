from .StateMachine import NFAStateMachine, Transition, State

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
