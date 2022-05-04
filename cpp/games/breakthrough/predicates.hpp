// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
/**
 *  @file predicates.hpp
 *  @brief Predicates for moves in abstract board games.
 *  @author Bjarni Dagur Thor Kárason
 *  @see Variables
 */
#pragma once

#include "environment.hpp"
#include "variables.hpp"
#include <memory>
#include <string>

/// @brief Class to represent user-defined predicates for moves in abstract
///  board games.
/// @author Bjarni Dagur Thor Kárason
class Predicate
{
  public:
    /// @brief Class destructor.
    virtual ~Predicate() {}
    /// @brief Evaluates the predicate at the position (x, y) in an environment.
    /// @details
    ///  The (x, y) position corresponds to cell in the environment's board,
    ///  environment->board[x][y].
    ///
    /// @pre The position (x, y) is within the environment's bounds.
    ///
    /// @see Environment
    ///
    /// @param environment pointer to an environment to evaluate the predicate in.
    /// @param x the x coordinate to evaluate the predicate at.
    /// @param y the y coordinate to evaluate the predicate at.
    /// @return true if the predicate holds at (x, y).
    /// @return false if the predicate does not hold at (x, y).
    virtual bool operator()(Environment *environment, int x, int y) = 0;
    /// @brief Returns the name of the predicate. Useful for debugging.
    /// @return the name of the predicate as std::string.
    virtual std::string get_name() const = 0;
};

/// @brief A predicate to check if (x, y) is empty.
/// @author Bjarni Dagur Thor Kárason
class Empty : public Predicate
{
  public:
    Empty();
    ~Empty();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if (x, y) contains an opponent piece.
/// @author Bjarni Dagur Thor Kárason
class Opponent : public Predicate
{
  public:
    Opponent();
    ~Opponent();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief Class to store all predicates to use in game descriptions.
/// @author Bjarni Dagur Thor Kárason
class Predicates
{
  public:
    /// @brief Maps predicate names to corresponding functors.
    /// @details
    ///  When predicates are referenced in game descriptions they will be
    ///  fetched from this map.
    static std::map<std::string, std::shared_ptr<Predicate>> get_predicate;
};
