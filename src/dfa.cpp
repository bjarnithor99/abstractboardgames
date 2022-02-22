// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "dfa.hpp"

DFAInput::DFAInput(int dx, int dy, std::string predicate) : dx(dx), dy(dy), predicate(predicate) {}
DFAInput::~DFAInput() {}
bool DFAInput::operator<(const DFAInput &rhs) const {
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
bool DFAInput::operator==(const DFAInput &rhs) const {
    return this->dx == rhs.dx && this->dy == rhs.dy && this->predicate == rhs.predicate;
}

DFAState::DFAState() : is_accepting(false) {}
void DFAState::add_transition(DFAState *dst, DFAInput input) {
    transition[input] = dst;
}
