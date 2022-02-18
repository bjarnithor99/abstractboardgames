// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include "ast.hpp"
#include "visitor.hpp"
#include <iostream>
#include <string>

std::ostream &operator<<(std::ostream &os, const UnaryOperator &unaryOperator);
std::ostream &operator<<(std::ostream &os, const BinaryOperator &binaryOperator);

class PrintVisitor : public Visitor
{
  public:
    PrintVisitor();
    void visitWordsNode(WordsNode *wordsNode) override;
    void visitLetterNode(LetterNode *letterNode) override;
    void visitBinaryOpNode(BinaryOpNode *binaryOpNode) override;
    void visitUnaryOpNode(UnaryOpNode *unaryOpNode) override;
    std::string indent();

  private:
    int depth;
};
