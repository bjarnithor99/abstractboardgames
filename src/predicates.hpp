// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include "environment.hpp"
#include "variables.hpp"
#include <memory>
#include <string>

class Predicate
{
  public:
    virtual ~Predicate() {}
    virtual bool operator()(Environment *environment, int x, int y) = 0;
    virtual std::string get_name() const = 0;
};

class Empty : public Predicate
{
  public:
    Empty();
    ~Empty();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

class Opponent : public Predicate
{
  public:
    Opponent();
    ~Opponent();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

class Predicates
{
  public:
    static std::map<std::string, std::shared_ptr<Predicate>> get_predicate;
};
