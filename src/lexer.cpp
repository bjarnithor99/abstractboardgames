// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "lexer.hpp"

std::ostream &operator<<(std::ostream &os, const Token &token) {
    switch (token) {
    case Token::EOI:
        os << "Token::EOI";
        break;
    case Token::Unknown:
        os << "Token::Unknown";
        break;
    case Token::OpStar:
        os << "Token::OpStar";
        break;
    case Token::OpQuestion:
        os << "Token::OpQuestion";
        break;
    case Token::OpPlus:
        os << "Token::OpPlus";
        break;
    case Token::OpOr:
        os << "Token::OpOr";
        break;
    case Token::OpAssign:
        os << "Token::OpAssign";
        break;
    case Token::LParen:
        os << "Token::LParen";
        break;
    case Token::RParen:
        os << "Token::RParen";
        break;
    case Token::LSquare:
        os << "Token::LSquare";
        break;
    case Token::RSquare:
        os << "Token::RSquare";
        break;
    case Token::Comma:
        os << "Token::Comma";
        break;
    case Token::Hashtag:
        os << "Token::Hashtag";
        break;
    case Token::Players:
        os << "Token::Players";
        break;
    case Token::Pieces:
        os << "Token::Pieces";
        break;
    case Token::BoardSize:
        os << "Token::BoardSize";
        break;
    case Token::Board:
        os << "Token::Board";
        break;
    case Token::Rule:
        os << "Token::Rule";
        break;
    case Token::Number:
        os << "Token::Number";
        break;
    case Token::String:
        os << "Token::String";
        break;
    default:
        os << "Not implemented";
        break;
    }
    return os;
}

Location::Location() {}
Location::Location(int line, int col) : line(line), col(col) {}
Location::Location(Location *loc) : line(loc->line), col(loc->col) {}

std::ostream &operator<<(std::ostream &os, const Location &location) {
    os << "line: " << location.line << " col: " << location.col;
    return os;
}

TokenTuple::TokenTuple() {}
TokenTuple::TokenTuple(Token token, std::string lexeme, Location location)
    : token(token), lexeme(lexeme), location(location) {}

Lexer::Lexer(std::ifstream *filestream) : filestream(filestream), location(1, 0) {
    if (!filestream->is_open()) {
        throw std::runtime_error("File not open");
    }
    read_next_char();
}
std::map<std::string, Token> Lexer::reserved_keywords = {
    {"players", Token::Players}, {"pieces", Token::Pieces}, {"board_size", Token::BoardSize},
    {"board", Token::Board},     {"rule", Token::Rule},
};

TokenTuple Lexer::next() {
    // Remove white-spaces, if any, before matching next token.
    while (isspace(ch)) {
        read_next_char();
    }

    // Record the starting location of the lexeme we are matching.
    Location loc = Location(&location);

    // Try to match a lexeme.
    TokenTuple ret;
    if (ch == '\0') {
        ret = TokenTuple(Token::EOI, "", loc);
        read_next_char();
    }
    else if (ch == '*') {
        ret = TokenTuple(Token::OpStar, "*", loc);
        read_next_char();
    }
    else if (ch == '?') {
        ret = TokenTuple(Token::OpQuestion, "?", loc);
        read_next_char();
    }
    else if (ch == '+') {
        ret = TokenTuple(Token::OpPlus, "+", loc);
        read_next_char();
    }
    else if (ch == '|') {
        ret = TokenTuple(Token::OpOr, "|", loc);
        read_next_char();
    }
    else if (ch == '=') {
        ret = TokenTuple(Token::OpAssign, "=", loc);
        read_next_char();
    }
    else if (ch == '(') {
        ret = TokenTuple(Token::LParen, "(", loc);
        read_next_char();
    }
    else if (ch == ')') {
        ret = TokenTuple(Token::RParen, ")", loc);
        read_next_char();
    }
    else if (ch == '[') {
        ret = TokenTuple(Token::LSquare, "[", loc);
        read_next_char();
    }
    else if (ch == ']') {
        ret = TokenTuple(Token::RSquare, "]", loc);
        read_next_char();
    }
    else if (ch == ',') {
        ret = TokenTuple(Token::Comma, ",", loc);
        read_next_char();
    }
    else if (ch == '#') {
        ret = TokenTuple(Token::Hashtag, "#", loc);
        read_next_char();
    }
    else if (isdigit(ch) || ch == '-') {
        std::string number(1, ch);
        read_next_char();
        while (isdigit(ch)) {
            number += ch;
            read_next_char();
        }
        ret = TokenTuple(Token::Number, number, loc);
    }
    else if (isalpha(ch)) {
        std::string str(1, ch);
        read_next_char();
        while (isalnum(ch) || ch == '-' || ch == '_') {
            str += ch;
            read_next_char();
        }
        if (reserved_keywords.find(str) != reserved_keywords.end()) {
            ret = TokenTuple(reserved_keywords[str], str, loc);
        }
        else {
            ret = TokenTuple(Token::String, str, loc);
        }
    }
    else {
        ret = TokenTuple(Token::Unknown, std::string(1, ch), loc);
        read_next_char();
    }
    return ret;
}

char Lexer::read_next_char() {
    if (filestream->peek() == std::ifstream::traits_type::eof()) {
        ch = '\0';
    }
    else {
        filestream->get(ch);
        if (ch == '\n') {
            location.line += 1;
            location.col = 0;
        }
        else {
            location.col++;
        }
    }
    return ch;
}
