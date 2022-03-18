// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "parser.hpp"

Parser::Parser(std::ifstream *filestream)
    : filestream(filestream), lexer(filestream), tokenTuple(lexer.next()), environment(nullptr) {}
Parser::~Parser() {}

void Parser::parse() {
    while (tokenTuple.token != Token::EOI) {
        if (tokenTuple.token == Token::Players) {
            parse_player_list();
            match(Token::Semicomma);
        }
        else if (tokenTuple.token == Token::Pieces) {
            parse_pieces_list();
            match(Token::Semicomma);
        }
        else if (tokenTuple.token == Token::Rule) {
            parse_rule();
            match(Token::Semicomma);
        }
        else if (tokenTuple.token == Token::BoardSize) {
            parse_board_size();
            match(Token::Semicomma);
        }
        else {
            parse_board();
            match(Token::Semicomma);
        }
    }
    match(Token::EOI);
}

std::unique_ptr<Environment> Parser::get_environment() {
    environment->pieces.merge(pieces);
    return std::move(environment);
}

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

void Parser::parse_player_list() {
    match(Token::Players);
    match(Token::OpAssign);
    if (!players.empty()) {
        std::ostringstream oss;
        oss << "Redeclaration of players in " << tokenTuple.location;
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
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
    match(Token::Pieces);
    match(Token::OpAssign);
    if (!pieces.empty()) {
        std::ostringstream oss;
        oss << "Redeclaration of pieces in " << tokenTuple.location;
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
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
    std::string player = "";
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
    match(Token::BoardSize);
    match(Token::OpAssign);
    if (environment != nullptr) {
        std::ostringstream oss;
        oss << "Redeclaration of board size in " << tokenTuple.location << ".";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    int x = parse_int();
    match(Token::Comma);
    int y = parse_int();
    if (x <= 0 || y <= 0) {
        std::ostringstream oss;
        oss << "Invalid board size (" << x << ", " << y << "). Board size must be positive.";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    environment = std::make_unique<Environment>(x, y);
}

void Parser::parse_board() {
    match(Token::Board);
    match(Token::OpAssign);
    if (environment == nullptr) {
        throw std::runtime_error("Board size must be declared before board.");
    }
    if (!environment->board.empty()) {
        std::ostringstream oss;
        oss << "Redeclaration of board in " << tokenTuple.location << ".";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    environment->board.resize(environment->board_size_x, std::vector<Cell>(environment->board_size_y, Cell()));
    Location loc = tokenTuple.location;
    std::string piece = parse_string();
    if (pieces.find(piece) == pieces.end()) {
        std::ostringstream oss;
        oss << "Unrecognized piece " << piece << " in board declaration in " << loc << ".";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    if (pieces[piece].second == nullptr && pieces[piece].first != "") {
        std::ostringstream oss;
        oss << "Rule for piece " << piece << " must be declared before board declaration.";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    int board_size_x = environment->board_size_x;
    int board_size_y = environment->board_size_y;
    int cur_x = 0, cur_y = 0;
    environment->board[cur_x][cur_y] =
        Cell(cur_y, board_size_x - 1 - cur_x, piece, pieces[piece].first, pieces[piece].second.get());
    cur_x += ((cur_y + 1) / board_size_y);
    cur_y = (cur_y + 1) % board_size_y;
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
        if (pieces[piece].second == nullptr && pieces[piece].first != "") {
            std::ostringstream oss;
            oss << "Rule for piece " << piece << " must be declared before board declaration.";
            std::string error_msg = oss.str();
            throw std::runtime_error(error_msg);
        }
        environment->board[cur_x][cur_y] =
            Cell(cur_y, board_size_x - 1 - cur_x, piece, pieces[piece].first, pieces[piece].second.get());
        cur_x += ((cur_y + 1) / board_size_y);
        cur_y = (cur_y + 1) % board_size_y;
        piece_count++;
    }
    if (piece_count != board_size_x * board_size_y) {
        std::ostringstream oss;
        oss << "Invalid board declaration. Expected " << board_size_x * board_size_y << " pieces but got "
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
    std::unique_ptr<Node> node = parse_sentence();
    pieces[piece].second = FATools::getMinimizedDfa(node.get());
}

std::unique_ptr<Node> Parser::parse_sentence() {
    std::unique_ptr<Node> node = parse_word();
    while (match_if(Token::OpOr)) {
        std::unique_ptr<Node> rhs = parse_word();
        node = std::make_unique<BinaryOpNode>(BinaryOperator::OpOr, std::move(node), std::move(rhs));
    }
    return node;
}

std::unique_ptr<WordsNode> Parser::parse_word() {
    std::unique_ptr<WordsNode> wordsNode = std::make_unique<WordsNode>();
    do {
        std::unique_ptr<Node> node = parse_core_word();
        if (match_if(Token::OpStar)) {
            node = std::make_unique<UnaryOpNode>(UnaryOperator::OpStar, std::move(node));
        }
        else if (match_if(Token::OpQuestion)) {
            node = std::make_unique<UnaryOpNode>(UnaryOperator::OpQuestion, std::move(node));
        }
        else if (match_if(Token::OpPlus)) {
            node = std::make_unique<UnaryOpNode>(UnaryOperator::OpPlus, std::move(node));
        }
        wordsNode->add_word_node(std::move(node));
    } while (tokenTuple.token == Token::LParen || tokenTuple.token == Token::LSquare);
    return wordsNode;
}

std::unique_ptr<Node> Parser::parse_core_word() {
    std::unique_ptr<Node> node;
    if (match_if(Token::LParen)) {
        node = parse_sentence();
        match(Token::RParen);
    }
    else {
        node = parse_letter();
    }
    return node;
}

std::unique_ptr<LetterNode> Parser::parse_letter() {
    match(Token::LSquare);
    int dx = parse_int();
    match(Token::Comma);
    int dy = parse_int();
    match(Token::Comma);
    std::string predicate_name = parse_string();
    match(Token::RSquare);

    std::string side_effect = "";
    if (match_if(Token::LCurly)) {
        side_effect = parse_string();
        match(Token::RCurly);
    }

    return std::make_unique<LetterNode>(dx, dy, Predicates::get_predicate[predicate_name], side_effect);
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
