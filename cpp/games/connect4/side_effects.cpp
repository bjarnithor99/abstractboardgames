// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "side_effects.hpp"

Default::Default() {}
Default::~Default() {}
void Default::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {}
std::string Default::get_name() const {
    return "Default";
}

Place::Place() {}
Place::~Place() {}
void Place::operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) {
    std::string players_piece = environment->current_player == "black" ? "bPawn" : "wPawn";
    Cell &cell = environment->board[new_x][new_y];
    cell.owners = environment->pieces[players_piece].first;
    cell.piece = players_piece;
    cell.state = environment->pieces[players_piece].second.get();
}
std::string Place::get_name() const {
    return "Place";
}

std::map<std::string, std::shared_ptr<SideEffect>> SideEffects::get_side_effect = {
    {"Default", std::make_shared<Default>()},
    {"Place", std::make_shared<Place>()},
};
