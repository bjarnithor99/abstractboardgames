// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
/**
 *  @file variables.hpp
 *  @brief Variables to use in predicates, side effects, and terminal conditions.
 *  @author Bjarni Dagur Thor Kárason
 *  @see Predicate
 *  @see SideEffect
 *  @see TerminalCondition
 */
#pragma once

#include <tuple>
#include <utility>

/// @brief Class to keep track of user-defined variables.
/// @details
///  The following variables are necessary to facilitate learning and in
///  terminal condition checks:
///  @li black_score,
///  @li white_score,
///  @li n_moves_found,
///  @li game_over.
///
/// @see TerminalConditions
///
/// @author Bjarni Dagur Thor Kárason
class Variables
{
  public:
    /// @brief Class constructor.
    Variables();
    /// @brief Class copy constructor.
    Variables(const Variables &variables);
    /// @brief Class destructor.
    ~Variables();

    /// @brief Keeps track of black's score.
    /// @details Necessary to determine winner once game is over.
    int black_score;
    /// @brief Keeps track of white's score.
    /// @details Necessary to determine winner once game is over.
    int white_score;
    /// @brief Keeps track of number of possible legal moves from the current environment state.
    /// @details Necessary to check for draw.
    int n_moves_found;
    /// @brief Keeps track of the current state of the game.
    /// @details True if the game is over, false otherwise.
    bool game_over;
};
