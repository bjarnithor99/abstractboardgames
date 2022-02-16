// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

#include <memory>
#include <string>
#include <vector>

enum class UnaryOperator
{
    None,
    OpStar,
    OpQuestion,
    OpPlus,
};

enum class BinaryOperator
{
    None,
    OpOr,
};

class Node
{
  public:
    virtual ~Node(){};
};

class LetterNode : public Node
{
  public:
    LetterNode(int dx, int dy, std::string predicate);
    ~LetterNode();
    int dx;
    int dy;
    std::string predicate;
};

class CoreWordNode : public Node
{
  public:
    CoreWordNode(Node *childNode);
    ~CoreWordNode();
    Node *childNode;
};

class UnaryWordNode : public Node
{
  public:
    UnaryWordNode(Node *childNode);
    ~UnaryWordNode();
    Node *childNode;
};

class WordNode : public Node
{
  public:
    WordNode(Node *childNode);
    ~WordNode();
    Node *childNode;
};

class SentenceNode : public Node
{
  public:
    SentenceNode();
    ~SentenceNode();
    void add_word_node(WordNode *wordNode);
    std::vector<WordNode *> wordNodes;
};

class UnaryOpNode : public Node
{
  public:
    UnaryOpNode(UnaryOperator unaryOperator, Node *childNode);
    ~UnaryOpNode();
    UnaryOperator unaryOperator;
    Node *childNode;
};

class BinaryOpNode : public Node
{
  public:
    BinaryOpNode(BinaryOperator binaryOperator, Node *childNodeLHS, Node *childNodeRHS);
    ~BinaryOpNode();
    BinaryOperator binaryOperator;
    Node *childNodeLHS;
    Node *childNodeRHS;
};
