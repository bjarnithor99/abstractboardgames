// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include "ast.hpp"
#include "dfa.hpp"
#include "environment.hpp"
#include "fa_tools.hpp"
#include "lexer.hpp"
#include <memory>
#include <set>
#include <sstream>
#include <utility>

class Parser
{
  public:
    Parser(std::ifstream *filestream);
    ~Parser();
    void parse();
    std::unique_ptr<Environment> get_environment();

  private:
    std::ifstream *filestream;
    Lexer lexer;
    TokenTuple tokenTuple;
    std::set<std::string> players;
    std::map<std::string, std::pair<std::string, std::unique_ptr<DFAState, DFAStateDeleter>>> pieces;
    std::unique_ptr<Environment> environment;
    void match(Token token);
    bool match_if(Token token);
    void parse_player_list();
    void parse_pieces_list();
    void parse_piece();
    void parse_board_size();
    void parse_board();
    void parse_rule();
    std::unique_ptr<Node> parse_sentence();
    std::unique_ptr<WordsNode> parse_word();
    std::unique_ptr<Node> parse_core_word();
    std::unique_ptr<LetterNode> parse_letter();
    int parse_int();
    std::string parse_string();
};
