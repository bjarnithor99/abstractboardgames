// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "nfa.hpp"

NFAInput::NFAInput() : dx(0), dy(0), predicate(nullptr), is_epsilon(true) {}
NFAInput::NFAInput(const LetterNode &letterNode)
    : dx(letterNode.dx), dy(letterNode.dy), predicate(letterNode.predicate), side_effect(letterNode.side_effect),
      is_epsilon(false) {}
NFAInput::~NFAInput() {}
bool NFAInput::operator<(const NFAInput &rhs) const {
    return std::tie(this->dx, this->dy, this->predicate, this->side_effect, this->is_epsilon) <
           std::tie(rhs.dx, rhs.dy, rhs.predicate, rhs.side_effect, rhs.is_epsilon);
}

NFAState::NFAState() : is_accepting(false) {}
NFAState::~NFAState() {}
void NFAState::add_transition(NFAState *dst, NFAInput input) {
    transitions[input].push_back(dst);
}
void NFAState::destroy() {
    std::set<NFAState *> states;
    std::queue<NFAState *> q;
    states.insert(this);
    q.push(this);
    while (!q.empty()) {
        NFAState *at = q.front();
        q.pop();
        for (auto &p : at->transitions) {
            for (auto &next_state : p.second) {
                if (states.find(next_state) == states.end()) {
                    states.insert(next_state);
                    q.push(next_state);
                }
            }
        }
    }
    states.erase(this);
    for (NFAState *state : states)
        delete state;
    delete this;
}

void NFAStateDeleter::operator()(NFAState *state) const {
    state->destroy();
}
