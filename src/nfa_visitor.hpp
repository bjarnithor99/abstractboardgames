// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include "ast.hpp"
#include "nfa.hpp"
#include "visitor.hpp"
#include <stack>

class NFAWrapper
{
  public:
    NFAWrapper(NFAState *initial_state, NFAState *final_state);
    ~NFAWrapper();
    NFAState *initial_state;
    NFAState *final_state;
};

class NFAVisitor : public Visitor
{
  public:
    NFAVisitor();
    ~NFAVisitor();
    void visitWordsNode(WordsNode *wordsNode) override;
    void visitLetterNode(LetterNode *letterNode) override;
    void visitBinaryOpNode(BinaryOpNode *binaryOpNode) override;
    void visitUnaryOpNode(UnaryOpNode *unaryOpNode) override;
    NFAState *getNFA();

  private:
    std::stack<std::unique_ptr<NFAWrapper>> nfa_stack;
};
