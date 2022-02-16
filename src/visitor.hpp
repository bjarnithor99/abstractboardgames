// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

class SentenceNode;
class WordNode;
class UnaryWordNode;
class CoreWordNode;
class LetterNode;
class BinaryOpNode;
class UnaryOpNode;

class Visitor
{
  public:
    virtual ~Visitor(){};
    virtual void visitSentenceNode(SentenceNode *sentenceNode) = 0;
    virtual void visitWordNode(WordNode *WordNode) = 0;
    virtual void visitUnaryWordNode(UnaryWordNode *unaryWordNode) = 0;
    virtual void visitCoreWordNode(CoreWordNode *coreWordNode) = 0;
    virtual void visitLetterNode(LetterNode *letterNode) = 0;
    virtual void visitBinaryOpNode(BinaryOpNode *binaryOpNode) = 0;
    virtual void visitUnaryOpNode(UnaryOpNode *unaryOpNode) = 0;
};
