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

    if (environment->current_player == "black") {
        environment->variables.black_score = -1;
        environment->variables.white_score = 1;
    }
    else {
        environment->variables.black_score = 1;
        environment->variables.white_score = -1;
    }
    environment->variables.game_over = true;
    return true;
}
std::string NoMovesLeft::get_name() const {
    return "NoMovesLeft";
}

BlackReachedEnd::BlackReachedEnd() {}
BlackReachedEnd::~BlackReachedEnd() {}
bool BlackReachedEnd::operator()(Environment *environment) {
    for (size_t i = 0; i < environment->board[0].size(); i++) {
        const std::vector<std::string> &owners = environment->board[environment->board.size() - 1][i].owners;
        if (!owners.empty() && owners[0] == "black") {
            environment->variables.black_score = 1;
            environment->variables.white_score = -1;
            environment->variables.game_over = true;
            return true;
        }
    }
    return false;
}
std::string BlackReachedEnd::get_name() const {
    return "BlackReachedEnd";
}

WhiteReachedEnd::WhiteReachedEnd() {}
WhiteReachedEnd::~WhiteReachedEnd() {}
bool WhiteReachedEnd::operator()(Environment *environment) {
    for (size_t i = 0; i < environment->board[0].size(); i++) {
        const std::vector<std::string> &owners = environment->board[0][i].owners;
        if (!owners.empty() && owners[0] == "white") {
            environment->variables.black_score = -1;
            environment->variables.white_score = 1;
            environment->variables.game_over = true;
            return true;
        }
    }
    return false;
}
std::string WhiteReachedEnd::get_name() const {
    return "WhiteReachedEnd";
}

std::map<std::string, std::shared_ptr<TerminalCondition>> TerminalConditions::terminal_conditions = {
    {"NoMovesLeft", std::make_shared<NoMovesLeft>()},
    {"BlackReachedEnd", std::make_shared<BlackReachedEnd>()},
    {"WhiteReachedEnd", std::make_shared<WhiteReachedEnd>()},
};
