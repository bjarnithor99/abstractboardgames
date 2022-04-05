// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include "dfa.hpp"
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

class Cell
{
  public:
    Cell();
    Cell(int x, int y, std::string piece, std::string player, DFAState *state);
    ~Cell();
    int x;
    int y;
    std::string piece;
    std::string player;
    DFAState *state;
};

class Step
{
  public:
    Step(int x, int y, std::shared_ptr<SideEffect> side_effect);
    ~Step();
    int x;
    int y;
    std::shared_ptr<SideEffect> side_effect;
};

class Environment
{
  public:
    Environment(int board_size_x, int board_size_y);
    ~Environment();
    int board_size_x;
    int board_size_y;
    std::vector<std::vector<Cell>> board;
    std::map<std::string, std::pair<std::string, std::unique_ptr<DFAState, DFAStateDeleter>>> pieces;
    std::map<std::string, std::vector<std::pair<std::string, std::unique_ptr<DFAState, DFAStateDeleter>>>>
        post_conditions;
    std::string current_player;
    std::vector<std::vector<Step>> found_moves;
    bool contains_cell(int x, int y);
    int set_cell(int x, int y, Cell *cell);
    Cell *get_cell(int x, int y);
    void generate_moves(std::string player);
    bool execute_move(const std::vector<Step> &move);
    bool check_terminal_conditions();
    void print();

  private:
    bool verify_post_condition(DFAState *state, int x, int y);
    void generate_moves(DFAState *state, int x, int y);
    void prune_illegal_moves();
    std::vector<Step> candidate_move;
};
