// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include "ast.hpp"
#include "lexer.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

class Parser
{
  public:
    Parser(std::ifstream *filestream);
    SentenceNode *parse();

  private:
    std::ifstream *filestream;
    Lexer lexer;
    TokenTuple tokenTuple;
    void match(Token token);
    bool match_if(Token token);
    SentenceNode *parse_sentence();
    Node *parse_word();
    Node *parse_unary_word();
    Node *parse_core_word();
    LetterNode *parse_letter();
    int parse_number();
    std::string parse_predicate();
};
