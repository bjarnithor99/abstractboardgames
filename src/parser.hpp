// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include "lexer.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

class Parser
{
  public:
    Parser(std::ifstream *filestream);
    void parse();

  private:
    std::ifstream *filestream;
    Lexer lexer;
    TokenTuple tokenTuple;
    void match(Token token);
    bool match_if(Token token);
    void parse_sentence();
    void parse_word();
    void parse_unary_word();
    void parse_core_word();
    void parse_letter();
};
