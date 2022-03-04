// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include <map>
#include <string>

class DFAInput
{
  public:
    DFAInput(int dx, int dy, std::string predicate, std::string side_effect);
    ~DFAInput();
    bool operator<(const DFAInput &rhs) const;
    bool operator==(const DFAInput &rhs) const;

    int dx;
    int dy;
    std::string predicate;
    std::string side_effect;
};

class DFAState
{
  public:
    DFAState();
    void add_transition(DFAState *dst, DFAInput input);

    bool is_accepting;
    std::map<DFAInput, DFAState *> transition;
};
