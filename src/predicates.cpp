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

std::map<std::string, std::shared_ptr<Predicate>> Predicates::get_predicate = {
    {"empty", std::make_shared<Empty>()},
    {"opponent", std::make_shared<Opponent>()},
};
