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

std::map<std::string, std::shared_ptr<SideEffect>> SideEffects::get_side_effect = {
    {"Default", std::make_shared<Default>()},
};
