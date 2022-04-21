// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
/**
 *  @file macro_visitor.hpp
 *  @brief A Macro Visitor for macro'd regular expression abstract syntax trees.
 *  @author Bjarni Dagur Thor Kárason
 *  @see ast.hpp
 */
#pragma once

#include "ast.hpp"
#include "visitor.hpp"
#include <cassert>
#include <map>
#include <stack>

/// @brief A Macro Visitor.
/// @details
///  Expands macro calls in game descriptions with correct argument values.
///
/// @author Bjarni Dagur Thor Kárason
class MacroVisitor : public Visitor
{
  public:
    /// @brief MacroVisitor constructor.
    ///
    /// @param in_macro true if the macro being expanded is a part of another macro, false otherwise.
    /// @param argument_names the names of the macro arguments.
    /// @param argument_values the values of the macro arguments encoded as std::string.
    MacroVisitor(bool in_macro, std::vector<std::string> argument_names, std::vector<std::string> argument_values);
    /// @brief MacroVisitor destructor.
    ~MacroVisitor();
    void visitWordsNode(WordsNode *wordsNode) override;
    void visitLetterNode(LetterNode *letterNode) override;
    void visitMacroLetterNode(MacroLetterNode *macroLetterNode) override;
    void visitBinaryOpNode(BinaryOpNode *binaryOpNode) override;
    void visitUnaryOpNode(UnaryOpNode *unaryOpNode) override;
    /// @brief Returns the expanded macro.
    ///
    /// @note Since the root of the abstract syntax tree is returned as a
    ///  std::unique_ptr this function can only be called once.
    ///
    /// @returns the root of the abstract syntax tree of the macro'd regular
    ///  expression with correct argument values.
    std::unique_ptr<Node> get_node();

  private:
    /// @brief True if the macro being expanded is a part of another macro, false otherwise.
    bool in_macro;
    /// @brief Maps argument names to their std::string encoded values.
    std::map<std::string, std::string> argument_value;
    /// @brief Stores intermediate abstract syntax sub-trees during MacroVisitor construction.
    std::stack<std::unique_ptr<Node>> node_stack;
};
