// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include <cctype>
#include <fstream>
#include <map>
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
    OpAssign,

    // Punctuation marks
    LParen,
    RParen,
    LSquare,
    RSquare,
    LCurly,
    RCurly,
    Comma,

    // Declaration marks
    Hashtag,
    Players,
    Pieces,
    BoardSize,
    Board,
    Rule,

    // Units
    Number,
    String,
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
    static std::map<std::string, Token> reserved_keywords;
    std::ifstream *filestream;
    char ch;
    Location location;
    char read_next_char();
};
