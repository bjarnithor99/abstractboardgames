// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "nfa.hpp"

NFAInput::NFAInput() : dx(0), dy(0), predicate("none"), is_epsilon(true) {}
NFAInput::NFAInput(int dx, int dy, std::string predicate) : dx(dx), dy(dy), predicate(predicate), is_epsilon(false) {}
NFAInput::~NFAInput() {}
bool NFAInput::operator<(const NFAInput &rhs) const {
    if (this->dx < rhs.dx)
        return true;
    if (this->dx > rhs.dx)
        return false;
    if (this->dy < rhs.dy)
        return true;
    if (this->dy > rhs.dy)
        return false;
    if (this->predicate < rhs.predicate)
        return true;
    return this->predicate > rhs.predicate;
}

NFAState::NFAState() : is_accepting(false) {}
void NFAState::add_transition(NFAState *dst, NFAInput input) {
    transitions[input].push_back(dst);
}
