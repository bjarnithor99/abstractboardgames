from .Parser import QuestionMark,Union,Concatination,Star,Plus,Letter,SyntaxTree,SyntaxTreeNode,Parser,lex
from .StateMachine2 import NFAStateMachine, State
StateMachine = NFAStateMachine

class Generator:
    def __init__(self, ast: SyntaxTree) -> None:
        self.ast = ast

    def createStatMachine(self) -> StateMachine:
        return self.createStateMachineFromNode(self.ast.rootNode)

    def createStateMachineFromNode(self, astNode: SyntaxTreeNode) -> StateMachine:
        astNodeToGen = {
            QuestionMark: self.questionmark,
            Star: self.star,
            Union: self.union,
            Concatination: self.concatination,
            Letter: self.letter,
            Plus: self.plus
        }
        gen = astNodeToGen[type(astNode)]
        return gen(astNode)

    def plus(self, plusNode: Plus) -> StateMachine:
        sm = self.createStateMachineFromNode(plusNode.child)
        sm.addTransition(sm.final, sm.start, None)
        newFinal = sm.addState()
        sm.addTransition(sm.final, newFinal, None)
        sm.final = newFinal
        return sm

    def concatination(self, concatinationNode: Concatination) -> StateMachine:
        sm1 = self.createStateMachineFromNode(concatinationNode.child1)
        sm2 = self.createStateMachineFromNode(concatinationNode.child2)
        a, b = sm1.final, sm2.start
        sm1.final = sm2.final
        sm1.mergeStates(a, b)
        return sm1

    def questionmark(self, questionmarkNode: QuestionMark) -> StateMachine:
        sm = self.createStateMachineFromNode(questionmarkNode.child)
        sm.addTransition(sm.start, sm.final, None)
        return sm

    def star(self, starNode: Star) -> StateMachine:
        sm = self.createStateMachineFromNode(starNode.child)
        newFinal = sm.addState()
        sm.addTransition(sm.final, newFinal, None)
        sm.mergeStates(sm.start, sm.final)
        sm.final = newFinal
        return sm

    def union(self, unionNode: Union) -> StateMachine:
        sm1 = self.createStateMachineFromNode(unionNode.child1)
        sm2 = self.createStateMachineFromNode(unionNode.child2)
        
        newSM = StateMachine()

        newSM.addTransition(newSM.start, sm1.start, None)
        newSM.addTransition(newSM.start, sm2.start, None)

        newSM.final = sm1.final
        newSM.mergeStates(sm1.final, sm2.final)

        return newSM

    def letter(self, letterNode: Letter) -> StateMachine:
        sm: StateMachine = StateMachine()
        sm.final = sm.addState()
        sm.addTransition(sm.start, sm.final, letterNode.value)
        return sm

if __name__ == "__main__":
    tokens = lex('((1,0,empty)*(1,0,enemy)?) | ((0,1,empty)*(0,1,enemy)?)')
    print("--TOKENS--")
    print([token.value for token in tokens])
    abs = Parser(tokens).parse()
    print("--ABSTRACT SYNTAX TREE--")
    print(abs)
    sm = Generator(abs).createStatMachine()
    print("--STATE MACHINE--")
    print(sm)
