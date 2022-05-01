// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "predicates.hpp"

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

std::map<std::string, std::shared_ptr<Predicate>> Predicates::get_predicate = {
    {"Empty", std::make_shared<Empty>()},
    {"Opponent", std::make_shared<Opponent>()},
};
