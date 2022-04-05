// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include "ast.hpp"
#include "dfa.hpp"
#include "nfa.hpp"
#include "nfa_visitor.hpp"
#include <algorithm>
#include <queue>
#include <set>

namespace FATools {
std::set<NFAState *> calculateEClosure(NFAState *state);
std::set<NFAState *> calculateEClosure(std::set<NFAState *> states);
std::set<NFAState *> move(std::set<NFAState *> states, NFAInput input);
bool nfaSetContainsAcceptingState(std::set<NFAState *> states);
std::set<NFAInput> getAllInputSymbolsForSet(std::set<NFAState *> states);
std::unique_ptr<DFAState, DFAStateDeleter> nfaToDfa(NFAState *nfa_initial_state); // namespace FATools
std::set<DFAState *> getParents(DFAState *child, std::set<DFAState *> *all_states, DFAInput input);
std::set<DFAState *> getParents(std::set<DFAState *> children, std::set<DFAState *> *all_states, DFAInput input);
std::unique_ptr<DFAState, DFAStateDeleter> minimizeDfa(DFAState *initial_state);
std::unique_ptr<DFAState, DFAStateDeleter> getMinimizedDfa(Node *node);
} // namespace FATools
