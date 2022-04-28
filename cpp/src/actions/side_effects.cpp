// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "side_effects.hpp"

Default::Default() {}
Default::~Default() {}
void Default::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    if (old_x == new_x && old_y == new_y)
        return;
    Cell &old_cell = environment->board[old_x][old_y];
    Cell &new_cell = environment->board[new_x][new_y];
    new_cell.piece = old_cell.piece;
    new_cell.player = old_cell.player;
    new_cell.state = old_cell.state;
    old_cell.piece = "empty";
    old_cell.player = "";
    old_cell.state = nullptr;
}
std::string Default::get_name() const {
    return "Default";
}

PromoteToQueen::PromoteToQueen() {}
PromoteToQueen::~PromoteToQueen() {}
void PromoteToQueen::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    Cell &cell = environment->board[new_x][new_y];
    if (cell.player == "white") {
        cell.piece = "wQueen";
        cell.state = environment->pieces["wQueen"].second.get();
    }
    else {
        cell.piece = "bQueen";
        cell.state = environment->pieces["bQueen"].second.get();
    }
}
std::string PromoteToQueen::get_name() const {
    return "PromoteToQueen";
}

PromoteToRook::PromoteToRook() {}
PromoteToRook::~PromoteToRook() {}
void PromoteToRook::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    Cell &cell = environment->board[new_x][new_y];
    if (cell.player == "white") {
        cell.piece = "wRook";
        cell.state = environment->pieces["wRook"].second.get();
    }
    else {
        cell.piece = "bRook";
        cell.state = environment->pieces["bRook"].second.get();
    }
}
std::string PromoteToRook::get_name() const {
    return "PromoteToRook";
}

PromoteToBishop::PromoteToBishop() {}
PromoteToBishop::~PromoteToBishop() {}
void PromoteToBishop::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    Cell &cell = environment->board[new_x][new_y];
    if (cell.player == "white") {
        cell.piece = "wBishop";
        cell.state = environment->pieces["wBishop"].second.get();
    }
    else {
        cell.piece = "bBishop";
        cell.state = environment->pieces["bBishop"].second.get();
    }
}
std::string PromoteToBishop::get_name() const {
    return "PromoteToBishop";
}

PromoteToKnight::PromoteToKnight() {}
PromoteToKnight::~PromoteToKnight() {}
void PromoteToKnight::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    Cell &cell = environment->board[new_x][new_y];
    if (cell.player == "white") {
        cell.piece = "wKnight";
        cell.state = environment->pieces["wKnight"].second.get();
    }
    else {
        cell.piece = "bKnight";
        cell.state = environment->pieces["bKnight"].second.get();
    }
}
std::string PromoteToKnight::get_name() const {
    return "PromoteToKnight";
}

SetEnPassantable::SetEnPassantable() {}
SetEnPassantable::~SetEnPassantable() {}
void SetEnPassantable::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    environment->variables.en_passant_pawn = {environment->move_count, new_x, new_y};
}
std::string SetEnPassantable::get_name() const {
    return "SetEnPassantable";
}

CastleLeft::CastleLeft() {}
CastleLeft::~CastleLeft() {}
void CastleLeft::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    Cell &rook_src = environment->board[new_x][new_y - 2];
    Cell &rook_dst = environment->board[new_x][new_y + 1];
    rook_dst.piece = rook_src.piece;
    rook_dst.player = rook_src.player;
    rook_dst.state = rook_src.state;
    rook_src.piece = "empty";
    rook_src.player = "";
    rook_src.state = nullptr;
}
std::string CastleLeft::get_name() const {
    return "CastleLeft";
}

CastleRight::CastleRight() {}
CastleRight::~CastleRight() {}
void CastleRight::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    Cell &rook_src = environment->board[new_x][new_y + 1];
    Cell &rook_dst = environment->board[new_x][new_y - 1];
    rook_dst.piece = rook_src.piece;
    rook_dst.player = rook_src.player;
    rook_dst.state = rook_src.state;
    rook_src.piece = "empty";
    rook_src.player = "";
    rook_src.state = nullptr;
}
std::string CastleRight::get_name() const {
    return "CastleRight";
}

MarkMoved::MarkMoved() {}
MarkMoved::~MarkMoved() {}
void MarkMoved::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    if (environment->board[new_x][new_y].piece == "bKing") {
        environment->variables.black_king_moved = true;
    }
    else if (environment->board[new_x][new_y].piece == "wKing") {
        environment->variables.white_king_moved = true;
    }
    else if (environment->board[new_x][new_y].piece == "bRook") {
        if (environment->board[0][0].piece != "bRook")
            environment->variables.black_rook_left_moved = true;
        if (environment->board[0][7].piece != "bRook")
            environment->variables.black_rook_right_moved = true;
    }
    else if (environment->board[new_x][new_y].piece == "wRook") {
        if (environment->board[7][0].piece != "wRook")
            environment->variables.white_rook_left_moved = true;
        if (environment->board[7][7].piece != "wRook")
            environment->variables.white_rook_right_moved = true;
    }
}
std::string MarkMoved::get_name() const {
    return "MarkMoved";
}

std::map<std::string, std::shared_ptr<SideEffect>> SideEffects::get_side_effect = {
    {"Default", std::make_shared<Default>()},
    {"PromoteToQueen", std::make_shared<PromoteToQueen>()},
    {"PromoteToRook", std::make_shared<PromoteToRook>()},
    {"PromoteToBishop", std::make_shared<PromoteToBishop>()},
    {"PromoteToKnight", std::make_shared<PromoteToKnight>()},
    {"SetEnPassantable", std::make_shared<SetEnPassantable>()},
    {"CastleLeft", std::make_shared<CastleLeft>()},
    {"CastleRight", std::make_shared<CastleRight>()},
    {"MarkMoved", std::make_shared<MarkMoved>()},
};
