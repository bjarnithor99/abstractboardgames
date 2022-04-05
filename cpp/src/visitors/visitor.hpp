// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#pragma once

class WordsNode;
class LetterNode;
class BinaryOpNode;
class UnaryOpNode;

class Visitor
{
  public:
    virtual ~Visitor(){};
    virtual void visitWordsNode(WordsNode *wordsNode) = 0;
    virtual void visitLetterNode(LetterNode *letterNode) = 0;
    virtual void visitBinaryOpNode(BinaryOpNode *binaryOpNode) = 0;
    virtual void visitUnaryOpNode(UnaryOpNode *unaryOpNode) = 0;
};
