// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "nfa_visitor.hpp"

NFAWrapper::NFAWrapper(NFAState *initial_state, NFAState *final_state)
    : initial_state(initial_state), final_state(final_state) {}
NFAWrapper::~NFAWrapper() {}

NFAVisitor::NFAVisitor() {}
NFAVisitor::~NFAVisitor() {}

void NFAVisitor::visitWordsNode(WordsNode *wordsNode) {
    int toConcat = 0;
    for (const std::unique_ptr<Node> &wordNode : wordsNode->wordNodes) {
        toConcat++;
        wordNode->accept(this);
    }
    while (toConcat >= 2) {
        std::unique_ptr<NFAWrapper> nfa_wrapper_rhs = std::move(nfa_stack.top());
        nfa_stack.pop();
        std::unique_ptr<NFAWrapper> nfa_wrapper_lhs = std::move(nfa_stack.top());
        nfa_stack.pop();

        // To concatenate two NFAs A and B:
        // 1) Connect the final state of A to the initial state of B with an epsilon transition.
        nfa_wrapper_lhs->final_state->add_transition(nfa_wrapper_rhs->initial_state, NFAInput());
        // 2) The final state is A is no longer an accepting state.
        nfa_wrapper_lhs->final_state->is_accepting = false;

        nfa_stack.push(std::make_unique<NFAWrapper>(nfa_wrapper_lhs->initial_state, nfa_wrapper_rhs->final_state));

        toConcat--;
    }
}

void NFAVisitor::visitLetterNode(LetterNode *letterNode) {
    NFAState *initial_state = new NFAState();
    NFAState *final_state = new NFAState();
    initial_state->add_transition(
        final_state, NFAInput(letterNode->dx, letterNode->dy, letterNode->predicate, letterNode->side_effect));
    final_state->is_accepting = true;
    nfa_stack.push(std::make_unique<NFAWrapper>(initial_state, final_state));
}

void NFAVisitor::visitMacroLetterNode(MacroLetterNode *macroLetterNode) {
    throw std::runtime_error("MacroLetterNodes are not allowed in NFAVisitor.");
}

void NFAVisitor::visitBinaryOpNode(BinaryOpNode *binaryOpNode) {
    binaryOpNode->childNodeLHS->accept(this);
    binaryOpNode->childNodeRHS->accept(this);

    std::unique_ptr<NFAWrapper> nfa_wrapper_rhs = std::move(nfa_stack.top());
    nfa_stack.pop();
    std::unique_ptr<NFAWrapper> nfa_wrapper_lhs = std::move(nfa_stack.top());
    nfa_stack.pop();

    NFAState *initial_state = new NFAState();
    NFAState *final_state = new NFAState();

    switch (binaryOpNode->binaryOperator) {
    case BinaryOperator::OpOr:
        // To union two NFAs A and B:
        // 1) Connect the new initial state to the initial state of A and to the initial state of B with epsilon
        // transitions.
        initial_state->add_transition(nfa_wrapper_lhs->initial_state, NFAInput());
        initial_state->add_transition(nfa_wrapper_rhs->initial_state, NFAInput());
        // 2) Connect the final state of A and the final state of B to the new final state with epsilon transitions.
        nfa_wrapper_lhs->final_state->add_transition(final_state, NFAInput());
        nfa_wrapper_rhs->final_state->add_transition(final_state, NFAInput());
        // 3) The final states of A and B are no longer accepting states.
        nfa_wrapper_lhs->final_state->is_accepting = false;
        nfa_wrapper_rhs->final_state->is_accepting = false;
        // 4) Instead the new final state is an accepting state.
        final_state->is_accepting = true;
    }

    nfa_stack.push(std::make_unique<NFAWrapper>(initial_state, final_state));
}

void NFAVisitor::visitUnaryOpNode(UnaryOpNode *unaryOpNode) {
    unaryOpNode->childNode->accept(this);

    std::unique_ptr<NFAWrapper> nfa_wrapper = std::move(nfa_stack.top());
    nfa_stack.pop();

    NFAState *initial_state = new NFAState();
    NFAState *final_state = new NFAState();

    switch (unaryOpNode->unaryOperator) {
    case UnaryOperator::OpStar:
        // To star a NFA A:
        // 1) Connect the new initial state to the new final state with an epsilon transition.
        initial_state->add_transition(final_state, NFAInput());
        // 2) Connect the new initial state to the initial state of A with an epsilon transition.
        initial_state->add_transition(nfa_wrapper->initial_state, NFAInput());
        // 3) Connect the final state of A to the new final state with an epsilon transition.
        nfa_wrapper->final_state->add_transition(final_state, NFAInput());
        // 4) Connect the final state of A to the initial state of A with an epsilon transition.
        nfa_wrapper->final_state->add_transition(nfa_wrapper->initial_state, NFAInput());
        break;
    case UnaryOperator::OpQuestion:
        // To question a NFA A:
        // 1) Connect the new initial state to the new final state with an epsilon transition.
        initial_state->add_transition(final_state, NFAInput());
        // 2) Connect the new initial state to the initial state of A with an epsilon transition.
        initial_state->add_transition(nfa_wrapper->initial_state, NFAInput());
        // 3) Connect the final state of A to the new final state with an epsilon transition.
        nfa_wrapper->final_state->add_transition(final_state, NFAInput());
        break;
    case UnaryOperator::OpPlus:
        // To plus a NFA A:
        // 1) Connect the new initial state to the initial state of A with an epsilon transition.
        initial_state->add_transition(nfa_wrapper->initial_state, NFAInput());
        // 2) Connect the final state of A to the new final state with an epsilon transition.
        nfa_wrapper->final_state->add_transition(final_state, NFAInput());
        // 3) Connect the final state of A to the initial state of A with an epsilon transition.
        nfa_wrapper->final_state->add_transition(nfa_wrapper->initial_state, NFAInput());
        break;
    }
    // In all cases the final state of A is no longer an accepting state.
    nfa_wrapper->final_state->is_accepting = false;
    // Instead the new final state is an accepting state.
    final_state->is_accepting = true;

    nfa_stack.push(std::make_unique<NFAWrapper>(initial_state, final_state));
}

std::unique_ptr<NFAState, NFAStateDeleter> NFAVisitor::getNFA() {
    std::unique_ptr<NFAState, NFAStateDeleter> nfa_initial_state(nfa_stack.top()->initial_state);
    nfa_stack.pop();
    return nfa_initial_state;
}
