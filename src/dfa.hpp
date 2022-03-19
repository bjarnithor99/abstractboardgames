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

class Predicate;
class SideEffect;

class DFAInput
{
  public:
    DFAInput(int dx, int dy, std::shared_ptr<Predicate> predicate, std::shared_ptr<SideEffect> side_effect);
    ~DFAInput();
    bool operator<(const DFAInput &rhs) const;
    bool operator==(const DFAInput &rhs) const;

    int dx;
    int dy;
    std::shared_ptr<Predicate> predicate;
    std::shared_ptr<SideEffect> side_effect;
};

class DFAState
{
  public:
    DFAState();
    ~DFAState();
    void add_transition(DFAState *dst, DFAInput input);
    void destroy();

    bool is_accepting;
    std::map<DFAInput, DFAState *> transition;
};

struct DFAStateDeleter {
    void operator()(DFAState *state) const;
};
