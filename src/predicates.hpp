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

class WhitePawn : public Predicate
{
  public:
    WhitePawn();
    ~WhitePawn();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

class WhiteKnight : public Predicate
{
  public:
    WhiteKnight();
    ~WhiteKnight();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

class WhiteBishop : public Predicate
{
  public:
    WhiteBishop();
    ~WhiteBishop();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

class WhiteRook : public Predicate
{
  public:
    WhiteRook();
    ~WhiteRook();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

class WhiteQueen : public Predicate
{
  public:
    WhiteQueen();
    ~WhiteQueen();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

class WhiteKing : public Predicate
{
  public:
    WhiteKing();
    ~WhiteKing();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

class BlackPawn : public Predicate
{
  public:
    BlackPawn();
    ~BlackPawn();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

class BlackKnight : public Predicate
{
  public:
    BlackKnight();
    ~BlackKnight();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

class BlackBishop : public Predicate
{
  public:
    BlackBishop();
    ~BlackBishop();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

class BlackRook : public Predicate
{
  public:
    BlackRook();
    ~BlackRook();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

class BlackQueen : public Predicate
{
  public:
    BlackQueen();
    ~BlackQueen();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

class BlackKing : public Predicate
{
  public:
    BlackKing();
    ~BlackKing();
    bool operator()(Environment *environment, int x, int y) override;
    std::string get_name() const override;
};

class Predicates
{
  public:
    static std::map<std::string, std::shared_ptr<Predicate>> get_predicate;
};
