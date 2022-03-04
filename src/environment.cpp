#include "environment.hpp"

#define COUTRESET "\033[0m"
#define COUTBLACK "\033[1m\033[30m"
#define COUTBLUE "\033[1m\033[34m"

Cell::Cell() {}
Cell::Cell(int x, int y, std::string piece, std::string player, DFAState *state)
    : x(x), y(y), piece(piece), player(player), state(state) {}
Cell::~Cell() {}

Environment::Environment(int board_size_x, int board_size_y) : board_size_x(board_size_x), board_size_y(board_size_y) {}
Environment::~Environment() {}

bool Environment::contains_cell(int x, int y) {
    return 0 <= x && x < board_size_x && 0 <= y && y < board_size_y;
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

void Environment::generate_moves(std::string player) {
    current_player = player;
    found_moves.clear();
    for (int i = 0; i < board_size_x; i++) {
        for (int j = 0; j < board_size_y; j++) {
            if (board[i][j].player == player) {
                candidate_move.clear();
                candidate_move.push_back({i, j});
                generate_moves(board[i][j].state, i, j);
            }
        }
    }
}

void Environment::generate_moves(DFAState *state, int x, int y) {
    if (state->is_accepting)
        found_moves.push_back(candidate_move);
    for (auto &p : state->transition) {
        const DFAInput &input = p.first;
        int next_x = x - input.dy;
        int next_y = y + input.dx;
        if (!contains_cell(next_x, next_y))
            continue;
        if (!verify_predicate(input.predicate, next_x, next_y))
            continue;
        candidate_move.push_back({next_x, next_y});
        generate_moves(p.second, next_x, next_y);
        candidate_move.pop_back();
    }
}

bool Environment::verify_predicate(std::string predicate, int x, int y) {
    if (predicate == "empty")
        return board[x][y].piece == "empty";
    else if (predicate == "opponent")
        return board[x][y].player != "" && board[x][y].player != current_player;
    else {
        std::cout << "Unknown predicate " << predicate << std::endl;
        return false;
    }
}

void Environment::print() {
    for (int i = 0; i < board_size_x; i++) {
        if (i != 0)
            std::cout << "\n";
        for (int j = 0; j < board_size_y; j++) {
            if (j != 0)
                std::cout << " ";
            if (board[i][j].player == "white")
                std::cout << COUTBLUE << board[i][j].piece << COUTRESET;
            else if (board[i][j].player == "black")
                std::cout << COUTBLACK << board[i][j].piece << COUTRESET;
            else
                std::cout << board[i][j].piece;
        }
        std::cout << "\n";
    }
    std::cout << std::flush;
}
