// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
/**
 *  @file print_visitor.hpp
 *  @brief A Print Visitor for regular expression abstract syntax trees.
 *  @author Bjarni Dagur Thor Kárason
 *  @see ast.hpp
 */
#pragma once

#include "ast.hpp"
#include "visitor.hpp"
#include <iostream>
#include <memory>
#include <string>

/// @brief Defines how a UnaryOperator should be printed.
std::ostream &operator<<(std::ostream &os, const UnaryOperator &unaryOperator);
/// @brief Defines how a BinaryOperator should be printed.
std::ostream &operator<<(std::ostream &os, const BinaryOperator &binaryOperator);

/// @brief A Print Visitor.
/// @details
///  Pretty-prints a regular expression abstract syntax tree.
///
/// @author Bjarni Dagur Thor Kárason
class PrintVisitor : public Visitor
{
  public:
    /// @brief PrintVisitor constructor.
    PrintVisitor();
    void visitWordsNode(WordsNode *wordsNode) override;
    void visitLetterNode(LetterNode *letterNode) override;
    void visitMacroLetterNode(MacroLetterNode *macroLetterNode) override;
    void visitBinaryOpNode(BinaryOpNode *binaryOpNode) override;
    void visitUnaryOpNode(UnaryOpNode *unaryOpNode) override;
    /// @brief Construct indentation depending on the current depth in the abstract syntax tree.
    /// @returns A std::string with appropriate indentation for pretty-printing
    ///  nodes in the abstract syntax tree.
    std::string indent();

  private:
    /// @brief Keeps track of the current depth in the abstract syntax tree.
    int depth;
};
