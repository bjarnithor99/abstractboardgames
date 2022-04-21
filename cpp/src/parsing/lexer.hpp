// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
/**
 *  @file lexer.hpp
 *  @brief Defines a Lexer for the game description language.
 *  @author Bjarni Dagur Thor Kárason
 */
#pragma once

#include <cctype>
#include <fstream>
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>

/// @brief Tokens recognized by the Lexer.
/// @details
///  A game description will be decomposed into these tokens, which can then be
///  parsed by the Parser.
///
/// @see Lexer
/// @see Parser
enum class Token
{
    /// @brief End of input.
    EOI,
    /// @brief Unkown token.
    Unknown,

    // Operators
    /// @brief The star '*' operator.
    OpStar,
    /// @brief The question mark '?' operator.
    OpQuestion,
    /// @brief The plus '+' operator.
    OpPlus,
    /// @brief The or '|' operator.
    OpOr,
    /// @brief The assignment '=' operator.
    OpAssign,

    // Punctuation marks
    /// @brief A left parenthesis '('.
    LParen,
    /// @brief A left parenthesis ')'.
    RParen,
    /// @brief A left square bracket '['.
    LSquare,
    /// @brief A right square bracket ']'.
    RSquare,
    /// @brief A left curly bracket '{'.
    LCurly,
    /// @brief A right curly bracket '}'.
    RCurly,
    /// @brief A comma ','.
    Comma,
    /// @brief A semicomma ';'.
    Semicomma,

    // Declaration marks
    /// @brief The players declaration token 'players'.
    Players,
    /// @brief The piece declaration token 'pieces'.
    Pieces,
    /// @brief The board size declaration token 'board_size'.
    BoardSize,
    /// @brief The board declaration token 'board'.
    Board,
    /// @brief The rule declaration token 'rule'.
    Rule,
    /// @brief The macro declaration token 'macro'.
    Macro,
    /// @brief The post condition declaration token 'post'.
    PostCondition,

    // Units
    /// @brief A number token.
    Number,
    /// @brief A string token.
    String,
};

/// @brief Defines how a Token should be printed.
std::ostream &operator<<(std::ostream &os, const Token &t);

/// @brief A Token's Location.
/// @details
///  Stores the location (in the input file) of the last read Token.
struct Location {
    /// @brief The number of the line the last Token was read from.
    int line;
    /// @brief The number of the column the last Token was read from.
    int col;
    /// @brief Location constructor.
    Location();
    /// @brief Location constructor from line/column pair.
    Location(int line, int col);
    /// @brief Location copy constructor.
    Location(const Location &loc);
};

/// @brief Defines how Location should be printed.
std::ostream &operator<<(std::ostream &os, const Location &location);

/// @brief Aggregation of useful lexing information.
/// @details
///  Contains information about the next token to process, such as the Token's
///  type, its lexeme, and Location.
///
/// @see Token
/// @see Location
struct TokenTuple {
    /// @brief The type of the next Token.
    Token token;
    /// @brief The Token's lexeme.
    std::string lexeme;
    /// @brief The Location of the Token.
    Location location;
    /// @brief TokenTuple constructor.
    TokenTuple();
    /// @brief TokenTuple constructor for a Token, lexeme, and Location.
    TokenTuple(Token token, std::string lexeme, Location location);
};

/// @brief A class that tokenizes a game description.
/// @details
///  Takes as input a file containing a game description and tokenizes it. To be
///  used with Parser.
///
/// @see Parser
///
/// @author Bjarni Dagur Thor Kárason
class Lexer
{
  public:
    /// @brief Lexer constructor.
    ///
    /// @param file_path a path to a file containing the game description to tokenize.
    Lexer(std::string file_path);
    /// @brief Lexer descructor.
    ~Lexer();
    /// @brief Get the next Token in the game description as a TokenTuple.
    ///
    /// @returns a TokenTuple containing the next Token.
    TokenTuple next();

  private:
    /// @brief Maps reserved keywords to their corresponding Tokens.
    static std::map<std::string, Token> reserved_keywords;
    /// @brief The game description as a filestream.
    std::ifstream filestream;
    /// @brief The current character in the filestream.
    char ch;
    /// @brief The current Location in the filestream.
    Location location;
    /// @brief Get the next character in the filestream.
    /// @details
    ///  Updates the current character (ch) and the current location.
    ///
    /// @returns the next character in the filestream.
    char read_next_char();
};
