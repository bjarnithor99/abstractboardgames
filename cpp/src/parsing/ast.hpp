// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include "predicates.hpp"
#include "side_effects.hpp"
#include "visitor.hpp"
#include <memory>
#include <string>
#include <vector>

enum class UnaryOperator
{
    OpStar,
    OpQuestion,
    OpPlus,
};

enum class BinaryOperator
{
    OpOr,
};

class Node
{
  public:
    virtual ~Node(){};
    virtual void accept(Visitor *visitor) = 0;
};

class LetterNode : public Node
{
  public:
    LetterNode(int dx, int dy, std::shared_ptr<Predicate> predicate, std::shared_ptr<SideEffect> side_effect);
    ~LetterNode();
    void accept(Visitor *visitor) override;
    int dx;
    int dy;
    std::shared_ptr<Predicate> predicate;
    std::shared_ptr<SideEffect> side_effect;
};

class MacroLetterNode : public Node
{
  public:
    MacroLetterNode(std::string dx, std::string dy, std::shared_ptr<Predicate> predicate,
                    std::shared_ptr<SideEffect> side_effect);
    ~MacroLetterNode();
    void accept(Visitor *visitor) override;
    std::string dx;
    std::string dy;
    std::shared_ptr<Predicate> predicate;
    std::shared_ptr<SideEffect> side_effect;
};

class WordsNode : public Node
{
  public:
    WordsNode();
    ~WordsNode();
    void accept(Visitor *visitor) override;
    void add_word_node(std::unique_ptr<Node> wordNode);
    std::vector<std::unique_ptr<Node>> wordNodes;
};

class UnaryOpNode : public Node
{
  public:
    UnaryOpNode(UnaryOperator unaryOperator, std::unique_ptr<Node> childNode);
    ~UnaryOpNode();
    void accept(Visitor *visitor) override;
    UnaryOperator unaryOperator;
    std::unique_ptr<Node> childNode;
};

class BinaryOpNode : public Node
{
  public:
    BinaryOpNode(BinaryOperator binaryOperator, std::unique_ptr<Node> childNodeLHS, std::unique_ptr<Node> childNodeRHS);
    ~BinaryOpNode();
    void accept(Visitor *visitor) override;
    BinaryOperator binaryOperator;
    std::unique_ptr<Node> childNodeLHS;
    std::unique_ptr<Node> childNodeRHS;
};
