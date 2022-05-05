// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "predicates.hpp"

True::True() {}
True::~True() {}
bool True::operator()(Environment *environment, int x, int y) {
    return true;
}
std::string True::get_name() const {
    return "True";
}

Empty::Empty() {}
Empty::~Empty() {}
bool Empty::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].owners.empty();
}
std::string Empty::get_name() const {
    return "Empty";
}

Opponent::Opponent() {}
Opponent::~Opponent() {}
bool Opponent::operator()(Environment *environment, int x, int y) {
    return !environment->board[x][y].owners.empty() &&
           environment->board[x][y].owners[0] != environment->current_player;
}
std::string Opponent::get_name() const {
    return "Opponent";
}

WhitePawn::WhitePawn() {}
WhitePawn::~WhitePawn() {}
bool WhitePawn::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "wPawn";
}
std::string WhitePawn::get_name() const {
    return "WPawn";
}

WhiteKnight::WhiteKnight() {}
WhiteKnight::~WhiteKnight() {}
bool WhiteKnight::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "wKnight";
}
std::string WhiteKnight::get_name() const {
    return "WKnight";
}

WhiteBishop::WhiteBishop() {}
WhiteBishop::~WhiteBishop() {}
bool WhiteBishop::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "wBishop";
}
std::string WhiteBishop::get_name() const {
    return "WBishop";
}

WhiteRook::WhiteRook() {}
WhiteRook::~WhiteRook() {}
bool WhiteRook::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "wRook";
}
std::string WhiteRook::get_name() const {
    return "WRook";
}

WhiteQueen::WhiteQueen() {}
WhiteQueen::~WhiteQueen() {}
bool WhiteQueen::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "wQueen";
}
std::string WhiteQueen::get_name() const {
    return "WQueen";
}

WhiteKing::WhiteKing() {}
WhiteKing::~WhiteKing() {}
bool WhiteKing::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "wKing";
}
std::string WhiteKing::get_name() const {
    return "WKing";
}

BlackPawn::BlackPawn() {}
BlackPawn::~BlackPawn() {}
bool BlackPawn::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "bPawn";
}
std::string BlackPawn::get_name() const {
    return "BPawn";
}

BlackKnight::BlackKnight() {}
BlackKnight::~BlackKnight() {}
bool BlackKnight::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "bKnight";
}
std::string BlackKnight::get_name() const {
    return "BKnight";
}

BlackBishop::BlackBishop() {}
BlackBishop::~BlackBishop() {}
bool BlackBishop::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "bBishop";
}
std::string BlackBishop::get_name() const {
    return "BBishop";
}

BlackRook::BlackRook() {}
BlackRook::~BlackRook() {}
bool BlackRook::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "bRook";
}
std::string BlackRook::get_name() const {
    return "BRook";
}

BlackQueen::BlackQueen() {}
BlackQueen::~BlackQueen() {}
bool BlackQueen::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "bQueen";
}
std::string BlackQueen::get_name() const {
    return "BQueen";
}

BlackKing::BlackKing() {}
BlackKing::~BlackKing() {}
bool BlackKing::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "bKing";
}
std::string BlackKing::get_name() const {
    return "BKing";
}

PawnInitialRow::PawnInitialRow() {}
PawnInitialRow::~PawnInitialRow() {}
bool PawnInitialRow::operator()(Environment *environment, int x, int y) {
    return (environment->board[x][y].piece == "bPawn" && x == 1) ||
           (environment->board[x][y].piece == "wPawn" && x == 6);
}
std::string PawnInitialRow::get_name() const {
    return "PawnInitialRow";
}

FinalRow::FinalRow() {}
FinalRow::~FinalRow() {}
bool FinalRow::operator()(Environment *environment, int x, int y) {
    return (!environment->board[x][y].owners.empty() && environment->board[x][y].owners[0] == "black" && x == 7) ||
           (!environment->board[x][y].owners.empty() && environment->board[x][y].owners[0] == "white" && x == 0);
}
std::string FinalRow::get_name() const {
    return "FinalRow";
}

NotFinalRow::NotFinalRow() {}
NotFinalRow::~NotFinalRow() {}
bool NotFinalRow::operator()(Environment *environment, int x, int y) {
    return !((!environment->board[x][y].owners.empty() && environment->board[x][y].owners[0] == "black" && x == 7) ||
             (!environment->board[x][y].owners.empty() && environment->board[x][y].owners[0] == "white" && x == 0));
}
std::string NotFinalRow::get_name() const {
    return "NotFinalRow";
}

EnPassantable::EnPassantable() {}
EnPassantable::~EnPassantable() {}
bool EnPassantable::operator()(Environment *environment, int x, int y) {
    int en_passant_move_number, en_passant_x, en_passant_y;
    std::tie(en_passant_move_number, en_passant_x, en_passant_y) = environment->variables.en_passant_pawn;
    return en_passant_move_number + 1 == environment->move_count && en_passant_x == x && en_passant_y == y;
}
std::string EnPassantable::get_name() const {
    return "EnPassantable";
}

RightToCastleRight::RightToCastleRight() {}
RightToCastleRight::~RightToCastleRight() {}
bool RightToCastleRight::operator()(Environment *environment, int x, int y) {
    if (environment->current_player == "black")
        return !environment->variables.black_king_moved && !environment->variables.black_rook_right_moved;
    else
        return !environment->variables.white_king_moved && !environment->variables.white_rook_right_moved;
}
std::string RightToCastleRight::get_name() const {
    return "RightToCastleRight";
}

RightToCastleLeft::RightToCastleLeft() {}
RightToCastleLeft::~RightToCastleLeft() {}
bool RightToCastleLeft::operator()(Environment *environment, int x, int y) {
    if (environment->current_player == "black")
        return !environment->variables.black_king_moved && !environment->variables.black_rook_left_moved;
    else
        return !environment->variables.white_king_moved && !environment->variables.white_rook_left_moved;
}
std::string RightToCastleLeft::get_name() const {
    return "RightToCastleLeft";
}

NotAttacked::NotAttacked() {}
NotAttacked::~NotAttacked() {}
bool NotAttacked::operator()(Environment *environment, int x, int y) {
    return !attacked_by_pawn(environment, x, y) && !attacked_by_knight(environment, x, y) &&
           !attacked_diagonally(environment, x, y) && !attacked_straight(environment, x, y);
}
bool NotAttacked::attacked_by_pawn(Environment *environment, int x, int y) {
    if (environment->current_player == "black") {
        bool attacked = false;
        attacked |= (environment->contains_cell(x + 1, y - 1) && environment->board[x + 1][y - 1].piece == "wPawn");
        attacked |= (environment->contains_cell(x + 1, y + 1) && environment->board[x + 1][y + 1].piece == "wPawn");
        return attacked;
    }
    else {
        bool attacked = false;
        attacked |= (environment->contains_cell(x - 1, y - 1) && environment->board[x - 1][y - 1].piece == "bPawn");
        attacked |= (environment->contains_cell(x - 1, y + 1) && environment->board[x - 1][y + 1].piece == "bPawn");
        return attacked;
    }
}
bool NotAttacked::attacked_by_knight(Environment *environment, int x, int y) {
    std::string opponent_knight = environment->current_player == "black" ? "wKnight" : "bKnight";
    bool attacked = false;
    attacked |= (environment->contains_cell(x + 1, y + 2) && environment->board[x + 1][y + 2].piece == opponent_knight);
    attacked |= (environment->contains_cell(x + 1, y - 2) && environment->board[x + 1][y - 2].piece == opponent_knight);
    attacked |= (environment->contains_cell(x - 1, y + 2) && environment->board[x - 1][y + 2].piece == opponent_knight);
    attacked |= (environment->contains_cell(x - 1, y - 2) && environment->board[x - 1][y - 2].piece == opponent_knight);
    attacked |= (environment->contains_cell(x + 2, y + 1) && environment->board[x + 2][y + 1].piece == opponent_knight);
    attacked |= (environment->contains_cell(x + 2, y - 1) && environment->board[x + 2][y - 1].piece == opponent_knight);
    attacked |= (environment->contains_cell(x - 2, y + 1) && environment->board[x - 2][y + 1].piece == opponent_knight);
    attacked |= (environment->contains_cell(x - 2, y - 1) && environment->board[x - 2][y - 1].piece == opponent_knight);
    return attacked;
}
bool NotAttacked::attacked_diagonally(Environment *environment, int x, int y) {
    std::string opponent_bishop = environment->current_player == "black" ? "wBishop" : "bBishop";
    std::string opponent_queen = environment->current_player == "black" ? "wQueen" : "bQueen";
    std::vector<std::string> opponents{opponent_bishop, opponent_queen};
    return attacked_coef(environment, x, y, 1, 1, opponents) || attacked_coef(environment, x, y, 1, -1, opponents) ||
           attacked_coef(environment, x, y, -1, 1, opponents) || attacked_coef(environment, x, y, -1, -1, opponents);
}
bool NotAttacked::attacked_straight(Environment *environment, int x, int y) {
    std::string opponent_rook = environment->current_player == "black" ? "wRook" : "bRook";
    std::string opponent_queen = environment->current_player == "black" ? "wQueen" : "bQueen";
    std::vector<std::string> opponents{opponent_rook, opponent_queen};
    return attacked_coef(environment, x, y, 1, 0, opponents) || attacked_coef(environment, x, y, -1, 0, opponents) ||
           attacked_coef(environment, x, y, 0, 1, opponents) || attacked_coef(environment, x, y, 0, -1, opponents);
}
bool NotAttacked::attacked_coef(Environment *environment, int x, int y, int x_coef, int y_coef,
                                const std::vector<std::string> &opponents) {
    for (int i = 1; i < 8; i++) {
        if (!environment->contains_cell(x + x_coef * i, y + x_coef * i))
            break;
        for (const std::string &opponent : opponents) {
            if (environment->board[x + x_coef * i][y + y_coef * i].piece == opponent)
                return true;
        }
        if (!environment->board[x + x_coef * i][y + y_coef * i].owners.empty())
            break;
    }
    return false;
}
std::string NotAttacked::get_name() const {
    return "NotAttacked";
}

std::map<std::string, std::shared_ptr<Predicate>> Predicates::get_predicate = {
    {"True", std::make_shared<True>()},
    {"Empty", std::make_shared<Empty>()},
    {"Opponent", std::make_shared<Opponent>()},
    {"WPawn", std::make_shared<WhitePawn>()},
    {"WKnight", std::make_shared<WhiteKnight>()},
    {"WBishop", std::make_shared<WhiteBishop>()},
    {"WRook", std::make_shared<WhiteRook>()},
    {"WQueen", std::make_shared<WhiteQueen>()},
    {"WKing", std::make_shared<WhiteKing>()},
    {"BPawn", std::make_shared<BlackPawn>()},
    {"BKnight", std::make_shared<BlackKnight>()},
    {"BBishop", std::make_shared<BlackBishop>()},
    {"BRook", std::make_shared<BlackRook>()},
    {"BQueen", std::make_shared<BlackQueen>()},
    {"BKing", std::make_shared<BlackKing>()},
    {"PawnInitialRow", std::make_shared<PawnInitialRow>()},
    {"FinalRow", std::make_shared<FinalRow>()},
    {"NotFinalRow", std::make_shared<NotFinalRow>()},
    {"EnPassantable", std::make_shared<EnPassantable>()},
    {"RightToCastleLeft", std::make_shared<RightToCastleLeft>()},
    {"RightToCastleRight", std::make_shared<RightToCastleRight>()},
    {"NotAttacked", std::make_shared<NotAttacked>()},
};
