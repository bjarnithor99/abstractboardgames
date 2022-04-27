// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
/**
 *  @file fa_tools.hpp
 *  @brief A collection of tools to work with state machines. Most notably to
 *   convert a regular expression abstract syntax tree to a minimized DFA.
 *  @author Bjarni Dagur Thor Kárason
 *  @see nfa.hpp
 *  @see dfa.hpp
 *  @see ast.hpp
 */
#pragma once

#include "ast.hpp"
#include "dfa.hpp"
#include "nfa.hpp"
#include "nfa_visitor.hpp"
#include <algorithm>
#include <fstream>
#include <queue>
#include <set>

/// @brief A collection of tools to work with state machines. Most notably to
///  convert a regular expression abstract syntax tree to a minimized DFA.
///
/// @author Bjarni Dagur Thor Kárason
namespace FATools {
/// @brief Finds all states reachable from an initial state.
/// @details
///  Includes the initial state.
///
/// @param initial_state the initial state to find reachable states form.
///
/// @returns the set of reachable states from \p initial_state.
std::set<DFAState *> get_all_states(DFAState *initial_state);
/// @brief Finds all states reachable from an initial state.
/// @details
///  Includes the initial state.
///
/// @param initial_state the initial state to find reachable states form.
///
/// @returns the set of reachable states from \p initial_state.
std::set<NFAState *> get_all_states(NFAState *initial_state);
/// @brief Writes a DFA as a dot graph to a file.
///
/// @param initial_state the initial state of a DFA to convert to dot format.
/// @param output_path the output file to write the results to.
void to_dot(DFAState *initial_state, std::string output_path);
/// @brief Writes a NFA as a dot graph to a file.
///
/// @param initial_state the initial state of a NFA to convert to dot format.
/// @param output_path the output file to write the results to.
void to_dot(NFAState *initial_state, std::string output_path);
/// @brief Calculates the epsilon-closure of a NFAState.
/// @details
///  Calculates the set of states reachable from \p state using only
///  epsilon-transitions.
///
/// @param state the state to calculate epsilon-closure for.
///
/// @returns the epsilon-closure of state as a set of NFAState pointers.
std::set<NFAState *> calculateEClosure(NFAState *state);
/// @brief Calculates the epsilon-closure of a set of NFAState.
/// @details
///  Calculates the set of states reachable from \p states using only
///  epsilon-transitions.
///
/// @param states set of states to calculate epsilon-closure for.
///
/// @returns the epsilon-closure of states as a set of NFAState pointers.
std::set<NFAState *> calculateEClosure(std::set<NFAState *> states);
/// @brief Calculates move from \p states given \p input.
/// @details
///  Calculates the set of states reachable from \p states by transitioning only
///  on \p input.
///
/// @param states set of states to calculate move form.
/// @param input the input letter to transition on.
///
/// @returns the set of states reachable from \p states given \p input.
std::set<NFAState *> move(std::set<NFAState *> states, NFAInput input);
/// @brief Checks whether set of states contains an accepting state.
///
/// @param states the states to check.
///
/// @returns true if \p states contains an accepting state.
/// @returns false if \p states does not contain an accepting state.
bool nfaSetContainsAcceptingState(std::set<NFAState *> states);
/// @brief Calculates the set of letters over all transitions from \p states.
///
/// @param states the set of states to find input letters from.
///
/// @returns the set of DFAInput letters over all transition from \p states.
std::set<DFAInput> getAllInputSymbolsForSet(std::set<DFAState *> states);
/// @brief Calculates the set of letters over all transitions from \p states (excluding epsilon).
///
/// @param states the set of states to find input letters from.
///
/// @returns the set of NFAInput letters over all transition from \p states.
std::set<NFAInput> getAllInputSymbolsForSet(std::set<NFAState *> states);
/// @brief Converts an NFA to an equivalent DFA.
/// @details
///  Uses subset construction.
///
/// @param nfa_initial_state the initial state of the NFA to convert to a DFA.
///
/// @returns the initial state of an equivalent DFA.
std::unique_ptr<DFAState, DFAStateDeleter> nfaToDfa(NFAState *nfa_initial_state); // namespace FATools
/// @brief Finds all parent states of \p child with \p input.
/// @details
///  Finds all states with a transition on \p input to \p child.
///
/// @param child the state to find parents of.
/// @param all_states the set of all states.
/// @param input the letter to search over.
///
/// @returns a set of parents of \p child.
std::set<DFAState *> getParents(DFAState *child, std::set<DFAState *> *all_states, DFAInput input);
/// @brief Finds all parent states of \p children with \p input.
/// @details
///  Finds all states such that
///  Finds all states that have a transition in \p input to some child in \p children.
///
/// @param children the set of states to find parents of.
/// @param all_states the set of all states.
/// @param input the letter to search over.
///
/// @returns a set of parents of \p child.
std::set<DFAState *> getParents(std::set<DFAState *> children, std::set<DFAState *> *all_states, DFAInput input);
/// @brief Minimizes a DFA.
/// @details
///  Uses Hopcroft's algorithm.
///
/// @param initial_state the initial state of the DFA to minimize.
///
/// @returns the initial state of the equivalent minimized DFA.
std::unique_ptr<DFAState, DFAStateDeleter> minimizeDfa(DFAState *initial_state);
/// @brief Finds a minimized DFA equivalent to a regular expression abstract syntax tree.
///
/// @param node the root of the regular expression syntax tree to convert to DFA.
///
/// @returns the initial state of the equivalent minimized DFA.
std::unique_ptr<DFAState, DFAStateDeleter> getMinimizedDfa(Node *node);
} // namespace FATools
