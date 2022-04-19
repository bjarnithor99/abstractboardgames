// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "macro_visitor.hpp"

MacroVisitor::MacroVisitor(bool in_macro, std::vector<std::string> argument_names,
                           std::vector<std::string> argument_values)
    : in_macro(in_macro) {
    assert(argument_names.size() == argument_values.size());
    for (size_t i = 0; i < argument_names.size(); i++) {
        argument_value[argument_names[i]] = argument_values[i];
    }
}
MacroVisitor::~MacroVisitor() {}

void MacroVisitor::visitWordsNode(WordsNode *wordsNode) {
    std::unique_ptr<WordsNode> node = std::make_unique<WordsNode>();
    for (const std::unique_ptr<Node> &wordNode : wordsNode->wordNodes) {
        wordNode->accept(this);
        node->add_word_node(std::move(node_stack.top()));
        node_stack.pop();
    }
    node_stack.push(std::move(node));
}

void MacroVisitor::visitLetterNode(LetterNode *letterNode) {
    throw std::runtime_error("Only MacroLetterNodes are allowed in macro bodies, not LetterNodes.");
}

void MacroVisitor::visitMacroLetterNode(MacroLetterNode *macroLetterNode) {
    if (in_macro) {
        std::string dx;
        if (argument_value.find(macroLetterNode->dx) != argument_value.end())
            dx = argument_value[macroLetterNode->dx];
        else
            dx = macroLetterNode->dx;

        std::string dy;
        if (argument_value.find(macroLetterNode->dy) != argument_value.end())
            dy = argument_value[macroLetterNode->dy];
        else
            dy = macroLetterNode->dy;

        node_stack.push(
            std::make_unique<MacroLetterNode>(dx, dy, macroLetterNode->predicate, macroLetterNode->side_effect));
    }
    else {
        int dx;
        if (argument_value.find(macroLetterNode->dx) != argument_value.end())
            dx = std::stoi(argument_value[macroLetterNode->dx]);
        else
            dx = std::stoi(macroLetterNode->dx);

        int dy;
        if (argument_value.find(macroLetterNode->dy) != argument_value.end())
            dy = std::stoi(argument_value[macroLetterNode->dy]);
        else
            dy = std::stoi(macroLetterNode->dy);

        node_stack.push(std::make_unique<LetterNode>(dx, dy, macroLetterNode->predicate, macroLetterNode->side_effect));
    }
}

void MacroVisitor::visitBinaryOpNode(BinaryOpNode *binaryOpNode) {
    binaryOpNode->childNodeLHS->accept(this);
    binaryOpNode->childNodeRHS->accept(this);
    std::unique_ptr<Node> childNodeRHS = std::move(node_stack.top());
    node_stack.pop();
    std::unique_ptr<Node> childNodeLHS = std::move(node_stack.top());
    node_stack.pop();
    node_stack.push(
        std::make_unique<BinaryOpNode>(binaryOpNode->binaryOperator, std::move(childNodeLHS), std::move(childNodeRHS)));
}

void MacroVisitor::visitUnaryOpNode(UnaryOpNode *unaryOpNode) {
    unaryOpNode->childNode->accept(this);
    std::unique_ptr<Node> childNode = std::move(node_stack.top());
    node_stack.pop();
    node_stack.push(std::make_unique<UnaryOpNode>(unaryOpNode->unaryOperator, std::move(childNode)));
}

std::unique_ptr<Node> MacroVisitor::get_node() {
    std::unique_ptr<Node> node(std::move(node_stack.top()));
    node_stack.pop();
    return node;
}
