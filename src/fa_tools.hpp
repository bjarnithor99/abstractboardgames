// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include "dfa.hpp"
#include "nfa.hpp"
#include <algorithm>
#include <queue>
#include <set>

namespace FATools {
std::set<NFAState *> calculateEClosure(NFAState *state);
std::set<NFAState *> calculateEClosure(std::set<NFAState *> states);
std::set<NFAState *> move(std::set<NFAState *> states, NFAInput input);
bool nfaSetContainsAcceptingState(std::set<NFAState *> states);
std::set<NFAInput> getAllInputSymbolsForSet(std::set<NFAState *> states);
DFAState *nfaToDfa(NFAState *nfa_initial_state); // namespace FATools
} // namespace FATools
