// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "dfa.hpp"

DFAInput::DFAInput(int dx, int dy, std::shared_ptr<Predicate> predicate, std::string side_effect)
    : dx(dx), dy(dy), predicate(predicate), side_effect(side_effect) {}
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
    if (this->predicate > rhs.predicate)
        return false;
    if (this->side_effect < rhs.side_effect)
        return true;
    return this->side_effect > rhs.side_effect;
}
bool DFAInput::operator==(const DFAInput &rhs) const {
    return this->dx == rhs.dx && this->dy == rhs.dy && this->predicate == rhs.predicate;
}

DFAState::DFAState() : is_accepting(false) {}
DFAState::~DFAState() {}
void DFAState::add_transition(DFAState *dst, DFAInput input) {
    transition[input] = dst;
}
void DFAState::destroy() {
    std::set<DFAState *> states;
    std::queue<DFAState *> q;
    states.insert(this);
    q.push(this);
    while (!q.empty()) {
        DFAState *at = q.front();
        q.pop();
        for (auto &p : at->transition) {
            auto &next_state = p.second;
            if (states.find(next_state) == states.end()) {
                states.insert(next_state);
                q.push(next_state);
            }
        }
    }
    states.erase(this);
    for (DFAState *state : states)
        delete state;
    delete this;
}

void DFAStateDeleter::operator()(DFAState *state) const {
    state->destroy();
}
