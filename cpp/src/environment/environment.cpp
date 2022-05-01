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
Cell::Cell(std::string piece, std::vector<std::string> owners, DFAState *state)
    : piece(piece), owners(owners), state(state) {}
Cell::~Cell() {}

Step::Step(int x, int y, std::shared_ptr<SideEffect> side_effect) : x(x), y(y), side_effect(side_effect) {}
Step::~Step() {}

Environment::Environment(int board_size_x, int board_size_y)
    : board_size_x(board_size_x), board_size_y(board_size_y), move_count(0), variables(Variables()) {}
Environment::~Environment() {}

bool Environment::contains_cell(int x, int y) {
    return 0 <= x && x < board_size_x && 0 <= y && y < board_size_y;
}

std::vector<std::vector<std::vector<int>>> Environment::get_environment_representation() {
    std::vector<std::vector<std::vector<int>>> representation;

    // One plane indicating presence of each piece type.
    for (auto &p : pieces) {
        std::string piece_name = p.first;
        std::vector<std::vector<int>> piece_bitmap(board.size(), std::vector<int>(board[0].size(), 0));
        for (size_t i = 0; i < board.size(); i++) {
            for (size_t j = 0; j < board[0].size(); j++) {
                if (board[i][j].piece == piece_name) {
                    piece_bitmap[i][j] = 1;
                }
            }
        }
        representation.push_back(piece_bitmap);
    }

    // One plane indicating whose turn it is. Assumes two players.
    std::vector<std::vector<int>> turn(board.size(), std::vector<int>(board[0].size(), current_player == players[0]));
    representation.push_back(turn);

    return representation;
}

std::vector<std::vector<Step>> Environment::generate_moves() {
    found_moves.clear();
    for (int i = 0; i < board_size_x; i++) {
        for (int j = 0; j < board_size_y; j++) {
            const std::vector<std::string> &owners = board[i][j].owners;
            if (std::find(owners.begin(), owners.end(), current_player) != owners.end()) {
                candidate_move.clear();
                candidate_move.push_back(Step(i, j, SideEffects::get_side_effect["Default"]));
                generate_moves(board[i][j].state, i, j);
            }
        }
    }
    return prune_illegal_moves();
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
        execute_move(
            std::vector<Step>{candidate_move[candidate_move.size() - 2], candidate_move[candidate_move.size() - 1]},
            true);
        generate_moves(p.second, next_x, next_y);
        candidate_move.pop_back();
        undo_move(true);
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

std::vector<std::vector<Step>> Environment::prune_illegal_moves() {
    std::vector<std::vector<Step>> legal_moves;
    for (const std::vector<Step> &move : found_moves) {
        execute_move(move, true);
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
            legal_moves.push_back(std::move(move));
        undo_move(true);
    }
    found_moves.clear();
    variables.n_moves_found = legal_moves.size();

    if (legal_moves.empty())
        check_terminal_conditions();

    return legal_moves;
}

void Environment::execute_move(const std::vector<Step> &move, bool searching) {
    move_stack.push({board, variables});
    int n_steps = move.size();
    for (int i = 1; i < n_steps; i++) {
        int old_x = move[i - 1].x;
        int old_y = move[i - 1].y;
        int new_x = move[i].x;
        int new_y = move[i].y;
        (*(move[i].side_effect))(this, old_x, old_y, new_x, new_y);
    }
    if (!searching) {
        move_count++;
        check_terminal_conditions();
        update_current_player();
    }
}

void Environment::undo_move(bool searching) {
    std::tie(board, variables) = move_stack.top();
    move_stack.pop();
    if (!searching) {
        move_count--;
        update_current_player();
    }
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

std::string Environment::get_first_player() {
    return players[0];
}

std::string Environment::get_current_player() {
    return current_player;
}

bool Environment::game_over() {
    return variables.game_over;
}

int Environment::get_white_score() {
    return variables.white_score;
}

void Environment::reset() {
    if (move_stack.empty())
        return;
    while (move_stack.size() != 1)
        move_stack.pop();
    move_count = 1;
    undo_move();
}

void Environment::print() {
    for (size_t i = 0; i < board.size(); i++) {
        if (i != 0)
            std::cout << "\n";
        for (size_t j = 0; j < board[0].size(); j++) {
            if (j != 0)
                std::cout << " ";
            if (board[i][j].owners.size() == 1 && board[i][j].owners[0] == "white")
                std::cout << COUTBLUE << std::setw(8) << board[i][j].piece << COUTRESET;
            else if (board[i][j].owners.size() == 1 && board[i][j].owners[0] == "black")
                std::cout << COUTRED << std::setw(8) << board[i][j].piece << COUTRESET;
            else
                std::cout << std::setw(8) << board[i][j].piece;
        }
        std::cout << "\n";
    }
    std::cout << std::flush;
}

std::string Environment::jsonify() {
    std::string json = "{\"board\": [";
    for (size_t i = 0; i < board[0].size(); i++) {
        if (i != 0)
            json += ", ";
        json += "[";
        for (size_t j = 0; j < board.size(); j++) {
            if (j != 0)
                json += ", ";
            json += "\"" + board[board.size() - 1 - j][i].piece + "\"";
        }
        json += "]";
    }
    json += "], \"moves\": [";
    std::vector<std::vector<Step>> moves = generate_moves();
    bool need_move_separator = false;
    for (const std::vector<Step> &move : moves) {
        if (need_move_separator)
            json += ", ";
        json += "{\"start\": [";
        int x = move[0].x, y = move[0].y;
        json += std::to_string(y) + ", " + std::to_string(board.size() - 1 - x);
        json += "], \"letters\": [";
        bool need_step_separator = false;
        for (size_t i = 1; i < move.size(); i++) {
            if (need_step_separator)
                json += ", ";
            int dx = move[i].y - y, dy = -(move[i].x - x);
            x = move[i].x, y = move[i].y;
            json += "{\"dx\": " + std::to_string(dx) + ", \"dy\": " + std::to_string(dy) + ", \"effect\": \"" +
                    move[i].side_effect->get_name() + "\"}";
            need_step_separator = true;
        }
        json += "]";
        json += "}";
        need_move_separator = true;
    }
    json += "]";
    json += "}";
    return json;
}
