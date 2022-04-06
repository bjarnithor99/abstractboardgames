// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "variables.hpp"

Variables::Variables() : black_score(0), white_score(0), game_over(false) {}
Variables::Variables(const Variables &variables)
    : black_score(variables.black_score), white_score(variables.white_score), game_over(variables.game_over) {}
Variables::~Variables() {}
