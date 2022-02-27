// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "parser.hpp"

Parser::Parser(std::ifstream *filestream)
    : filestream(filestream), lexer(filestream), tokenTuple(lexer.next()), board_size({-1, -1}) {}

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
    while (match_if(Token::Hashtag)) {
        if (tokenTuple.token == Token::Players)
            parse_player_list();
        else if (tokenTuple.token == Token::Pieces)
            parse_pieces_list();
        else if (tokenTuple.token == Token::BoardSize)
            parse_board_size();
        else if (tokenTuple.token == Token::Board)
            parse_board();
        else
            parse_rule();
    }
    match(Token::EOI);
}

void Parser::parse_player_list() {
    if (!players.empty()) {
        std::ostringstream oss;
        oss << "Redeclaration of players in " << tokenTuple.location;
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    match(Token::Players);
    match(Token::OpAssign);
    std::string player = parse_string();
    players.insert(player);
    while (match_if(Token::Comma)) {
        Location loc = tokenTuple.location;
        player = parse_string();
        if (players.find(player) != players.end()) {
            std::ostringstream oss;
            oss << "Duplicate player " << player << " in player declaration in " << loc << ".";
            std::string error_msg = oss.str();
            throw std::runtime_error(error_msg);
        }
        players.insert(player);
    }
}

void Parser::parse_pieces_list() {
    if (!pieces.empty()) {
        std::ostringstream oss;
        oss << "Redeclaration of pieces in " << tokenTuple.location;
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    match(Token::Pieces);
    match(Token::OpAssign);
    parse_piece();
    while (match_if(Token::Comma)) {
        parse_piece();
    }
}

void Parser::parse_piece() {
    Location loc = tokenTuple.location;
    std::string piece = parse_string();
    if (pieces.find(piece) != pieces.end()) {
        std::ostringstream oss;
        oss << "Duplicate piece " << piece << " in piece declaration in " << loc << ".";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    std::string player;
    if (match_if(Token::LParen)) {
        loc = tokenTuple.location;
        player = parse_string();
        if (players.find(player) == players.end()) {
            std::ostringstream oss;
            oss << "Unrecognized player " << player << " in piece declaration in " << loc << ".";
            std::string error_msg = oss.str();
            throw std::runtime_error(error_msg);
        }
        match(Token::RParen);
    }
    pieces[piece] = {player, nullptr};
}

void Parser::parse_board_size() {
    if (board_size != std::make_pair(-1, -1)) {
        std::ostringstream oss;
        oss << "Redeclaration of board size in " << tokenTuple.location << ".";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    match(Token::BoardSize);
    match(Token::OpAssign);
    int x = parse_int();
    match(Token::Comma);
    int y = parse_int();
    if (x <= 0 || y <= 0) {
        std::ostringstream oss;
        oss << "Invalid board size (" << x << ", " << y << "). Board size must be positive.";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    board_size = {x, y};
}

void Parser::parse_board() {
    if (!board.empty()) {
        std::ostringstream oss;
        oss << "Redeclaration of board in " << tokenTuple.location << ".";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    if (board_size == std::make_pair(-1, -1)) {
        throw std::runtime_error("Board size must be defined before board.");
    }
    board.resize(board_size.first, std::vector<std::string>(board_size.second));
    match(Token::Board);
    match(Token::OpAssign);
    Location loc = tokenTuple.location;
    std::string piece = parse_string();
    board[0][0] = piece;
    if (pieces.find(piece) == pieces.end()) {
        std::ostringstream oss;
        oss << "Unrecognized piece " << piece << " in board declaration in " << loc << ".";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    int piece_count = 1;
    while (match_if(Token::Comma)) {
        loc = tokenTuple.location;
        piece = parse_string();
        if (pieces.find(piece) == pieces.end()) {
            std::ostringstream oss;
            oss << "Unrecognized piece " << piece << " in board declaration in " << loc << ".";
            std::string error_msg = oss.str();
            throw std::runtime_error(error_msg);
        }
        board[piece_count / board_size.first][piece_count % board_size.second] = piece;
        piece_count++;
    }
    if (piece_count != board_size.first * board_size.second) {
        std::ostringstream oss;
        oss << "Invalid board declaration. Expected " << board_size.first * board_size.second << " pieces but got "
            << piece_count << ".";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
}

void Parser::parse_rule() {
    match(Token::Rule);
    Location loc = tokenTuple.location;
    std::string piece = parse_string();
    if (pieces.find(piece) == pieces.end()) {
        std::ostringstream oss;
        oss << "Unrecognized piece " << piece << " in rule declaration in " << loc << ".";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    if (pieces[piece].second != nullptr) {
        std::ostringstream oss;
        oss << "Redeclaration of rule for piece " << piece << " in rule declaration in " << loc << ".";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    match(Token::OpAssign);
    Node *node = parse_sentence();
    pieces[piece].second = FATools::getMinimizedDfa(node);
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
    int dx = parse_int();
    match(Token::Comma);
    int dy = parse_int();
    match(Token::Comma);
    std::string predicate = parse_string();
    match(Token::RSquare);
    return new LetterNode(dx, dy, predicate);
}

int Parser::parse_int() {
    std::string lexeme = tokenTuple.lexeme;
    match(Token::Number);
    return std::stoi(lexeme);
}

std::string Parser::parse_string() {
    std::string lexeme = tokenTuple.lexeme;
    match(Token::String);
    return lexeme;
}
