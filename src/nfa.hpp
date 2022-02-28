// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include <map>
#include <string>
#include <vector>

class NFAInput
{
  public:
    NFAInput();
    NFAInput(int dx, int dy, std::string predicate);
    ~NFAInput();
    bool operator<(const NFAInput &rhs) const;

    int dx;
    int dy;
    std::string predicate;
    bool is_epsilon;
};

class NFAState
{
  public:
    NFAState();
    void add_transition(NFAState *dst, NFAInput input);

    bool is_accepting;
    std::map<NFAInput, std::vector<NFAState *>> transitions;
};