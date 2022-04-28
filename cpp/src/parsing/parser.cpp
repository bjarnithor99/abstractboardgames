// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "parser.hpp"

Parser::Parser(std::string file_path) : lexer(file_path), tokenTuple(lexer.next()), environment(nullptr) {}
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
        else if (tokenTuple.token == Token::Macro) {
            parse_macro();
            match(Token::Semicomma);
        }
        else if (tokenTuple.token == Token::Rule) {
            parse_rule();
            match(Token::Semicomma);
        }
        else if (tokenTuple.token == Token::PostCondition) {
            parse_post_condition();
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
    environment->players = players;
    environment->current_player = players[0];
    environment->pieces.merge(pieces);
    environment->post_conditions.merge(post_conditions);
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
    players.push_back(player);
    while (match_if(Token::Comma)) {
        Location loc = tokenTuple.location;
        player = parse_string();
        if (std::find(players.begin(), players.end(), player) != players.end()) {
            std::ostringstream oss;
            oss << "Duplicate player " << player << " in player declaration in " << loc << ".";
            std::string error_msg = oss.str();
            throw std::runtime_error(error_msg);
        }
        players.push_back(player);
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
        if (std::find(players.begin(), players.end(), player) == players.end()) {
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
    environment = std::make_unique<Environment>(y, x);
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
    environment->board[cur_x][cur_y] = Cell(piece, pieces[piece].first, pieces[piece].second.get());
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
        environment->board[cur_x][cur_y] = Cell(piece, pieces[piece].first, pieces[piece].second.get());
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

void Parser::parse_macro() {
    match(Token::Macro);
    Location loc = tokenTuple.location;
    std::string macro_name = parse_string();
    if (macros.find(macro_name) != macros.end()) {
        std::ostringstream oss;
        oss << "Redefinition of macro " << macro_name << " in " << loc << ".";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    match(Token::LParen);
    std::vector<std::string> arguments = parse_arguments();
    match(Token::RParen);
    match(Token::OpAssign);
    std::unique_ptr<Node> node = parse_sentence(true);
    macros[macro_name] = {std::move(node), arguments};
}

std::vector<std::string> Parser::parse_arguments(bool in_macro) {
    std::vector<std::string> argument_names;
    if (tokenTuple.token == Token::RParen)
        return argument_names;
    Location loc = tokenTuple.location;
    std::string argument_name = parse_argument();
    argument_names.push_back(argument_name);
    while (match_if(Token::Comma)) {
        loc = tokenTuple.location;
        argument_name = parse_argument();
        if (!in_macro && find(argument_names.begin(), argument_names.end(), argument_name) != argument_names.end()) {
            std::ostringstream oss;
            oss << "Duplicate argument " << argument_name << " in macro definition in " << loc << ".";
            std::string error_msg = oss.str();
            throw std::runtime_error(error_msg);
        }
        argument_names.push_back(argument_name);
    }
    return argument_names;
}

std::string Parser::parse_argument() {
    std::string argument;
    if (tokenTuple.token == Token::Number)
        argument = std::to_string(parse_int());
    else
        argument = parse_string();
    return argument;
}

void Parser::parse_post_condition() {
    match(Token::PostCondition);
    Location loc = tokenTuple.location;
    std::string player = parse_string();
    if (std::find(players.begin(), players.end(), player) == players.end()) {
        std::ostringstream oss;
        oss << "Unrecognized player " << player << " in post condition declaration in " << loc << ".";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    loc = tokenTuple.location;
    std::string piece = parse_string();
    if (pieces.find(piece) == pieces.end()) {
        std::ostringstream oss;
        oss << "Unrecognized piece " << piece << " in rule declaration in " << loc << ".";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    match(Token::OpAssign);
    std::unique_ptr<Node> node = parse_sentence();
    post_conditions[player].push_back(std::make_pair(piece, FATools::getMinimizedDfa(node.get())));
}

std::unique_ptr<Node> Parser::parse_macro_call(bool in_macro) {
    Location loc = tokenTuple.location;
    std::string macro_name = parse_string();
    if (macros.find(macro_name) == macros.end()) {
        std::ostringstream oss;
        oss << "Unrecognized macro " << macro_name << " in rule declaration in " << loc << ".";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    match(Token::LParen);
    std::vector<std::string> argument_values = parse_arguments(true);
    match(Token::RParen);
    std::vector<std::string> argument_names = macros[macro_name].second;
    if (argument_names.size() != argument_values.size()) {
        std::ostringstream oss;
        oss << "Invalid number of arguments for macro " << macro_name << " in rule declaration in " << loc
            << ". Expected " << argument_names.size() << " but got " << argument_values.size() << ".";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }
    Node *macro_node = macros[macro_name].first.get();
    MacroVisitor macroVisitor = MacroVisitor(in_macro, argument_names, argument_values);
    macro_node->accept(&macroVisitor);
    std::unique_ptr<Node> node = macroVisitor.get_node();
    return node;
}

std::unique_ptr<Node> Parser::parse_sentence(bool in_macro) {
    std::unique_ptr<Node> node = parse_word(in_macro);
    while (match_if(Token::OpOr)) {
        std::unique_ptr<Node> rhs = parse_word(in_macro);
        node = std::make_unique<BinaryOpNode>(BinaryOperator::OpOr, std::move(node), std::move(rhs));
    }
    return node;
}

std::unique_ptr<WordsNode> Parser::parse_word(bool in_macro) {
    std::unique_ptr<WordsNode> wordsNode = std::make_unique<WordsNode>();
    do {
        std::unique_ptr<Node> node;
        if (tokenTuple.token == Token::String)
            node = parse_macro_call(in_macro);
        else
            node = parse_core_word(in_macro);
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
    } while (tokenTuple.token == Token::LParen || tokenTuple.token == Token::LSquare ||
             tokenTuple.token == Token::String);
    return wordsNode;
}

std::unique_ptr<Node> Parser::parse_core_word(bool in_macro) {
    std::unique_ptr<Node> node;
    if (match_if(Token::LParen)) {
        node = parse_sentence(in_macro);
        match(Token::RParen);
    }
    else {
        if (in_macro)
            node = parse_macro_letter();
        else
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

    std::string side_effect_name = "Default";
    if (match_if(Token::LCurly)) {
        side_effect_name = parse_string();
        match(Token::RCurly);
    }

    if (Predicates::get_predicate.find(predicate_name) == Predicates::get_predicate.end()) {
        std::ostringstream oss;
        oss << "Syntax error: predicate '" << predicate_name
            << "'is not defined. Did you forget to add it to Predicates::get_predicate?";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }

    if (SideEffects::get_side_effect.find(side_effect_name) == SideEffects::get_side_effect.end()) {
        std::ostringstream oss;
        oss << "Syntax error: side effect '" << side_effect_name
            << "'is not defined. Did you forget to add it to SideEffects::get_side_effect?";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }

    return std::make_unique<LetterNode>(dx, dy, Predicates::get_predicate[predicate_name],
                                        SideEffects::get_side_effect[side_effect_name]);
}

std::unique_ptr<MacroLetterNode> Parser::parse_macro_letter() {
    match(Token::LSquare);
    std::string dx = parse_argument();
    match(Token::Comma);
    std::string dy = parse_argument();
    match(Token::Comma);
    std::string predicate_name = parse_string();
    match(Token::RSquare);

    std::string side_effect_name = "Default";
    if (match_if(Token::LCurly)) {
        side_effect_name = parse_string();
        match(Token::RCurly);
    }

    if (Predicates::get_predicate.find(predicate_name) == Predicates::get_predicate.end()) {
        std::ostringstream oss;
        oss << "Syntax error: predicate '" << predicate_name
            << "'is not defined. Did you forget to add it to Predicates::get_predicate?";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }

    if (SideEffects::get_side_effect.find(side_effect_name) == SideEffects::get_side_effect.end()) {
        std::ostringstream oss;
        oss << "Syntax error: side effect '" << side_effect_name
            << "'is not defined. Did you forget to add it to SideEffects::get_side_effect?";
        std::string error_msg = oss.str();
        throw std::runtime_error(error_msg);
    }

    return std::make_unique<MacroLetterNode>(dx, dy, Predicates::get_predicate[predicate_name],
                                             SideEffects::get_side_effect[side_effect_name]);
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
