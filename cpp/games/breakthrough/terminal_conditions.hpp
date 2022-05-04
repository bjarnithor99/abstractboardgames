// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
/**
 *  @file terminal_conditions.hpp
 *  @brief TerminalConditions for abstract board games.
 *  @author Bjarni Dagur Thor Kárason
 *  @see Variables
 */
#pragma once

#include "environment.hpp"
#include "predicates.hpp"
#include "variables.hpp"
#include <memory>
#include <string>

/// @brief Class to represent user-defined terminal conditions for abstract
///  board games.
/// @details
///  Terminal conditions are checked after each player makes a move, and also if
///  a player has no legal moves. Terminal conditions can have side effect such
///  as declaring the game to be over and setting the players' final scores.
/// @see Variables
/// @author Bjarni Dagur Thor Kárason
class TerminalCondition
{
  public:
    /// @brief Class destructor.
    virtual ~TerminalCondition() {}
    /// @brief Evaluates the terminal condition in an environment.
    ///
    /// @see Environment
    ///
    /// @param environment pointer to an environment to evaluate the terminal condition in.
    /// @return true if the terminal condition holds.
    /// @return false if the terminal condition does not holds.
    virtual bool operator()(Environment *environment) = 0;
    /// @brief Returns the name of the predicate. Useful for debugging.
    /// @return the name of the predicate as std::string.
    virtual std::string get_name() const = 0;
};

/// @brief A terminal condition that checks if the current player has no legal moves.
/// @author Bjarni Dagur Thor Kárason
class NoMovesLeft : public TerminalCondition
{
  public:
    NoMovesLeft();
    ~NoMovesLeft();
    bool operator()(Environment *environment) override;
    std::string get_name() const override;
};

/// @brief A terminal condition that checks if the white player has reached the opposite end of the board.
/// @author Bjarni Dagur Thor Kárason
class WhiteReachedEnd : public TerminalCondition
{
  public:
    WhiteReachedEnd();
    ~WhiteReachedEnd();
    bool operator()(Environment *environment) override;
    std::string get_name() const override;
};

/// @brief A terminal condition that checks if the black player has reached the opposite end of the board.
/// @author Bjarni Dagur Thor Kárason
class BlackReachedEnd : public TerminalCondition
{
  public:
    BlackReachedEnd();
    ~BlackReachedEnd();
    bool operator()(Environment *environment) override;
    std::string get_name() const override;
};

/// @brief Class to store all terminal conditions to use in game descriptions.
/// @author Bjarni Dagur Thor Kárason
class TerminalConditions
{
  public:
    /// @brief Maps terminal condition names to corresponding functors.
    /// @details
    ///  When terminal conditions are checked they will be fetched from this
    ///  map.
    static std::map<std::string, std::shared_ptr<TerminalCondition>> terminal_conditions;
};
