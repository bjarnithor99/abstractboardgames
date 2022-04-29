// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "predicates.hpp"

False::False() {}
False::~False() {}
bool False::operator()(Environment *environment, int x, int y) {
    return false;
}
std::string False::get_name() const {
    return "False";
}

Empty::Empty() {}
Empty::~Empty() {}
bool Empty::operator()(Environment *environment, int x, int y) {
    return environment->board[x][y].piece == "empty";
}
std::string Empty::get_name() const {
    return "Empty";
}

LowestUnoccupied::LowestUnoccupied() {}
LowestUnoccupied::~LowestUnoccupied() {}
bool LowestUnoccupied::operator()(Environment *environment, int x, int y) {
    return !environment->contains_cell(x + 1, y) || environment->board[x + 1][y].piece != "empty";
}
std::string LowestUnoccupied::get_name() const {
    return "LowestUnoccupied";
}

std::map<std::string, std::shared_ptr<Predicate>> Predicates::get_predicate = {
    {"False", std::make_shared<False>()},
    {"Empty", std::make_shared<Empty>()},
    {"LowestUnoccupied", std::make_shared<LowestUnoccupied>()},
};
