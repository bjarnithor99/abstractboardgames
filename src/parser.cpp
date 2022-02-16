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

SentenceNode *Parser::parse() {
    SentenceNode *sentenceNode = parse_sentence();
    return sentenceNode;
}

SentenceNode *Parser::parse_sentence() {
    SentenceNode *sentenceNode = new SentenceNode();
    while (tokenTuple.token != Token::EOI) {
        if (tokenTuple.token == Token::RParen)
            break;
        sentenceNode->add_word_node(parse_word());
    }
    return sentenceNode;
}

WordNode *Parser::parse_word() {
    Node *expr = parse_unary_word();
    while (match_if(Token::OpOr)) {
        Node *rhs = parse_unary_word();
        expr = new BinaryOpNode(BinaryOperator::OpOr, expr, rhs);
    }
    return new WordNode(expr);
}

UnaryWordNode *Parser::parse_unary_word() {
    Node *childNode = parse_core_word();
    if (match_if(Token::OpStar)) {
        childNode = new UnaryOpNode(UnaryOperator::OpStar, childNode);
    }
    else if (match_if(Token::OpQuestion)) {
        childNode = new UnaryOpNode(UnaryOperator::OpQuestion, childNode);
    }
    else if (match_if(Token::OpPlus)) {
        childNode = new UnaryOpNode(UnaryOperator::OpPlus, childNode);
    }
    return new UnaryWordNode(childNode);
}

CoreWordNode *Parser::parse_core_word() {
    Node *childNode;
    if (match_if(Token::LParen)) {
        childNode = parse_sentence();
        match(Token::RParen);
    }
    else {
        childNode = parse_letter();
    }
    return new CoreWordNode(childNode);
}

LetterNode *Parser::parse_letter() {
    match(Token::LSquare);
    int dx = parse_number();
    match(Token::Comma);
    int dy = parse_number();
    match(Token::Comma);
    std::string predicate = parse_predicate();
    match(Token::RSquare);
    return new LetterNode(dx, dy, predicate);
}

int Parser::parse_number() {
    std::string lexeme = tokenTuple.lexeme;
    match(Token::Number);
    return std::stoi(lexeme);
}

std::string Parser::parse_predicate() {
    std::string lexeme = tokenTuple.lexeme;
    match(Token::Predicate);
    return lexeme;
}
