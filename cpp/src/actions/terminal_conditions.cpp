// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "terminal_conditions.hpp"

NoMovesLeft::NoMovesLeft() {}
NoMovesLeft::~NoMovesLeft() {}
bool NoMovesLeft::operator()(Environment *environment) {
    if (environment->found_moves.empty()) {
        environment->variables.black_score = 50;
        environment->variables.white_score = 50;
        return true;
    }
    return false;
}
std::string NoMovesLeft::get_name() const {
    return "nomovesleft";
}

BlackReachedEnd::BlackReachedEnd() {}
BlackReachedEnd::~BlackReachedEnd() {}
bool BlackReachedEnd::operator()(Environment *environment) {
    for (int i = 0; i < 3; i++) {
        if (environment->board[3][i].player == "black") {
            environment->variables.black_score = 100;
            environment->variables.white_score = 0;
            return true;
        }
    }
    return false;
}
std::string BlackReachedEnd::get_name() const {
    return "blackreachedend";
}

WhiteReachedEnd::WhiteReachedEnd() {}
WhiteReachedEnd::~WhiteReachedEnd() {}
bool WhiteReachedEnd::operator()(Environment *environment) {
    for (int i = 0; i < 3; i++) {
        if (environment->board[0][i].player == "white") {
            environment->variables.black_score = 0;
            environment->variables.white_score = 100;
            return true;
        }
    }
    return false;
}
std::string WhiteReachedEnd::get_name() const {
    return "whitereachedend";
}

std::map<std::string, std::shared_ptr<TerminalCondition>> TerminalConditions::terminal_conditions = {
    {"nomovesleft", std::make_shared<NoMovesLeft>()},
    {"blackreachedend", std::make_shared<BlackReachedEnd>()},
    {"whitereachedend", std::make_shared<WhiteReachedEnd>()},
};
