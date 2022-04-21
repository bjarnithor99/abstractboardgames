// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
/**
 *  @file nfa_visitor.hpp
 *  @brief A NFA Visitor for regular expression abstract syntax trees.
 *  @author Bjarni Dagur Thor Kárason
 *  @see ast.hpp
 *  @see nfa.hpp
 */
#pragma once

#include "ast.hpp"
#include "nfa.hpp"
#include "visitor.hpp"
#include <stack>

/// @brief A wrapper around a NFA.
/// @details
///  Keeps track of the initial and end states.
///
/// @pre All NFAs have a single initial state and a single final state.
///
/// @author Bjarni Dagur Thor Kárason
class NFAWrapper
{
  public:
    /// @brief NFAWrapper constructor.
    ///
    /// @param initial_state the initial state of an NFA
    /// @param final_state the final state of the NFA.
    NFAWrapper(NFAState *initial_state, NFAState *final_state);
    /// @brief NFAWrapper destructor.
    ~NFAWrapper();
    /// @brief The initial state of the wrapped NFA.
    NFAState *initial_state;
    /// @brief The final state of the wrapped NFA.
    NFAState *final_state;
};

/// @brief A NFA Visitor.
/// @details
///  Converts a regular expression abstract syntax tree to a NFA.
/// @author Bjarni Dagur Thor Kárason
class NFAVisitor : public Visitor
{
  public:
    /// @brief NFAVisitor constructor.
    NFAVisitor();
    /// @brief NFAVisitor destructor.
    ~NFAVisitor();
    void visitWordsNode(WordsNode *wordsNode) override;
    void visitLetterNode(LetterNode *letterNode) override;
    void visitMacroLetterNode(MacroLetterNode *macroLetterNode) override;
    void visitBinaryOpNode(BinaryOpNode *binaryOpNode) override;
    void visitUnaryOpNode(UnaryOpNode *unaryOpNode) override;
    /// @brief Returns the resulting NFA.
    ///
    /// @note Since the NFA is returned as a std::unique_ptr this function can
    ///  only be called once.
    ///
    /// @returns The initial state of a NFA equivalent to the visited
    ///  regular expression abstract syntax tree.
    std::unique_ptr<NFAState, NFAStateDeleter> getNFA();

  private:
    /// @brief Stores intermediate NFAs during NFAVisitor construction.
    std::stack<std::unique_ptr<NFAWrapper>> nfa_stack;
};
