// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include "ast.hpp"
#include "visitor.hpp"
#include <cassert>
#include <map>
#include <stack>

class MacroVisitor : public Visitor
{
  public:
    MacroVisitor(bool in_macro, std::vector<std::string> argument_names, std::vector<std::string> argument_values);
    ~MacroVisitor();
    void visitWordsNode(WordsNode *wordsNode) override;
    void visitLetterNode(LetterNode *letterNode) override;
    void visitMacroLetterNode(MacroLetterNode *macroLetterNode) override;
    void visitBinaryOpNode(BinaryOpNode *binaryOpNode) override;
    void visitUnaryOpNode(UnaryOpNode *unaryOpNode) override;
    std::unique_ptr<Node> get_node();

  private:
    bool in_macro;
    std::map<std::string, std::string> argument_value;
    std::stack<std::unique_ptr<Node>> node_stack;
};
