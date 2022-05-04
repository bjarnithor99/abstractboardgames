// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
/**
 *  @file environment.hpp
 *  @brief Defines classes to keep track of the current environment of an abstract board game.
 *  @author Bjarni Dagur Thor Kárason
 */
#pragma once

#include "dfa.hpp"
#include "variables.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

/// @brief Class to represent a single cell in a game board.
/// @author Bjarni Dagur Thor Kárason
class Cell
{
  public:
    /// @brief Cell constructor.
    Cell();
    /// @brief Cell constructor from cell state information.
    ///
    /// @param piece the name of the piece on this Cell.
    /// @param owners the list of players who own the \p piece on this Cell.
    /// @param state a DFA that generates legal moves for the \p piece on this Cell.
    Cell(std::string piece, std::vector<std::string> owners, DFAState *state);
    /// @brief Cell destructor.
    ~Cell();
    /// @brief The name of the piece on this Cell.
    std::string piece;
    /// @brief The list of players who own the \p piece on this Cell.
    std::vector<std::string> owners;
    /// @brief A DFA that generates legal moves for the \p piece on this Cell.
    DFAState *state;
};

/// @brief Class to represent a single step in a piece's move.
/// @details
///  A piece's move \f$s_0 s_1 \ldots s_n\f$ is composed of multiple Step where
///  \li \f$s_0\f$ is the piece's initial position,
///  \li \f$s_n\f$ is the piece's final position,
///  \li when the piece moves from \f$s_{i - 1}\f$ to \f$s_i\f$ the SideEffect
///   of Step \f$s_i\f$ is executed.
///
/// @author Bjarni Dagur Thor Kárason
class Step
{
  public:
    /// @brief Step constructor from move components.
    ///
    /// @param x the x coordinate of the destination Cell as an array index.
    /// @param y the y coordinate of the destination Cell as an array index.
    /// @param side_effect the SideEffect of the move.
    Step(int x, int y, std::shared_ptr<SideEffect> side_effect);
    /// @brief Step destructor.
    ~Step();
    /// @brief The x coordinate of the destination Cell as an array index.
    int x;
    /// @brief The y coordinate of the destination Cell as an array index.
    int y;
    /// @brief The SideEffect of the move.
    std::shared_ptr<SideEffect> side_effect;
};

/// @brief Manages the current state of a game.
/// @details
///  Keeps track of the current board and Variables, whose turn it is, generates
///  legal moves for the current player, and executes players' moves.
/// @author Bjarni Dagur Thor Kárason
class Environment
{
  public:
    /// @brief Environment constructor from game board size.
    Environment(int board_size_x, int board_size_y);
    /// @brief Environment desctructor.
    ~Environment();
    /// @brief The managed game board's size along the x axis of a Cartesian coordinate system.
    int board_size_x;
    /// @brief The managed game board's size along the y axis of a Cartesian coordinate system.
    int board_size_y;
    /// @brief Keeps track of how many moves have been made in the current game.
    int move_count;
    /// @brief Keeps track of the current game board.
    /// @details
    ///  First dimension of the vector corresponds to the x axis in a Cartesian
    ///  coordinate system, second dimension of the vector corresponds to the y
    ///  axis in a Cartesian coordinate system.
    ///
    /// @see Cell
    std::vector<std::vector<Cell>> board;
    /// @brief The pieces defined in the game description, which player they
    ///  belong to, and how they can move.
    /// @details
    ///  A map that takes a piece's name and returns a list of the players it
    ///  belongs to, and a state machine to generate legal moves for it.
    ///  A map that takes a piece's name and returns the name of the player it
    ///  belongs to, and a state machine to generate legal moves for it.
    std::map<std::string, std::pair<std::vector<std::string>, std::unique_ptr<DFAState, DFAStateDeleter>>> pieces;
    /// @brief The post conditions defined in the game description that must
    ///  hold after a player makes a move.
    /// @details
    ///  A map that takes a player's name and returns a vector containing all of
    ///  his post conditions as piece/state machine pairs.
    ///
    /// @note
    ///  Post conditions are defined for player/piece pairs. After the player
    ///  makes a move, all his post conditions are checked for each of the
    ///  pieces they are defined for. Post conditions are defined using regular
    ///  expressions, and a post condition holds if the corresponding regular
    ///  expression is not matched.
    std::map<std::string, std::vector<std::pair<std::string, std::unique_ptr<DFAState, DFAStateDeleter>>>>
        post_conditions;
    /// @brief The players defined in the game description.
    std::vector<std::string> players;
    /// @brief Keeps track of whose turn it is.
    std::string current_player;
    /// @brief Keeps track of the Variables in the current game state.
    Variables variables;
    /// @brief Checks if a Cell's coordinates are within the board's bounds.
    ///
    /// @param x the x coordinate of the Cell to check.
    /// @param y the y coordinate of the Cell to check.
    ///
    /// @returns true if Environment#board[\p x][\p y] exists.
    /// @returns false if Environment#board[\p x][\p y] does not exist.
    bool contains_cell(int x, int y);
    /// @brief Returns a 3-dimensional representation of the current environment
    ///  for a neural network.
    ///
    /// @returns A 3-dimensional representation of the current environment.
    std::vector<std::vector<std::vector<int>>> get_environment_representation();
    /// @brief Generates all legal moves for Environment#current_player.
    ///
    /// @returns a vector of legal moves.
    ///
    /// @see Step
    std::vector<std::vector<Step>> generate_moves();
    /// @brief Executes \p move in the current Environment state.
    /// @details
    ///  Automatically updates whose turn it is.
    ///
    /// @param move the move to execute.
    /// @param searching true if moves are being generated, false otherwise.
    ///
    /// @warning \p move can have SideEffect and TerminalCondition that change
    ///  Environment::Variables.
    ///
    ///  @pre \p move is a legal move in the current state.
    void execute_move(const std::vector<Step> &move, bool searching = false);
    /// @brief Undos the last move.
    /// @details
    ///  Sets the game state to the state before the move.
    ///
    /// @param searching true if moves are being generated, false otherwise.
    ///
    /// @pre There is a move to undo.
    void undo_move(bool searching = false);
    /// @brief Checks all defined TerminalCondition.
    ///
    /// @warning TerminalCondition can update Environment#variables if they are satisfied.
    bool check_terminal_conditions();
    /// @brief Returns the name of the starting player.
    std::string get_first_player();
    /// @brief Returns the name of the current player.
    std::string get_current_player();
    /// @brief Checks if the game is over.
    bool game_over();
    /// @brief Returns white's score in the current state.
    int get_white_score();
    /// @brief Resets the environment to its original state.
    void reset();
    /// @brief Prints the current game board state to standard out.
    void print();
    /// @brief Return a json representation of the envirnment for the GUI service.
    std::string jsonify();

  private:
    /// @brief Verifies that all post condition hold.
    ///
    /// @returns true if all post conditions holds.
    /// @returns false if some post condition does not hold.
    bool verify_post_conditions();
    /// @brief Verifies that a post condition holds.
    ///
    /// @param state the root of the post condition's DFA to check.
    /// @param x the x coordinate of the Cell to check the post condition from.
    /// @param y the y coordinate of the Cell to check the post condition from.
    ///
    /// @returns true if the post condition holds.
    /// @returns false if the post condition does not hold.
    bool verify_post_condition(DFAState *state, int x, int y);
    /// @brief Helper function for Environment::generate_moves.
    ///
    /// @param state the current state of a DFA that generates legal moves for a piece.
    /// @param x the current x coordinate of the piece.
    /// @param y the current y coordinate of the piece.
    void generate_moves(DFAState *state, int x, int y);
    /// @brief Updates whose turn it is.
    void update_current_player();
    /// @brief Stores found moves during move generation.
    std::vector<std::vector<Step>> found_moves;
    /// @brief Stores intermediate moves during move generation.
    std::vector<Step> candidate_move;
    /// @brief Stores the side effects executed in the environment in a reverse
    ///  order.
    std::stack<std::shared_ptr<SideEffect>> side_effect_stack;
    /// @brief Stores how many side effects were executed in each move.
    std::stack<int> side_effect_cnt;
};
