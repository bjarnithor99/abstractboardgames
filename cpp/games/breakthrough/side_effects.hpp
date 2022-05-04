// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
/**
 *  @file side_effects.hpp
 *  @brief SideEffects for moves in abstract board games.
 *  @author Bjarni Dagur Thor Kárason
 *  @see Variables
 */
#pragma once

#include "environment.hpp"
#include "variables.hpp"
#include <memory>
#include <string>

/// @brief Class to represent user-defined side effect for moves in abstract
///  board games.
/// @author Bjarni Dagur Thor Kárason
class SideEffect
{
  public:
    /// @brief Class descructor.
    virtual ~SideEffect() {}
    /// @brief Executes the side effect of moving a piece from (old_x, old_y) to
    ///  (new_x, new_y) in an environment.
    /// @details
    ///  A (x, y) position corresponds to a cell in the environment's board,
    ///  environment->board[x][y].
    ///
    /// @pre The positions (old_x, old_y) and (new_x, new_y) are within the
    ///  environment's bounds.
    ///
    /// @see Environment
    ///
    /// @param environment pointer to an environment to execute the side effect in.
    /// @param old_x the x coordinate the piece is leaving.
    /// @param old_y the y coordinate the piece is leaving.
    /// @param new_x the x coordinate the piece is going to.
    /// @param new_y the y coordinate the piece is going to.
    /// @return void
    virtual void operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) = 0;
    /// @brief Returns the name of the predicate. Useful for debugging.
    /// @return the name of the predicate as std::string.
    virtual std::string get_name() const = 0;
};

/// @brief A default side effect that captures the piece at (new_x, new_y) if any.
/// @author Bjarni Dagur Thor Kárason
class Default : public SideEffect
{
  public:
    Default();
    ~Default();
    void operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) override;
    std::string get_name() const override;
};

/// @brief Class to store all side effects to use in game descriptions.
/// @author Bjarni Dagur Thor Kárason
class SideEffects
{
  public:
    /// @brief Maps side effect names to corresponding functors.
    /// @details
    ///  When side effects are referenced in game descriptions they will be
    ///  fetched from this map.
    static std::map<std::string, std::shared_ptr<SideEffect>> get_side_effect;
};
