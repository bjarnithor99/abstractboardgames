// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
// This include statement is a hack. The Predicate class needs to access member
// variables of the Environment class, so predicates.hpp includes
// environment.hpp. However, the Environment class also needs to access member
// functions of the Predicates class. To avoid circular includes, predicates.hpp
// includes Environment, dfa.hpp and nfa.hpp (which store pointers to Predicate)
// forward declare the Predicate class, and this compilation unit includes
// predicates.hpp, effectively including environment.hpp as well.
#include "predicates.hpp"
#include "side_effects.hpp"
#include "terminal_conditions.hpp"

#define COUTRESET "\033[0m"
#define COUTRED "\033[1m\033[31m"
#define COUTBLUE "\033[1m\033[34m"

Cell::Cell() {}
Cell::Cell(int x, int y, std::string piece, std::string player, DFAState *state)
    : x(x), y(y), piece(piece), player(player), state(state) {}
Cell::~Cell() {}

Step::Step(int x, int y, std::shared_ptr<SideEffect> side_effect) : x(x), y(y), side_effect(side_effect) {}
Step::~Step() {}

Environment::Environment(int board_size_x, int board_size_y)
    : board_size_x(board_size_x), board_size_y(board_size_y), move_count(0), variables(Variables()) {}
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

void Environment::generate_moves() {
    found_moves.clear();
    for (int i = 0; i < board_size_x; i++) {
        for (int j = 0; j < board_size_y; j++) {
            if (board[i][j].player == current_player) {
                candidate_move.clear();
                candidate_move.push_back(Step(i, j, SideEffects::get_side_effect["default"]));
                generate_moves(board[i][j].state, i, j);
            }
        }
    }
    prune_illegal_moves();
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
        if (!(*input.predicate)(this, next_x, next_y))
            continue;
        candidate_move.push_back(Step(next_x, next_y, input.side_effect));
        generate_moves(p.second, next_x, next_y);
        candidate_move.pop_back();
    }
}

bool Environment::verify_post_condition(DFAState *state, int x, int y) {
    if (state->is_accepting)
        return false;
    bool res = true;
    for (auto &p : state->transition) {
        const DFAInput &input = p.first;
        int next_x = x - input.dy;
        int next_y = y + input.dx;
        if (!contains_cell(next_x, next_y))
            continue;
        if (!(*input.predicate)(this, next_x, next_y))
            continue;
        res &= verify_post_condition(p.second, next_x, next_y);
    }
    return res;
}

void Environment::prune_illegal_moves() {
    std::vector<std::vector<Step>> legal_moves;
    for (const std::vector<Step> &move : found_moves) {
        execute_move(move);
        bool legal_move = true;
        for (auto &p : post_conditions[current_player]) {
            const std::string &piece = p.first;
            const std::unique_ptr<DFAState, DFAStateDeleter> &post_condition = p.second;
            for (int i = 0; i < board_size_x; i++) {
                for (int j = 0; j < board_size_y; j++) {
                    if (board[i][j].piece == piece) {
                        if (!verify_post_condition(post_condition.get(), i, j)) {
                            legal_move = false;
                        }
                    }
                }
            }
        }
        if (legal_move)
            legal_moves.push_back(move);
        undo_move();
    }
    found_moves = legal_moves;
}

bool Environment::execute_move(const std::vector<Step> &move) {
    move_stack.push({board, variables});
    move_count++;
    int n_steps = move.size();
    for (int i = 1; i < n_steps; i++) {
        int old_x = move[i - 1].x;
        int old_y = move[i - 1].y;
        int new_x = move[i].x;
        int new_y = move[i].y;
        (*(move[i].side_effect))(this, old_x, old_y, new_x, new_y);
    }
    bool game_over = check_terminal_conditions();
    update_current_player();
    return game_over;
}

void Environment::undo_move() {
    std::tie(board, variables) = move_stack.top();
    move_count--;
    move_stack.pop();
    update_current_player();
}

bool Environment::check_terminal_conditions() {
    bool game_over = false;
    for (auto &p : TerminalConditions::terminal_conditions) {
        const std::shared_ptr<TerminalCondition> &terminal_condition = p.second;
        if ((*terminal_condition)(this))
            game_over = true;
    }
    return game_over;
}

void Environment::update_current_player() {
    current_player = players[move_count % players.size()];
}

void Environment::print() {
    for (int i = 0; i < board_size_x; i++) {
        if (i != 0)
            std::cout << "\n";
        for (int j = 0; j < board_size_y; j++) {
            if (j != 0)
                std::cout << " ";
            if (board[i][j].player == "white")
                std::cout << COUTBLUE << std::setw(8) << board[i][j].piece << COUTRESET;
            else if (board[i][j].player == "black")
                std::cout << COUTRED << std::setw(8) << board[i][j].piece << COUTRESET;
            else
                std::cout << std::setw(8) << board[i][j].piece;
        }
        std::cout << "\n";
    }
    std::cout << std::flush;
}
