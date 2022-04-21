// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
/**
 *  @file visitor.hpp
 *  @brief A class to implement the Visitor pattern.
 *  @author Bjarni Dagur Thor Kárason
 *  @see print_visitor.hpp
 *  @see nfa_visitor.hpp
 *  @see macro_visitor.hpp
 */
#pragma once

class WordsNode;
class LetterNode;
class MacroLetterNode;
class BinaryOpNode;
class UnaryOpNode;

/// @brief An abstract Visitor class.
/// @author Bjarni Dagur Thor Kárason
class Visitor
{
  public:
    /// @brief Visitor destructor.
    virtual ~Visitor(){};
    /// @brief A Visitor function for a WordsNode.
    virtual void visitWordsNode(WordsNode *wordsNode) = 0;
    /// @brief A Visitor function for a LetterNode.
    virtual void visitLetterNode(LetterNode *letterNode) = 0;
    /// @brief A Visitor function for a MacroLetterNode.
    virtual void visitMacroLetterNode(MacroLetterNode *macroLetterNode) = 0;
    /// @brief A Visitor function for a BinaryOpNode.
    virtual void visitBinaryOpNode(BinaryOpNode *binaryOpNode) = 0;
    /// @brief A Visitor function for a UnaryOpNode.
    virtual void visitUnaryOpNode(UnaryOpNode *unaryOpNode) = 0;
};
