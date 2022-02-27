// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include "ast.hpp"
#include "dfa.hpp"
#include "fa_tools.hpp"
#include "lexer.hpp"
#include <set>
#include <sstream>
#include <utility>

class Parser
{
  public:
    Parser(std::ifstream *filestream);
    void parse();

  private:
    std::ifstream *filestream;
    Lexer lexer;
    TokenTuple tokenTuple;
    std::set<std::string> players;
    std::map<std::string, DFAState *> pieces;
    std::pair<int, int> board_size;
    std::vector<std::vector<std::string>> board;
    void match(Token token);
    bool match_if(Token token);
    void parse_player_list();
    void parse_pieces_list();
    void parse_board_size();
    void parse_board();
    void parse_rule();
    Node *parse_sentence();
    WordsNode *parse_word();
    Node *parse_unary_word();
    Node *parse_core_word();
    LetterNode *parse_letter();
    int parse_int();
    std::string parse_string();
};
