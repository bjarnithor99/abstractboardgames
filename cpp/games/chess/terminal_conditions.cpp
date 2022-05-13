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

    std::string own_king = environment->current_player == "black" ? "bKing" : "wKing";
    bool check_mate = false, done = false;
    for (size_t i = 0; !done && i < environment->board.size(); i++) {
        for (size_t j = 0; !done && j < environment->board[0].size(); j++) {
            if (environment->board[i][j].piece == own_king) {
                check_mate = !(*(Predicates::get_predicate["NotAttacked"]))(environment, i, j);
                done = true;
            }
        }
    }

    if (check_mate) {
        if (environment->current_player == "black") {
            environment->variables.black_score = -1;
            environment->variables.white_score = 1;
        }
        else {
            environment->variables.black_score = 1;
            environment->variables.white_score = -1;
        }
    }
    else {
        environment->variables.black_score = 0;
        environment->variables.white_score = 0;
    }
    environment->variables.game_over = true;
    return true;
}
std::string NoMovesLeft::get_name() const {
    return "NoMovesLeft";
}

Stagnation::Stagnation() {}
Stagnation::~Stagnation() {}
bool Stagnation::operator()(Environment *environment) {
    if (environment->move_count - environment->variables.stagnation >= 50) {
        environment->variables.black_score = 0;
        environment->variables.white_score = 0;
        environment->variables.game_over = true;
        return true;
    }
    return false;
}
std::string Stagnation::get_name() const {
    return "Stagnation";
}

std::map<std::string, std::shared_ptr<TerminalCondition>> TerminalConditions::terminal_conditions = {
    {"NoMovesLeft", std::make_shared<NoMovesLeft>()},
    {"Stagnation", std::make_shared<Stagnation>()},
};
