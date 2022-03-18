// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include "environment.hpp"
#include <memory>
#include <string>

class SideEffect
{
  public:
    virtual ~SideEffect() {}
    virtual void operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) = 0;
    virtual std::string get_name() const = 0;
};

class Default : public SideEffect
{
  public:
    Default();
    ~Default();
    void operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) override;
    std::string get_name() const override;
};

class PromoteWhitePawnToQueen : public SideEffect
{
  public:
    PromoteWhitePawnToQueen();
    ~PromoteWhitePawnToQueen();
    void operator()(Environment *environment, int old_x, int old_y, int new_x, int new_y) override;
    std::string get_name() const override;
};

class SideEffects
{
  public:
    static std::map<std::string, std::shared_ptr<SideEffect>> get_side_effect;
};
