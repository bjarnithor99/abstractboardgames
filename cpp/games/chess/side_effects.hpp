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
    /// @brief Reverses the changes made when this side effect was last
    ///  executed.
    virtual void operator()(Environment *environment) = 0;
    /// @brief Returns the name of the predicate. Useful for debugging.
    /// @return the name of the predicate as std::string.
    virtual std::string get_name() const = 0;

  protected:
    /// @brief Stores the state of the cells and their location before this side
    ///  effect was executed.
    std::stack<std::tuple<Cell, int, int>> cell_stack;
};

/// @brief A default side effect that captures the piece at (new_x, new_y) if any.
/// @author Bjarni Dagur Thor Kárason
class Default : public SideEffect
{
  public:
    Default();
    ~Default();
    void operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) override;
    void operator()(Environment *environment) override;
    std::string get_name() const override;

  private:
    std::stack<int> stagnation_stack;
};

/// @brief A side effect that promotes a Pawn to a Queen once it reaches the end
///  of the board.
/// @author Bjarni Dagur Thor Kárason
class PromoteToQueen : public SideEffect
{
  public:
    PromoteToQueen();
    ~PromoteToQueen();
    void operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) override;
    void operator()(Environment *environment) override;
    std::string get_name() const override;
};

/// @brief A side effect that promotes a Pawn to a Rook once it reaches the end
///  of the board.
/// @author Bjarni Dagur Thor Kárason
class PromoteToRook : public SideEffect
{
  public:
    PromoteToRook();
    ~PromoteToRook();
    void operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) override;
    void operator()(Environment *environment) override;
    std::string get_name() const override;
};

/// @brief A side effect that promotes a Pawn to a Bishop once it reaches the end
///  of the board.
/// @author Bjarni Dagur Thor Kárason
class PromoteToBishop : public SideEffect
{
  public:
    PromoteToBishop();
    ~PromoteToBishop();
    void operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) override;
    void operator()(Environment *environment) override;
    std::string get_name() const override;
};

/// @brief A side effect that promotes a Pawn to a Knight once it reaches the end
///  of the board.
/// @author Bjarni Dagur Thor Kárason
class PromoteToKnight : public SideEffect
{
  public:
    PromoteToKnight();
    ~PromoteToKnight();
    void operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) override;
    void operator()(Environment *environment) override;
    std::string get_name() const override;
};

/// @brief A side effect to indicate that a pawn is can be captured en passant.
/// @author Bjarni Dagur Thor Kárason
class SetEnPassantable : public SideEffect
{
  public:
    SetEnPassantable();
    ~SetEnPassantable();
    void operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) override;
    void operator()(Environment *environment) override;
    std::string get_name() const override;

  private:
    std::stack<std::tuple<int, int, int>> en_passant_stack;
};

/// @brief A side effect to castle to the right.
/// @author Bjarni Dagur Thor Kárason
class CastleLeft : public SideEffect
{
  public:
    CastleLeft();
    ~CastleLeft();
    void operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) override;
    void operator()(Environment *environment) override;
    std::string get_name() const override;
};

/// @brief A side effect to castle to the right.
/// @author Bjarni Dagur Thor Kárason
class CastleRight : public SideEffect
{
  public:
    CastleRight();
    ~CastleRight();
    void operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) override;
    void operator()(Environment *environment) override;
    std::string get_name() const override;
};

/// @brief A side effect castle to mark a piece as moved.
/// @author Bjarni Dagur Thor Kárason
class MarkMoved : public SideEffect
{
  public:
    MarkMoved();
    ~MarkMoved();
    void operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) override;
    void operator()(Environment *environment) override;
    std::string get_name() const override;

  private:
    std::stack<std::pair<bool *, bool>> moved_stack;
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
