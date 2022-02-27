#pragma once

#include "dfa.hpp"
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

class Environment
{
  public:
    Environment(int board_size_x, int board_size_y);
    ~Environment();
    int board_size_x;
    int board_size_y;
    std::vector<std::vector<Cell>> board;
    bool contains_cell(int x, int y);
    int set_cell(int x, int y, Cell *cell);
    Cell *get_cell(int x, int y);
    void print();
};
