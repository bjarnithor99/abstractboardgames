// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include <cctype>
#include <fstream>
#include <ostream>
#include <stdexcept>
#include <string>

enum class Token
{
    EOI,
    Unknown,

    // Operators
    OpStar,
    OpQuestion,
    OpPlus,
    OpOr,

    // Punctuation marks
    LParen,
    RParen,
    LSquare,
    RSquare,
    Comma,

    // Units
    Number,
    Predicate,
};

std::ostream &operator<<(std::ostream &os, const Token &t);

struct Location {
    int line;
    int col;
    Location();
    Location(int line, int col);
    Location(Location *loc);
};

std::ostream &operator<<(std::ostream &os, const Location &location);

struct TokenTuple {
    Token token;
    std::string lexeme;
    Location location;
    TokenTuple();
    TokenTuple(Token token, std::string lexeme, Location location);
};

class Lexer
{
  public:
    Lexer(std::ifstream *filestream);
    TokenTuple next();

  private:
    std::ifstream *filestream;
    char ch;
    Location location;
    char read_next_char();
};
