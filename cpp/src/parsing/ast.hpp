// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
/**
 *  @file ast.hpp
 *  @brief Nodes for an abstract syntax tree representing regular expression.
 *  @author Bjarni Dagur Thor Kárason
 */
#pragma once

#include "predicates.hpp"
#include "side_effects.hpp"
#include "visitor.hpp"
#include <memory>
#include <string>
#include <vector>

/// @brief Unary regular expression operator.
/// @details
///  Used in UnaryOpNode to compose regular expressions.
///
/// @see UnaryOpNode
enum class UnaryOperator
{
    /// @brief 0 or more occurences (Kleene star).
    OpStar,
    /// @brief 0 or 1 occurences.
    OpQuestion,
    /// @brief 1 or more occurences.
    OpPlus,
};

/// @brief Binary regular expression operator.
/// @details
///  Used in BinaryOpNode to compose regular expressions.
///
/// @see BinaryOpNode
enum class BinaryOperator
{
    /// @brief Either the left-hand side or right-hand side should hold for the whole
    ///  expression to hold.
    OpOr,
};

/// @brief Class to represent nodes in an abstract syntax tree.
/// @author Bjarni Dagur Thor Kárason
class Node
{
  public:
    /// @brief Class destructor.
    virtual ~Node(){};
    /// @brief Accept a Visitor.
    ///
    /// @param visitor pointer the visiting visitor.
    ///
    /// @see Visitor
    /// @see PrintVisitor
    /// @see NFAVisitor
    /// @see MacroVisitor
    virtual void accept(Visitor *visitor) = 0;
};

/// @brief Node class to represent a regular expression letter.
/// @details
///  A LetterNode stores information about a possible move of an abstract board
///  game piece. The move description is stored as movement along x and y axes
///  in a Cartesian coordinate system, a Predicate describing when the move is
///  legal, and a SideEffect of the move.
///
/// @see Environment
/// @see Predicate
/// @see SideEffect
///
/// @author Bjarni Dagur Thor Kárason
class LetterNode : public Node
{
  public:
    /// @brief LetterNode constructor.
    /// @details Constructs a LetterNode encoding a possible piece move.
    ///
    /// @param dx the move's delta along the x axis.
    /// @param dy the move's delta along the y axis.
    /// @param predicate a Predicate functor describing when the move is legal.
    /// @param side_effect a SideEffect functor describing the move's side effects.
    LetterNode(int dx, int dy, std::shared_ptr<Predicate> predicate, std::shared_ptr<SideEffect> side_effect);
    ~LetterNode();
    void accept(Visitor *visitor) override;
    /// @brief the encoded move's delta along the x axis.
    int dx;
    /// @brief the encoded move's delta along the y axis.
    int dy;
    /// @brief the encoded move's Predicate.
    std::shared_ptr<Predicate> predicate;
    /// @brief the encoded move's SideEffect.
    std::shared_ptr<SideEffect> side_effect;
};

/// @brief Node class to represent a macro'd regular expression letter.
/// @details
///  Similarly to a LetterNode, a MacroLetterNode stores information about a
///  possible move of an abstract board game piece, but some information can be
///  parameterized. The move description is stored as movement along x and y
///  axes in a Cartesian coordinate system, a Predicate describing when the move
///  is legal, and a SideEffect of the move.
///
///  Macro'd regular expressions can be used like functions in game
///  descriptionsa and are resolved during parsing.
///
/// @see LetterNode
/// @see Environment
/// @see Predicate
/// @see SideEffect
///
/// @author Bjarni Dagur Thor Kárason
class MacroLetterNode : public Node
{
  public:
    /// @brief MacroLetterNode constructor.
    /// @details Constructs a MacroLetterNode encoding a possible piece move.
    ///
    /// @param dx the move's delta along the x axis, possibly parameterized.
    /// @param dy the move's delta along the y axis, possible parameterized.
    /// @param predicate a Predicate functor describing when the move is legal.
    /// @param side_effect a SideEffect functor describing the move's side effects.
    MacroLetterNode(std::string dx, std::string dy, std::shared_ptr<Predicate> predicate,
                    std::shared_ptr<SideEffect> side_effect);
    ~MacroLetterNode();
    void accept(Visitor *visitor) override;
    /// @brief the encoded move's delta along the x axis, possibly parameterized.
    std::string dx;
    /// @brief the encoded move's delta along the y axis, possibly parameterized.
    std::string dy;
    /// @brief the encoded move's Predicate.
    std::shared_ptr<Predicate> predicate;
    /// @brief the encoded move's SideEffect.
    std::shared_ptr<SideEffect> side_effect;
};

/// @brief Node class to represent concatenated regular expressions.
/// @details
///  Represents a regular expression composed of concatenated sub-expressions by
///  storing the nodes representing the sub-expressions in order.
///
/// @see UnaryOpNode
/// @see BinaryOpNode
/// @see LetterNode
/// @see MacroLetterNode
///
/// @author Bjarni Dagur Thor Kárason
class WordsNode : public Node
{
  public:
    /// @brief WordsNode constructor.
    WordsNode();
    /// @brief WordsNode destructor.
    ~WordsNode();
    void accept(Visitor *visitor) override;
    /// @brief Add a Node to the concatenation.
    ///
    /// @param wordNode the Node to add to the concatenation
    void add_word_node(std::unique_ptr<Node> wordNode);
    /// @brief The concatenated sub-expression Nodes in order.
    std::vector<std::unique_ptr<Node>> wordNodes;
};

/// @brief Node class to represent a regular expression composed with a unary operator.
///
/// @see UnaryOperator
/// @see WordsNode
/// @see BinaryOpNode
/// @see LetterNode
/// @see MacroLetterNode
///
/// @author Bjarni Dagur Thor Kárason
class UnaryOpNode : public Node
{
  public:
    /// @brief UnaryOpNode constructor.
    /// @details
    ///  Constructs a UnaryOpNode encoding a sub-expression composed with a unary operator.
    ///
    /// @param unaryOperator the UnaryOperator applied to the sub-expression.
    /// @param childNode the sub-expression Node to compose.
    UnaryOpNode(UnaryOperator unaryOperator, std::unique_ptr<Node> childNode);
    /// @brief UnaryOpNode destructor.
    ~UnaryOpNode();
    void accept(Visitor *visitor) override;
    /// @brief The UnaryOperator applied the sub-expression.
    UnaryOperator unaryOperator;
    /// @brief The sub-expression to compose.
    std::unique_ptr<Node> childNode;
};

/// @brief Node class to represent a regular expression composed with a binary operator.
///
/// @see BinaryOperator
/// @see WordsNode
/// @see UnaryOpNode
/// @see LetterNode
/// @see MacroLetterNode
///
/// @author Bjarni Dagur Thor Kárason
class BinaryOpNode : public Node
{
  public:
    /// @brief BinaryOpNode constructor.
    /// @details
    ///  Constructs a BinaryOpNode encoding two sub-expressions composed with a binary operator.
    ///
    /// @param binaryOperator the BinaryOperator applied to the sub-expression.
    /// @param childNodeLHS the left-hand side sub-expression Node to compose.
    /// @param childNodeRHS the right-hand side sub-expression Node to compose.
    BinaryOpNode(BinaryOperator binaryOperator, std::unique_ptr<Node> childNodeLHS, std::unique_ptr<Node> childNodeRHS);
    /// @brief UnaryOpNode destructor.
    ~BinaryOpNode();
    void accept(Visitor *visitor) override;
    /// @brief The BinaryOperator applied to the sub-expressions.
    BinaryOperator binaryOperator;
    /// @brief The left-hand side sub-expression Node to compose.
    std::unique_ptr<Node> childNodeLHS;
    /// @brief The right-hand side sub-expression Node to compose.
    std::unique_ptr<Node> childNodeRHS;
};
