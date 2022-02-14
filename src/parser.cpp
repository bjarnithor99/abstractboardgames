// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "parser.hpp"

Parser::Parser(std::ifstream *filestream) : filestream(filestream), lexer(filestream), tokenTuple(lexer.next()) {}

void Parser::match(Token token) {
    if (tokenTuple.token == token) {
        tokenTuple = lexer.next();
    }
    else {
        std::ostringstream oss;
        oss << "Syntax error: expected " << token << " but got " << tokenTuple.token << " ('" << tokenTuple.lexeme
            << "') in " << tokenTuple.location << ".";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
}

bool Parser::match_if(Token token) {
    if (tokenTuple.token == token) {
        match(token);
        return true;
    }
    else {
        return false;
    }
}

void Parser::parse() {
    parse_sentence();
}

void Parser::parse_sentence() {
    while (tokenTuple.token != Token::EOI) {
        if (tokenTuple.token == Token::RParen)
            break;
        parse_word();
    }
}

void Parser::parse_word() {
    parse_unary_word();
    if (match_if(Token::OpOr)) {
        parse_word();
    }
}

void Parser::parse_unary_word() {
    parse_core_word();
    if (match_if(Token::OpStar)) {}
    else if (match_if(Token::OpQuestion)) {
    }
    else if (match_if(Token::OpPlus)) {
    }
}

void Parser::parse_core_word() {
    if (match_if(Token::LParen)) {
        parse_sentence();
        match(Token::RParen);
    }
    else {
        parse_letter();
    }
}

void Parser::parse_letter() {
    match(Token::LSquare);
    match(Token::Number);
    match(Token::Comma);
    match(Token::Number);
    match(Token::Comma);
    match(Token::Predicate);
    match(Token::RSquare);
}
    while (match_if(Token::OpOr)) {
        parse_unary_word();
