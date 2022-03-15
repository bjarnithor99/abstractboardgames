// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include <map>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <vector>

class Predicate;

class NFAInput
{
  public:
    NFAInput();
    NFAInput(int dx, int dy, std::shared_ptr<Predicate> predicate, std::string side_effect);
    ~NFAInput();
    bool operator<(const NFAInput &rhs) const;

    int dx;
    int dy;
    std::shared_ptr<Predicate> predicate;
    std::string side_effect;
    bool is_epsilon;
};

class NFAState
{
  public:
    NFAState();
    ~NFAState();
    void add_transition(NFAState *dst, NFAInput input);
    void destroy();

    bool is_accepting;
    std::map<NFAInput, std::vector<NFAState *>> transitions;
};

struct NFAStateDeleter {
    void operator()(NFAState *state) const;
};
