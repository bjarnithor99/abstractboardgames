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
    Step(int x, int y, std::string side_effect);
    ~Step();
    int x;
    int y;
    std::string side_effect;
};

class Environment
{
  public:
    Environment(int board_size_x, int board_size_y);
    ~Environment();
    int board_size_x;
    int board_size_y;
    std::vector<std::vector<Cell>> board;
    std::vector<std::vector<Step>> found_moves;
    bool contains_cell(int x, int y);
    int set_cell(int x, int y, Cell *cell);
    Cell *get_cell(int x, int y);
    void generate_moves(std::string player);
    void print();

  private:
    std::string current_player;
    void generate_moves(DFAState *state, int x, int y);
    std::vector<Step> candidate_move;
    bool verify_predicate(std::string predicate, int x, int y);
};
