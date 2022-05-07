// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "terminal_conditions.hpp"

NoMovesLeft::NoMovesLeft() {}
NoMovesLeft::~NoMovesLeft() {}
bool NoMovesLeft::operator()(Environment *environment) {
    if (environment->variables.n_moves_found != 0)
        return false;

    environment->variables.black_score = 0;
    environment->variables.white_score = 0;
    environment->variables.game_over = true;
    return true;
}
std::string NoMovesLeft::get_name() const {
    return "NoMovesLeft";
}

ThreeConnected::ThreeConnected() {}
ThreeConnected::~ThreeConnected() {}
bool ThreeConnected::operator()(Environment *environment) {
    bool connected = false;
    for (int i = 0; i < 3; i++) {
        connected |= check_line(environment, 0, i, 1, 0);
        connected |= check_line(environment, i, 0, 0, 1);
    }
    connected |= check_line(environment, 0, 0, 1, 1);
    connected |= check_line(environment, 0, 2, 1, -1);
    if (connected) {
        if (environment->current_player == "black") {
            environment->variables.black_score = 1;
            environment->variables.white_score = -1;
        }
        else {
            environment->variables.black_score = -1;
            environment->variables.white_score = 1;
        }
        environment->variables.game_over = true;
        return true;
    }
    return false;
}
bool ThreeConnected::check_line(Environment *environment, int x, int y, int x_coef, int y_coef) {
    if (environment->board[x][y].piece == "empty")
        return false;

    for (int i = 1; i <= 2; i++) {
        if (environment->board[x + (i - 1) * x_coef][y + (i - 1) * y_coef].piece !=
            environment->board[x + i * x_coef][y + i * y_coef].piece) {
            return false;
        }
    }
    return true;
}
std::string ThreeConnected::get_name() const {
    return "ThreeConnected";
}

std::map<std::string, std::shared_ptr<TerminalCondition>> TerminalConditions::terminal_conditions = {
    {"NoMovesLeft", std::make_shared<NoMovesLeft>()},
    {"ThreeConnected", std::make_shared<ThreeConnected>()},
};
