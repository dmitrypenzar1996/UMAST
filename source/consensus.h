/*  Copyright 2016, 2017 Andrew Sigorskih, Dmitry Penzar, Sergei Spirin 

    This file is part of UMAST, taken from our another program PQ.

    UMAST is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    UMAST is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with UMAST (a file named "COPYING.txt"). 
    If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __CONSENSUS__H
#define __CONSENSUS__H
#include "add.h"
#include "RMQ.h"
#include "Tree.h"
#include "stdlib.h"

extern const char LOGUNS[4097];
static const char intSize = sizeof(INT) * 8;


typedef struct
{
    unsigned size;
    INT* branch;
}Branch;


size_t branchGetIntSize(Branch* br);
Branch* branchCreate(unsigned size);
void branchDelete(Branch* branch);
unsigned countZeroRightNum_(INT p);
unsigned countZeroRightNum(INT p);
size_t* branchGetLeavesPos(Branch* br, size_t* leavesNum, size_t maxNum);
int branchCompare(Branch* br1, Branch* br2);
int vBranchCompare(const void* branch1, const void* branch2);
void branchNormalize(Branch* br);
char* branchToString(Branch* br);
char branchContradict(Branch* br1, Branch* br2);
char branchIsSubset(Branch* br1, Branch* br2);
Branch* branchAnd(Branch* br1, Branch* br2);
Branch* branchOr(Branch* br1, Branch* br2);
Branch* branchReverse(Branch* br);
Branch* branchCopy(Branch* br);
char branchIsZero(Branch* br);

typedef struct 
{
    Branch** array;
    unsigned size;
    unsigned maxSize;
}BranchArray;

BranchArray* branchArrayCreate(unsigned startSize);
void branchArrayDelete(BranchArray* ba);
void branchArrayAdd(BranchArray* ba, Branch* branch);
void branchArraySort(BranchArray* ba);
void branchArrayExtend(BranchArray* dest, BranchArray* source);
BranchArray* treeToBranch(Tree* tree, int* permutation);

typedef struct BranchWithOccurence
{
    Branch* branch;
    unsigned occurence; 
}BranchOcc;

BranchOcc* branchOccCreate(Branch* branch, unsigned occurence);
void branchOccDelete(BranchOcc* brc);
void branchPrint(Branch* br);
int branchOccCompare(BranchOcc* brc1, BranchOcc* brc2);
int vBranchOccCompare(const void* brc1, const void* brc2);
int branchOccCompareByBranch(BranchOcc* brc1, BranchOcc* brc2);
int vBranchOccCompareByBranch(const void* brc1, const void* brc2);

typedef struct
{
    BranchOcc** array;
    unsigned size;
    unsigned maxSize;
}BranchCounter;

BranchCounter* branchCounterCreate(size_t startMaxSize);
void branchCounterDelete(BranchCounter* bc);
void branchCounterAdd(BranchCounter* bc, Branch* br, size_t branchOccurence);
void branchCounterInc(BranchCounter* bc, unsigned pos);
BranchCounter* branchCount(BranchArray* ba);
void branchCounterSort(BranchCounter* brc);
void branchCounterSortByBranch(BranchCounter* brc);
BranchCounter* majorityRule(BranchCounter* bc, unsigned threshold);
BranchCounter* majorityExtendedRule(BranchCounter* bc, unsigned threshold);
Tree* branchCounterToTree(BranchCounter* bc, char** names);

typedef struct ParserNode
{
    Node* treeNode;
    BranchOcc* branchOcc;
    struct ParserNode* left;
    struct ParserNode* right;
    struct ParserNode* parent;
    Color color;
}ParserNode;

typedef struct
{
    ParserNode* root;
    size_t size;
}ParserTree;

ParserNode* parserNodeCreate(BranchOcc* branchOcc);
void parserNodeDelete(ParserNode* nd);
ParserTree* parserTreeCreate();
void parserTreeDelete(ParserTree* tree);
void parserTreePrint(ParserTree* tree);
Tree* makeConsensus(Tree** treeArray, size_t treeNum, double threshold,
        char extended); 
#endif
