// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

class Variables
{
  public:
    Variables();
    Variables(const Variables &variables);
    ~Variables();

    int black_score;
    int white_score;
};
