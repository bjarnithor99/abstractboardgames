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

FourConnected::FourConnected() {}
FourConnected::~FourConnected() {}
bool FourConnected::operator()(Environment *environment) {
    if (four_straight(environment) || four_vertical(environment)) {
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
bool FourConnected::four_straight(Environment *environment) {
    std::string players_piece = environment->current_player == "black" ? "bPawn" : "wPawn";

    for (size_t i = 0; i < environment->board.size(); i++) {
        int connected = 0;
        for (size_t j = 0; j < environment->board[0].size(); j++) {
            if (environment->board[i][j].piece == players_piece)
                connected++;
            else
                connected = 0;
            if (connected >= 4)
                return true;
        }
    }

    for (size_t i = 0; i < environment->board[0].size(); i++) {
        int connected = 0;
        for (size_t j = 0; j < environment->board.size(); j++) {
            if (environment->board[j][i].piece == players_piece)
                connected++;
            else
                connected = 0;
            if (connected >= 4)
                return true;
        }
    }

    return false;
}
bool FourConnected::four_vertical(Environment *environment) {
    if (environment->board.size() < 4 || environment->board[0].size() < 4)
        return false;

    std::string players_piece = environment->current_player == "black" ? "bPawn" : "wPawn";

    for (size_t i = 0; i < environment->board.size() - 3; i++) {
        for (size_t j = 0; j < environment->board[0].size() - 3; j++) {
            int connected = 0;
            for (int k = 0; k < 4; k++) {
                if (environment->board[i + k][j + k].piece == players_piece)
                    connected++;
                else
                    connected = 0;
                if (connected >= 4)
                    return true;
            }
        }
    }

    for (size_t i = 3; i < environment->board.size(); i++) {
        for (size_t j = 0; j < environment->board[0].size() - 3; j++) {
            int connected = 0;
            for (int k = 0; k < 4; k++) {
                if (environment->board[i - k][j + k].piece == players_piece)
                    connected++;
                else
                    connected = 0;
                if (connected >= 4)
                    return true;
            }
        }
    }

    return false;
}
std::string FourConnected::get_name() const {
    return "FourConnected";
}

std::map<std::string, std::shared_ptr<TerminalCondition>> TerminalConditions::terminal_conditions = {
    {"NoMovesLeft", std::make_shared<NoMovesLeft>()},
    {"FourConnected", std::make_shared<FourConnected>()},
};
