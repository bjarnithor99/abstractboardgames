// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "side_effects.hpp"

Default::Default() {}
Default::~Default() {}
void Default::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    Cell &old_cell = environment->board[old_x][old_y];
    Cell &new_cell = environment->board[new_x][new_y];
    cell_stack.push({old_cell, old_x, old_y});
    cell_stack.push({new_cell, new_x, new_y});

    if (old_x == new_x && old_y == new_y)
        return;

    new_cell.piece = old_cell.piece;
    new_cell.owners = old_cell.owners;
    new_cell.state = old_cell.state;
    old_cell.piece = "empty";
    old_cell.owners = std::vector<std::string>();
    old_cell.state = nullptr;
}
void Default::operator()(Environment *environment) {
    int x, y;
    for (int i = 0; i < 2; i++) {
        x = std::get<1>(cell_stack.top());
        y = std::get<2>(cell_stack.top());
        environment->board[x][y] = std::get<0>(cell_stack.top());
        cell_stack.pop();
    }
}
std::string Default::get_name() const {
    return "Default";
}

PromoteToQueen::PromoteToQueen() {}
PromoteToQueen::~PromoteToQueen() {}
void PromoteToQueen::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    Cell &cell = environment->board[new_x][new_y];
    cell_stack.push({cell, new_x, new_y});
    if (!cell.owners.empty() && cell.owners[0] == "white") {
        cell.piece = "wQueen";
        cell.state = environment->pieces["wQueen"].second.get();
    }
    else {
        cell.piece = "bQueen";
        cell.state = environment->pieces["bQueen"].second.get();
    }
}
void PromoteToQueen::operator()(Environment *environment) {
    int x = std::get<1>(cell_stack.top());
    int y = std::get<2>(cell_stack.top());
    environment->board[x][y] = std::get<0>(cell_stack.top());
    cell_stack.pop();
}
std::string PromoteToQueen::get_name() const {
    return "PromoteToQueen";
}

PromoteToRook::PromoteToRook() {}
PromoteToRook::~PromoteToRook() {}
void PromoteToRook::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    Cell &cell = environment->board[new_x][new_y];
    if (!cell.owners.empty() && cell.owners[0] == "white") {
        cell.piece = "wRook";
        cell.state = environment->pieces["wRook"].second.get();
    }
    else {
        cell.piece = "bRook";
        cell.state = environment->pieces["bRook"].second.get();
    }
}
void PromoteToRook::operator()(Environment *environment) {
    int x = std::get<1>(cell_stack.top());
    int y = std::get<2>(cell_stack.top());
    environment->board[x][y] = std::get<0>(cell_stack.top());
    cell_stack.pop();
}
std::string PromoteToRook::get_name() const {
    return "PromoteToRook";
}

PromoteToBishop::PromoteToBishop() {}
PromoteToBishop::~PromoteToBishop() {}
void PromoteToBishop::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    Cell &cell = environment->board[new_x][new_y];
    if (!cell.owners.empty() && cell.owners[0] == "white") {
        cell.piece = "wBishop";
        cell.state = environment->pieces["wBishop"].second.get();
    }
    else {
        cell.piece = "bBishop";
        cell.state = environment->pieces["bBishop"].second.get();
    }
}
void PromoteToBishop::operator()(Environment *environment) {
    int x = std::get<1>(cell_stack.top());
    int y = std::get<2>(cell_stack.top());
    environment->board[x][y] = std::get<0>(cell_stack.top());
    cell_stack.pop();
}
std::string PromoteToBishop::get_name() const {
    return "PromoteToBishop";
}

PromoteToKnight::PromoteToKnight() {}
PromoteToKnight::~PromoteToKnight() {}
void PromoteToKnight::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    Cell &cell = environment->board[new_x][new_y];
    if (!cell.owners.empty() && cell.owners[0] == "white") {
        cell.piece = "wKnight";
        cell.state = environment->pieces["wKnight"].second.get();
    }
    else {
        cell.piece = "bKnight";
        cell.state = environment->pieces["bKnight"].second.get();
    }
}
void PromoteToKnight::operator()(Environment *environment) {
    int x = std::get<1>(cell_stack.top());
    int y = std::get<2>(cell_stack.top());
    environment->board[x][y] = std::get<0>(cell_stack.top());
    cell_stack.pop();
}
std::string PromoteToKnight::get_name() const {
    return "PromoteToKnight";
}

SetEnPassantable::SetEnPassantable() {}
SetEnPassantable::~SetEnPassantable() {}
void SetEnPassantable::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    en_passant_stack.push(environment->variables.en_passant_pawn);
    environment->variables.en_passant_pawn = {environment->move_count, new_x, new_y};
}
void SetEnPassantable::operator()(Environment *environment) {
    environment->variables.en_passant_pawn = en_passant_stack.top();
    en_passant_stack.pop();
}
std::string SetEnPassantable::get_name() const {
    return "SetEnPassantable";
}

CastleLeft::CastleLeft() {}
CastleLeft::~CastleLeft() {}
void CastleLeft::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    Cell &rook_src = environment->board[new_x][new_y - 2];
    Cell &rook_dst = environment->board[new_x][new_y + 1];
    cell_stack.push({rook_src, new_x, new_y - 2});
    cell_stack.push({rook_dst, new_x, new_y + 1});

    rook_dst.piece = rook_src.piece;
    rook_dst.owners = rook_src.owners;
    rook_dst.state = rook_src.state;
    rook_src.piece = "empty";
    rook_src.owners = std::vector<std::string>();
    rook_src.state = nullptr;
}
void CastleLeft::operator()(Environment *environment) {
    int x, y;
    for (int i = 0; i < 2; i++) {
        x = std::get<1>(cell_stack.top());
        y = std::get<2>(cell_stack.top());
        environment->board[x][y] = std::get<0>(cell_stack.top());
        cell_stack.pop();
    }
}
std::string CastleLeft::get_name() const {
    return "CastleLeft";
}

CastleRight::CastleRight() {}
CastleRight::~CastleRight() {}
void CastleRight::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    Cell &rook_src = environment->board[new_x][new_y + 1];
    Cell &rook_dst = environment->board[new_x][new_y - 1];
    cell_stack.push({rook_src, new_x, new_y + 1});
    cell_stack.push({rook_dst, new_x, new_y - 1});

    rook_dst.piece = rook_src.piece;
    rook_dst.owners = rook_src.owners;
    rook_dst.state = rook_src.state;
    rook_src.piece = "empty";
    rook_src.owners = std::vector<std::string>();
    rook_src.state = nullptr;
}
void CastleRight::operator()(Environment *environment) {
    int x, y;
    for (int i = 0; i < 2; i++) {
        x = std::get<1>(cell_stack.top());
        y = std::get<2>(cell_stack.top());
        environment->board[x][y] = std::get<0>(cell_stack.top());
        cell_stack.pop();
    }
}
std::string CastleRight::get_name() const {
    return "CastleRight";
}

MarkMoved::MarkMoved() {}
MarkMoved::~MarkMoved() {}
void MarkMoved::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    bool *var_ptr, val;
    if (environment->board[new_x][new_y].piece == "bKing") {
        var_ptr = &environment->variables.black_king_moved;
        val = environment->variables.black_king_moved;
        environment->variables.black_king_moved = true;
    }
    else if (environment->board[new_x][new_y].piece == "wKing") {
        var_ptr = &environment->variables.white_king_moved;
        val = environment->variables.white_king_moved;
        environment->variables.white_king_moved = true;
    }
    else if (environment->board[new_x][new_y].piece == "bRook") {
        if (environment->board[0][0].piece != "bRook") {
            var_ptr = &environment->variables.black_rook_left_moved;
            val = environment->variables.black_rook_left_moved;
            environment->variables.black_rook_left_moved = true;
        }
        if (environment->board[0][7].piece != "bRook") {
            var_ptr = &environment->variables.black_rook_right_moved;
            val = environment->variables.black_rook_right_moved;
            environment->variables.black_rook_right_moved = true;
        }
    }
    else if (environment->board[new_x][new_y].piece == "wRook") {
        if (environment->board[7][0].piece != "wRook") {
            var_ptr = &environment->variables.white_rook_left_moved;
            val = environment->variables.white_rook_left_moved;
            environment->variables.white_rook_left_moved = true;
        }
        if (environment->board[7][7].piece != "wRook") {
            var_ptr = &environment->variables.white_rook_right_moved;
            val = environment->variables.white_rook_right_moved;
            environment->variables.white_rook_right_moved = true;
        }
    }
    moved_stack.push({var_ptr, val});
}
void MarkMoved::operator()(Environment *environment) {
    bool *var_ptr, val;
    std::tie(var_ptr, val) = moved_stack.top();
    moved_stack.pop();
    *var_ptr = val;
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
