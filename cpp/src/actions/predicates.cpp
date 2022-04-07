// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "predicates.hpp"

Empty::Empty() {}
Empty::~Empty() {}
bool Empty::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "empty";
}
std::string Empty::get_name() const {
    return "empty";
}

Opponent::Opponent() {}
Opponent::~Opponent() {}
bool Opponent::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].player != "" && environment->board[x][y].player != environment->current_player;
}
std::string Opponent::get_name() const {
    return "opponent";
}

WhitePawn::WhitePawn() {}
WhitePawn::~WhitePawn() {}
bool WhitePawn::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "wPawn";
}
std::string WhitePawn::get_name() const {
    return "wpawn";
}

WhiteKnight::WhiteKnight() {}
WhiteKnight::~WhiteKnight() {}
bool WhiteKnight::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "wKnight";
}
std::string WhiteKnight::get_name() const {
    return "wknight";
}

WhiteBishop::WhiteBishop() {}
WhiteBishop::~WhiteBishop() {}
bool WhiteBishop::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "wBishop";
}
std::string WhiteBishop::get_name() const {
    return "wbishop";
}

WhiteRook::WhiteRook() {}
WhiteRook::~WhiteRook() {}
bool WhiteRook::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "wRook";
}
std::string WhiteRook::get_name() const {
    return "wrook";
}

WhiteQueen::WhiteQueen() {}
WhiteQueen::~WhiteQueen() {}
bool WhiteQueen::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "wQueen";
}
std::string WhiteQueen::get_name() const {
    return "wqueen";
}

WhiteKing::WhiteKing() {}
WhiteKing::~WhiteKing() {}
bool WhiteKing::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "wKing";
}
std::string WhiteKing::get_name() const {
    return "wking";
}

BlackPawn::BlackPawn() {}
BlackPawn::~BlackPawn() {}
bool BlackPawn::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "bPawn";
}
std::string BlackPawn::get_name() const {
    return "bpawn";
}

BlackKnight::BlackKnight() {}
BlackKnight::~BlackKnight() {}
bool BlackKnight::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "bKnight";
}
std::string BlackKnight::get_name() const {
    return "bknight";
}

BlackBishop::BlackBishop() {}
BlackBishop::~BlackBishop() {}
bool BlackBishop::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "bBishop";
}
std::string BlackBishop::get_name() const {
    return "bbishop";
}

BlackRook::BlackRook() {}
BlackRook::~BlackRook() {}
bool BlackRook::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "bRook";
}
std::string BlackRook::get_name() const {
    return "brook";
}

BlackQueen::BlackQueen() {}
BlackQueen::~BlackQueen() {}
bool BlackQueen::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "bQueen";
}
std::string BlackQueen::get_name() const {
    return "bqueen";
}

BlackKing::BlackKing() {}
BlackKing::~BlackKing() {}
bool BlackKing::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "bKing";
}
std::string BlackKing::get_name() const {
    return "bking";
}

std::map<std::string, std::shared_ptr<Predicate>> Predicates::get_predicate = {
    {"empty", std::make_shared<Empty>()},         {"opponent", std::make_shared<Opponent>()},
    {"wpawn", std::make_shared<WhitePawn>()},     {"wknight", std::make_shared<WhiteKnight>()},
    {"wbishop", std::make_shared<WhiteBishop>()}, {"wrook", std::make_shared<WhiteRook>()},
    {"wqueen", std::make_shared<WhiteQueen>()},   {"wking", std::make_shared<WhiteKing>()},
    {"bpawn", std::make_shared<BlackPawn>()},     {"bknight", std::make_shared<BlackKnight>()},
    {"bbishop", std::make_shared<BlackBishop>()}, {"brook", std::make_shared<BlackRook>()},
    {"bqueen", std::make_shared<BlackQueen>()},   {"bking", std::make_shared<BlackKing>()},
};
