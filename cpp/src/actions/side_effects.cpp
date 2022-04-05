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
    new_cell.piece = old_cell.piece;
    new_cell.player = old_cell.player;
    new_cell.state = old_cell.state;
    old_cell.piece = "empty";
    old_cell.player = "";
    old_cell.state = nullptr;
}
std::string Default::get_name() const {
    return "default";
}

PromoteWhitePawnToQueen::PromoteWhitePawnToQueen() {}
PromoteWhitePawnToQueen::~PromoteWhitePawnToQueen() {}
void PromoteWhitePawnToQueen::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    Cell &cell = environment->board[new_x][new_y];
    cell.piece = "wQueen";
    cell.player = "white";
    cell.state = environment->pieces["wQueen"].second.get();
}
std::string PromoteWhitePawnToQueen::get_name() const {
    return "promotewhitepawntoqueen";
}

std::map<std::string, std::shared_ptr<SideEffect>> SideEffects::get_side_effect = {
    {"default", std::make_shared<Default>()},
    {"promotewhitepawntoqueen", std::make_shared<PromoteWhitePawnToQueen>()},
};
