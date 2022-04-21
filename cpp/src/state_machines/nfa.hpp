// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
/**
 *  @file nfa.hpp
 *  @brief Non-deterministic finite automata to represent regular expressions.
 *  @author Bjarni Dagur Thor Kárason
 *  @see dfa.hpp
 */
#pragma once

#include "ast.hpp"
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <vector>

class Predicate;
class SideEffect;

/// @brief Class to represent an edge in a NFA.
/// @details
///  Each edge in the NFA is labelled with a Letter describing piece movement,
///  or is an epsilon-edge.  Transition along an edge is available if the edge's
///  predicate holds, or if it is an epsilon-transition. An accepting run of the
///  NFA corresponds to a legal piece move.
///
/// @see NFAState
///
/// @note See game description language grammar for details on a Letter.
///
/// @author Bjarni Dagur Thor Kárason
class NFAInput
{
  public:
    /// @brief NFAInput constructor.
    NFAInput();
    /// @brief NFAInput constructor from Letter components.
    ///
    /// @param letterNode the Letter to label the NFA edge with.
    ///
    /// @see LetterNode
    NFAInput(const LetterNode &letterNode);
    /// @brief NFAInput desctructor.
    ~NFAInput();
    /// @brief Defines ordering for NFAInput.
    /// @details
    ///  Necessary to insert NFAInput into maps.
    ///
    /// @see NFAState#transitions
    bool operator<(const NFAInput &rhs) const;

    /// @brief the encoded move's delta along the x axis.
    int dx;
    /// @brief the encoded move's delta along the y axis.
    int dy;
    /// @brief the encoded move's Predicate.
    std::shared_ptr<Predicate> predicate;
    /// @brief the encoded move's SideEffect.
    std::shared_ptr<SideEffect> side_effect;
    /// @brief if true then the edge is an epsilon-transition, else it is a Letter transition.
    bool is_epsilon;
};

/// @brief Class to represent a node in a NFA.
/// @details
///  Each node in the NFA can have any number of transitions for each NFAInput
///  which are available depending on the edge's predicate. A run of the NFA
///  that ends at an accepting state is an accepting run, and the input
///  corresponds to a legal piece move.
///
/// @see NFAInput
///
/// @author Bjarni Dagur Thor Kárason
class NFAState
{
  public:
    /// @brief NFAState constructor.
    NFAState();
    /// @brief NFAState destructor.
    ~NFAState();
    /// @brief Add a transition from the current state.
    /// @details
    ///  Adds a transition from the current state to \p dst that is available
    ///  when \p input is satisfied.
    ///
    /// @see NFAInput
    ///
    /// @param dst the destination state of the transition.
    /// @param input the edge's label
    void add_transition(NFAState *dst, NFAInput input);
    /// @brief Frees all the memory allocated for a NFA whose root is the
    ///  current state.
    void destroy();

    /// @brief True if the current state is an accepting state, false otherwise.
    bool is_accepting;
    /// @brief Stores all transitions from the current state.
    /// @details
    ///  Map that takes as input NFAInput and returns a vector of possible
    ///  destination states given that input.
    std::map<NFAInput, std::vector<NFAState *>> transitions;
};

/// @brief A Deleter class for std::unique_ptr<NFAState>.
struct NFAStateDeleter {
    /// @brief Defines how a NFAState managed by std::unique_ptr should be
    ///  destroyed.
    void operator()(NFAState *state) const;
};
