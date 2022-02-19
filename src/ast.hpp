// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

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
    LetterNode(int dx, int dy, std::string predicate);
    ~LetterNode();
    void accept(Visitor *visitor) override;
    int dx;
    int dy;
    std::string predicate;
};

class WordsNode : public Node
{
  public:
    WordsNode();
    ~WordsNode();
    void accept(Visitor *visitor) override;
    void add_word_node(Node *wordNode);
    std::vector<Node *> wordNodes;
};

class UnaryOpNode : public Node
{
  public:
    UnaryOpNode(UnaryOperator unaryOperator, Node *childNode);
    ~UnaryOpNode();
    void accept(Visitor *visitor) override;
    UnaryOperator unaryOperator;
    Node *childNode;
};

class BinaryOpNode : public Node
{
  public:
    BinaryOpNode(BinaryOperator binaryOperator, Node *childNodeLHS, Node *childNodeRHS);
    ~BinaryOpNode();
    void accept(Visitor *visitor) override;
    BinaryOperator binaryOperator;
    Node *childNodeLHS;
    Node *childNodeRHS;
};
