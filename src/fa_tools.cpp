// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "fa_tools.hpp"

std::set<NFAState *> FATools::calculateEClosure(NFAState *state) {
    std::set<NFAState *> visited;
    std::queue<NFAState *> q;
    q.push(state);
    visited.insert(state);
    while (!q.empty()) {
        NFAState *at_state = q.front();
        q.pop();
        for (auto &p : at_state->transitions) {
            if (p.first.is_epsilon) {
                for (NFAState *next_state : p.second) {
                    if (visited.find(next_state) == visited.end()) {
                        q.push(next_state);
                        visited.insert(next_state);
                    }
                }
            }
        }
    }
    return visited;
}

std::set<NFAState *> FATools::calculateEClosure(std::set<NFAState *> states) {
    std::set<NFAState *> visited;
    for (NFAState *state : states) {
        std::set<NFAState *> merged;
        std::set<NFAState *> closure = calculateEClosure(state);
        std::merge(visited.begin(), visited.end(), closure.begin(), closure.end(), std::inserter(merged, merged.end()));
        visited = merged;
    }
    return visited;
}

std::set<NFAState *> FATools::move(std::set<NFAState *> states, NFAInput input) {
    std::set<NFAState *> visited;
    for (NFAState *state : states) {
        for (NFAState *next_state : state->transitions[input]) {
            visited.insert(next_state);
        }
    }
    return visited;
}

bool FATools::nfaSetContainsAcceptingState(std::set<NFAState *> states) {
    for (NFAState *state : states) {
        if (state->is_accepting)
            return true;
    }
    return false;
}

std::set<NFAInput> FATools::getAllInputSymbolsForSet(std::set<NFAState *> states) {
    std::set<NFAInput> input_symbols;
    for (NFAState *state : states) {
        for (auto &p : state->transitions) {
            if (!p.first.is_epsilon) {
                input_symbols.insert(p.first);
            }
        }
    }
    return input_symbols;
}

DFAState *FATools::nfaToDfa(NFAState *nfa_initial_state) {
    std::map<std::set<NFAState *>, DFAState *> nfa_set_to_dfa;
    std::set<NFAState *> nfa_initial_state_closure = calculateEClosure(nfa_initial_state);
    nfa_set_to_dfa[nfa_initial_state_closure] = new DFAState();
    nfa_set_to_dfa[nfa_initial_state_closure]->is_accepting = nfaSetContainsAcceptingState(nfa_initial_state_closure);
    std::queue<std::set<NFAState *>> q;
    q.push(nfa_initial_state_closure);
    while (!q.empty()) {
        std::set<NFAState *> at_nfa_set = q.front();
        q.pop();
        for (NFAInput input : getAllInputSymbolsForSet(at_nfa_set)) {
            std::set<NFAState *> next_nfa_set = calculateEClosure(move(at_nfa_set, input));
            if (nfa_set_to_dfa.find(next_nfa_set) == nfa_set_to_dfa.end()) {
                nfa_set_to_dfa[next_nfa_set] = new DFAState();
                nfa_set_to_dfa[next_nfa_set]->is_accepting = nfaSetContainsAcceptingState(next_nfa_set);
                q.push(next_nfa_set);
            }
            nfa_set_to_dfa[at_nfa_set]->add_transition(nfa_set_to_dfa[next_nfa_set],
                                                       DFAInput(input.dx, input.dy, input.predicate));
        }
    }
    return nfa_set_to_dfa[nfa_initial_state_closure];
}
