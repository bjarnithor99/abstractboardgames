// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "ast.hpp"

LetterNode::LetterNode(int dx, int dy, std::string predicate) : dx(dx), dy(dy), predicate(predicate) {}
LetterNode::~LetterNode() {}
void LetterNode::accept(Visitor *visitor) {
    visitor->visitLetterNode(this);
}

CoreWordNode::CoreWordNode(Node *childNode) : childNode(childNode) {}
CoreWordNode::~CoreWordNode() {
    delete childNode;
}
void CoreWordNode::accept(Visitor *visitor) {
    visitor->visitCoreWordNode(this);
}

UnaryWordNode::UnaryWordNode(Node *childNode) : childNode(childNode) {}
UnaryWordNode::~UnaryWordNode() {
    delete childNode;
}
void UnaryWordNode::accept(Visitor *visitor) {
    visitor->visitUnaryWordNode(this);
}

WordNode::WordNode(Node *childNode) : childNode(childNode) {}
WordNode::~WordNode() {
    delete childNode;
}
void WordNode::accept(Visitor *visitor) {
    visitor->visitWordNode(this);
}

SentenceNode::SentenceNode() {}
SentenceNode::~SentenceNode() {
    for (WordNode *wordNode : wordNodes) {
        delete wordNode;
    }
}
void SentenceNode::accept(Visitor *visitor) {
    visitor->visitSentenceNode(this);
}
void SentenceNode::add_word_node(WordNode *wordNode) {
    wordNodes.push_back(wordNode);
}

UnaryOpNode::UnaryOpNode(UnaryOperator unaryOperator, Node *childNode)
    : unaryOperator(unaryOperator), childNode(childNode) {}
UnaryOpNode::~UnaryOpNode() {
    delete childNode;
}
void UnaryOpNode::accept(Visitor *visitor) {
    visitor->visitUnaryOpNode(this);
}

BinaryOpNode::BinaryOpNode(BinaryOperator binaryOperator, Node *childNodeLHS, Node *childNodeRHS)
    : binaryOperator(binaryOperator), childNodeLHS(childNodeLHS), childNodeRHS(childNodeRHS) {}
BinaryOpNode::~BinaryOpNode() {
    delete childNodeLHS;
    delete childNodeRHS;
}
void BinaryOpNode::accept(Visitor *visitor) {
    visitor->visitBinaryOpNode(this);
}
