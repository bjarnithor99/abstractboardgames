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
#include "macro_visitor.hpp"
#include <memory>
#include <set>
#include <sstream>
#include <utility>

class Parser
{
  public:
    Parser(std::string file_path);
    ~Parser();
    void parse();
    std::unique_ptr<Environment> get_environment();

  private:
    Lexer lexer;
    TokenTuple tokenTuple;
    std::vector<std::string> players;
    std::map<std::string, std::pair<std::string, std::unique_ptr<DFAState, DFAStateDeleter>>> pieces;
    std::map<std::string, std::pair<std::unique_ptr<Node>, std::vector<std::string>>> macros;
    std::map<std::string, std::vector<std::pair<std::string, std::unique_ptr<DFAState, DFAStateDeleter>>>>
        post_conditions;
    std::unique_ptr<Environment> environment;
    void match(Token token);
    bool match_if(Token token);
    void parse_player_list();
    void parse_pieces_list();
    void parse_piece();
    void parse_board_size();
    void parse_board();
    void parse_rule();
    void parse_macro();
    std::vector<std::string> parse_arguments();
    std::string parse_argument();
    void parse_post_condition();
    std::unique_ptr<Node> parse_macro_call(bool in_macro);
    std::unique_ptr<Node> parse_sentence(bool in_macro = false);
    std::unique_ptr<WordsNode> parse_word(bool in_macro);
    std::unique_ptr<Node> parse_core_word(bool in_macro);
    std::unique_ptr<LetterNode> parse_letter();
    std::unique_ptr<MacroLetterNode> parse_macro_letter();
    int parse_int();
    std::string parse_string();
};
