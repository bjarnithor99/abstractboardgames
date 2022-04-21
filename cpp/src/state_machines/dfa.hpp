// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
/**
 *  @file dfa.hpp
 *  @brief Deterministic finite automata to represent regular expressions.
 *  @author Bjarni Dagur Thor Kárason
 *  @see nfa.hpp
 */
#pragma once

#include <map>
#include <memory>
#include <queue>
#include <set>
#include <string>

class Predicate;
class SideEffect;

/// @brief Class to represent an edge in a DFA.
/// @details
///  Each edge in the DFA is labelled with a Letter describing piece movement.
///  Transition along an edge is available if the edge's predicate holds. An
///  accepting run of the DFA corresponds to a legal piece move.
///
/// @see DFAState
///
/// @note See game description language grammar for details on a Letter.
///
/// @author Bjarni Dagur Thor Kárason
class DFAInput
{
  public:
    /// @brief DFAInput constructor.
    ///
    /// @param dx the move's delta along the x axis.
    /// @param dy the move's delta along the y axis.
    /// @param predicate a Predicate functor describing when the move is legal.
    /// @param side_effect a SideEffect functor describing the move's side effects.
    DFAInput(int dx, int dy, std::shared_ptr<Predicate> predicate, std::shared_ptr<SideEffect> side_effect);
    /// @brief DFAInput destructor.
    ~DFAInput();
    /// @brief Defines ordering for DFAInput.
    /// @details
    ///  Necessary to insert DFAInput into maps.
    ///
    /// @see DFAState::transition
    bool operator<(const DFAInput &rhs) const;
    /// @brief Defines when two DFAInput are equal.
    /// @details
    ///  Necessary to minimze DFA.
    bool operator==(const DFAInput &rhs) const;

    /// @brief the encoded move's delta along the x axis.
    int dx;
    /// @brief the encoded move's delta along the y axis.
    int dy;
    /// @brief the encoded move's Predicate.
    std::shared_ptr<Predicate> predicate;
    /// @brief the encoded move's SideEffect.
    std::shared_ptr<SideEffect> side_effect;
};

/// @brief Class to represent a node in a NFA.
/// @details
///  Each node in the DFA can have at most one transition for each DFAInput
///  which is available depending on the edge's predicate. A run of the DFA that
///  ends at an accepting state is an accepting run, and the input corresponds
///  to a legal piece move.
///
/// @see DFAInput
///
/// @author Bjarni Dagur Thor Kárason
class DFAState
{
  public:
    /// @brief DFAState constructor.
    DFAState();
    /// @brief NFAState destructor.
    ~DFAState();
    /// @brief Add a transition from the current state.
    /// @details
    ///  Adds a transition from the current state to \p dst that is available
    ///  when \p input is satisfied.
    ///
    /// @see NFAInput
    ///
    /// @param dst the destination state of the transition.
    /// @param input the edge's label
    void add_transition(DFAState *dst, DFAInput input);
    /// @brief Frees all the memory allocated for a DFA whose root is the
    ///  current state.
    void destroy();

    /// @brief True if the current state is an accepting state, false otherwise.
    bool is_accepting;
    /// @brief Stores all transitions from the current state.
    /// @details
    ///  Map that takes as input DFAInput and returns a possible destination
    ///  state given that input, if there is one.
    std::map<DFAInput, DFAState *> transition;
};

/// @brief A Deleter class for std::unique_ptr<DFAState>.
struct DFAStateDeleter {
    /// @brief Defines how a DFAState managed by std::unique_ptr should be
    ///  destroyed.
    void operator()(DFAState *state) const;
};
