// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "ast.hpp"

LetterNode::LetterNode(int dx, int dy, std::string predicate, std::string side_effect)
    : dx(dx), dy(dy), predicate(predicate), side_effect(side_effect) {}
LetterNode::~LetterNode() {}
void LetterNode::accept(Visitor *visitor) {
    visitor->visitLetterNode(this);
}

WordsNode::WordsNode() {}
WordsNode::~WordsNode() {
    for (Node *wordNode : wordNodes) {
        delete wordNode;
    }
}
void WordsNode::accept(Visitor *visitor) {
    visitor->visitWordsNode(this);
}
void WordsNode::add_word_node(Node *wordNode) {
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
