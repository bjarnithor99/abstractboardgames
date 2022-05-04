// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "variables.hpp"

Variables::Variables()
    : black_score(0), white_score(0), n_moves_found(0), game_over(false), en_passant_pawn({-2, -1, -1}),
      black_king_moved(false), black_rook_left_moved(false), black_rook_right_moved(false), white_king_moved(false),
      white_rook_left_moved(false), white_rook_right_moved(false) {}

Variables::Variables(const Variables &variables)
    : black_score(variables.black_score), white_score(variables.white_score), n_moves_found(variables.n_moves_found),
      game_over(variables.game_over), en_passant_pawn(variables.en_passant_pawn),
      black_king_moved(variables.black_king_moved), black_rook_left_moved(variables.black_rook_left_moved),
      black_rook_right_moved(variables.black_rook_right_moved), white_king_moved(variables.white_king_moved),
      white_rook_left_moved(variables.white_rook_left_moved), white_rook_right_moved(variables.white_rook_right_moved) {
}
Variables::~Variables() {}
