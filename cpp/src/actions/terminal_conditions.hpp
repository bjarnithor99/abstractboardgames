// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include "environment.hpp"
#include "variables.hpp"
#include <memory>
#include <string>

class TerminalCondition
{
  public:
    virtual ~TerminalCondition() {}
    virtual bool operator()(Environment *environment) = 0;
    virtual std::string get_name() const = 0;
};

class NoMovesLeft : public TerminalCondition
{
  public:
    NoMovesLeft();
    ~NoMovesLeft();
    bool operator()(Environment *environment) override;
    std::string get_name() const override;
};

class WhiteReachedEnd : public TerminalCondition
{
  public:
    WhiteReachedEnd();
    ~WhiteReachedEnd();
    bool operator()(Environment *environment) override;
    std::string get_name() const override;
};

class BlackReachedEnd : public TerminalCondition
{
  public:
    BlackReachedEnd();
    ~BlackReachedEnd();
    bool operator()(Environment *environment) override;
    std::string get_name() const override;
};

class TerminalConditions
{
  public:
    static std::map<std::string, std::shared_ptr<TerminalCondition>> terminal_conditions;
};
