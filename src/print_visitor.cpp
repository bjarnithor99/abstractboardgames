// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "print_visitor.hpp"

std::ostream &operator<<(std::ostream &os, const UnaryOperator &unaryOperator) {
    switch (unaryOperator) {
    case UnaryOperator::None:
        os << "None";
        break;
    case UnaryOperator::OpStar:
        os << "*";
        break;
    case UnaryOperator::OpQuestion:
        os << "?";
        break;
    case UnaryOperator::OpPlus:
        os << "+";
        break;
    default:
        os << "Not implemented";
        break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const BinaryOperator &binaryOperator) {
    switch (binaryOperator) {
    case BinaryOperator::None:
        os << "None";
        break;
    case BinaryOperator::OpOr:
        os << "|";
        break;
    default:
        os << "Not implemented";
        break;
    }
    return os;
}

PrintVisitor::PrintVisitor() : depth(0) {}

void PrintVisitor::visitSentenceNode(SentenceNode *sentenceNode) {
    std::cout << indent() << "SentenceNode" << std::endl;
    depth++;
    for (WordNode *wordNode : sentenceNode->wordNodes) {
        wordNode->accept(this);
    }
    depth--;
}

void PrintVisitor::visitWordNode(WordNode *wordNode) {
    std::cout << indent() << "WordNode" << std::endl;
    depth++;
    wordNode->childNode->accept(this);
    depth--;
}

void PrintVisitor::visitUnaryWordNode(UnaryWordNode *unaryWordNode) {
    std::cout << indent() << "UnaryWordNode" << std::endl;
    depth++;
    unaryWordNode->childNode->accept(this);
    depth--;
}

void PrintVisitor::visitCoreWordNode(CoreWordNode *coreWordNode) {
    std::cout << indent() << "CoreWordNode" << std::endl;
    depth++;
    coreWordNode->childNode->accept(this);
    depth--;
}

void PrintVisitor::visitLetterNode(LetterNode *letterNode) {
    std::cout << indent() << "LetterNode (" << letterNode->dx << ", " << letterNode->dy << ", " << letterNode->predicate
              << ")" << std::endl;
}

void PrintVisitor::visitBinaryOpNode(BinaryOpNode *binaryOpNode) {
    std::cout << indent() << "BinaryOpNode (" << binaryOpNode->binaryOperator << ")" << std::endl;
    depth++;
    binaryOpNode->childNodeLHS->accept(this);
    binaryOpNode->childNodeRHS->accept(this);
    depth--;
}

void PrintVisitor::visitUnaryOpNode(UnaryOpNode *unaryOpNode) {
    std::cout << indent() << "UnaryOpNode (" << unaryOpNode->unaryOperator << ")" << std::endl;
    depth++;
    unaryOpNode->childNode->accept(this);
    depth--;
}

std::string PrintVisitor::indent() {
    std::string indent = "";
    for (int i = 0; i < depth; i++) {
        indent += "   ";
    }
    return indent;
}
