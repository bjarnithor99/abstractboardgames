#include "environment.hpp"

Cell::Cell() {}
Cell::Cell(int x, int y, std::string piece, std::string player, DFAState *state)
    : x(x), y(y), piece(piece), player(player), state(state) {}
Cell::~Cell() {}

Environment::Environment(int board_size_x, int board_size_y) : board_size_x(board_size_x), board_size_y(board_size_y) {}
Environment::~Environment() {}

bool Environment::contains_cell(int x, int y) {
    return 0 <= x && x <= board_size_x && 0 <= y && y <= board_size_y;
}

int Environment::set_cell(int x, int y, Cell *cell) {
    if (!contains_cell(x, y))
        return -1;
    board[x][y].x = cell->x;
    board[x][y].y = cell->y;
    board[x][y].piece = cell->piece;
    board[x][y].player = cell->player;
    board[x][y].state = cell->state;
    return 0;
}

Cell *Environment::get_cell(int x, int y) {
    if (!contains_cell(x, y))
        return nullptr;
    return &board[x][y];
}

void Environment::print() {
    for (int i = 0; i < board_size_x; i++) {
        if (i != 0)
            std::cout << "\n";
        for (int j = 0; j < board_size_y; j++) {
            if (j != 0)
                std::cout << " ";
            std::cout << board[i][j].piece;
        }
        std::cout << "\n";
    }
    std::cout << std::flush;
}
