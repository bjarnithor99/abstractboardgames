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

True::True() {}
True::~True() {}
bool True::operator()(Environment *environment, int x, int y) {
    return true;
}
std::string True::get_name() const {
    return "True";
}

std::map<std::string, std::shared_ptr<Predicate>> Predicates::get_predicate = {
    {"False", std::make_shared<False>()},
    {"True", std::make_shared<True>()},
};
