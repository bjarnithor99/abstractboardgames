// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "ast.hpp"

LetterNode::LetterNode(int dx, int dy, std::shared_ptr<Predicate> predicate, std::shared_ptr<SideEffect> side_effect)
    : dx(dx), dy(dy), predicate(predicate), side_effect(side_effect) {}
LetterNode::~LetterNode() {}
void LetterNode::accept(Visitor *visitor) {
    visitor->visitLetterNode(this);
}

MacroLetterNode::MacroLetterNode(std::string dx, std::string dy, std::shared_ptr<Predicate> predicate,
                                 std::shared_ptr<SideEffect> side_effect)
    : dx(dx), dy(dy), predicate(predicate), side_effect(side_effect) {}
MacroLetterNode::~MacroLetterNode() {}
void MacroLetterNode::accept(Visitor *visitor) {
    visitor->visitMacroLetterNode(this);
}

WordsNode::WordsNode() {}
WordsNode::~WordsNode() {}
void WordsNode::accept(Visitor *visitor) {
    visitor->visitWordsNode(this);
}
void WordsNode::add_word_node(std::unique_ptr<Node> wordNode) {
    wordNodes.push_back(std::move(wordNode));
}

UnaryOpNode::UnaryOpNode(UnaryOperator unaryOperator, std::unique_ptr<Node> childNode)
    : unaryOperator(unaryOperator), childNode(std::move(childNode)) {}
UnaryOpNode::~UnaryOpNode() {}
void UnaryOpNode::accept(Visitor *visitor) {
    visitor->visitUnaryOpNode(this);
}

BinaryOpNode::BinaryOpNode(BinaryOperator binaryOperator, std::unique_ptr<Node> childNodeLHS,
                           std::unique_ptr<Node> childNodeRHS)
    : binaryOperator(binaryOperator), childNodeLHS(std::move(childNodeLHS)), childNodeRHS(std::move(childNodeRHS)) {}
BinaryOpNode::~BinaryOpNode() {}
void BinaryOpNode::accept(Visitor *visitor) {
    visitor->visitBinaryOpNode(this);
}
