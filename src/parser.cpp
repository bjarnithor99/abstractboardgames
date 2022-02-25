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

Node *Parser::parse() {
    Node *sentenceNode = parse_sentence();
    match(Token::EOI);
    return sentenceNode;
}

Node *Parser::parse_sentence() {
    Node *node = parse_word();
    while (match_if(Token::OpOr)) {
        Node *rhs = parse_word();
        node = new BinaryOpNode(BinaryOperator::OpOr, node, rhs);
    }
    return node;
}

WordsNode *Parser::parse_word() {
    WordsNode *wordsNode = new WordsNode();
    do {
        Node *node = parse_core_word();
        if (match_if(Token::OpStar)) {
            node = new UnaryOpNode(UnaryOperator::OpStar, node);
        }
        else if (match_if(Token::OpQuestion)) {
            node = new UnaryOpNode(UnaryOperator::OpQuestion, node);
        }
        else if (match_if(Token::OpPlus)) {
            node = new UnaryOpNode(UnaryOperator::OpPlus, node);
        }
        wordsNode->add_word_node(node);
    } while (tokenTuple.token == Token::LParen || tokenTuple.token == Token::LSquare);
    return wordsNode;
}

Node *Parser::parse_core_word() {
    Node *node;
    if (match_if(Token::LParen)) {
        node = parse_sentence();
        match(Token::RParen);
    }
    else {
        node = parse_letter();
    }
    return node;
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
    match(Token::String);
    return lexeme;
}
