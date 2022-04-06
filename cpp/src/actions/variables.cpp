// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "variables.hpp"

Variables::Variables() {
    int blackScore = 0;
    int whiteScore = 0;
}
Variables::Variables(const Variables &variables) {
    blackScore = variables.blackScore;
    whiteScore = variables.whiteScore;
}
Variables::~Variables() {}
