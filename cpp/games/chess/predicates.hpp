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

/// @brief A predicate that is always true.
/// @details
///  Useful for setting variables.
/// @author Bjarni Dagur Thor Kárason
class True : public Predicate
{
  public:
    True();
    ~True();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
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

/// @brief A predicate to check if (x, y) contains a WhitePawn.
/// @author Bjarni Dagur Thor Kárason
class WhitePawn : public Predicate
{
  public:
    WhitePawn();
    ~WhitePawn();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if (x, y) contains a WhiteKnight.
/// @author Bjarni Dagur Thor Kárason
class WhiteKnight : public Predicate
{
  public:
    WhiteKnight();
    ~WhiteKnight();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if (x, y) contains a WhiteBishop.
/// @author Bjarni Dagur Thor Kárason
class WhiteBishop : public Predicate
{
  public:
    WhiteBishop();
    ~WhiteBishop();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if (x, y) contains a WhiteRook.
/// @author Bjarni Dagur Thor Kárason
class WhiteRook : public Predicate
{
  public:
    WhiteRook();
    ~WhiteRook();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if (x, y) contains a WhiteQueen.
/// @author Bjarni Dagur Thor Kárason
class WhiteQueen : public Predicate
{
  public:
    WhiteQueen();
    ~WhiteQueen();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if (x, y) contains a White King.
/// @author Bjarni Dagur Thor Kárason
class WhiteKing : public Predicate
{
  public:
    WhiteKing();
    ~WhiteKing();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if (x, y) contains a BlackPawn.
/// @author Bjarni Dagur Thor Kárason
class BlackPawn : public Predicate
{
  public:
    BlackPawn();
    ~BlackPawn();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if (x, y) contains a BlackKnight.
/// @author Bjarni Dagur Thor Kárason
class BlackKnight : public Predicate
{
  public:
    BlackKnight();
    ~BlackKnight();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if (x, y) contains a BlackBishop.
/// @author Bjarni Dagur Thor Kárason
class BlackBishop : public Predicate
{
  public:
    BlackBishop();
    ~BlackBishop();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if (x, y) contains a BlackRook.
/// @author Bjarni Dagur Thor Kárason
class BlackRook : public Predicate
{
  public:
    BlackRook();
    ~BlackRook();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if (x, y) contains a BlackQueen.
/// @author Bjarni Dagur Thor Kárason
class BlackQueen : public Predicate
{
  public:
    BlackQueen();
    ~BlackQueen();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if (x, y) contains a BlackKing.
/// @author Bjarni Dagur Thor Kárason
class BlackKing : public Predicate
{
  public:
    BlackKing();
    ~BlackKing();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if (x, y) is a pawn's initial position.
/// @author Bjarni Dagur Thor Kárason
class PawnInitialRow : public Predicate
{
  public:
    PawnInitialRow();
    ~PawnInitialRow();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if (x, y) is in a player's final row.
/// @author Bjarni Dagur Thor Kárason
class FinalRow : public Predicate
{
  public:
    FinalRow();
    ~FinalRow();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if (x, y) is not in a player's final row.
/// @author Bjarni Dagur Thor Kárason
class NotFinalRow : public Predicate
{
  public:
    NotFinalRow();
    ~NotFinalRow();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if (x, y) can be captured en passant.
/// @author Bjarni Dagur Thor Kárason
class EnPassantable : public Predicate
{
  public:
    EnPassantable();
    ~EnPassantable();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if the current player has the right to castle to the left.
/// @author Bjarni Dagur Thor Kárason
class RightToCastleLeft : public Predicate
{
  public:
    RightToCastleLeft();
    ~RightToCastleLeft();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if the current player has the right to castle to the right.
/// @author Bjarni Dagur Thor Kárason
class RightToCastleRight : public Predicate
{
  public:
    RightToCastleRight();
    ~RightToCastleRight();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

/// @brief A predicate to check if (x, y) is not being attacked by the opponent.
/// @author Bjarni Dagur Thor Kárason
class NotAttacked : public Predicate
{
  public:
    NotAttacked();
    ~NotAttacked();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;

  private:
    bool attacked_by_pawn(Environment *environment, int x, int y);
    bool attacked_by_knight(Environment *environment, int x, int y);
    bool attacked_diagonally(Environment *environment, int x, int y);
    bool attacked_straight(Environment *environment, int x, int y);
    bool attacked_coef(Environment *environment, int x, int y, int x_coef, int y_coef,
                       const std::vector<std::string> &opponents);
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
