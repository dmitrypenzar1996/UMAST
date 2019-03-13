/*  Copyright 2016, 2017 Andrey Sigorskikh, Sergei Spirin

    This file is part of UMAST.

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

#ifndef _UMAST_H_
#define _UMAST_H_
#include "Branch.h"
#include "BranchAllocator.h"
#include "BranchArray.h"
#include "Tree.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

const int nodeTypeCount = 6;

Tree* deleteLeaves(Tree* tree, char** leavesToDelete, int leavesToDeleteAmount);
unsigned* treeRootAndTopSort(Tree* tree, unsigned nodeID, unsigned neighbourID, unsigned* setPermutation);
int* calculateLeavesPermutation(Tree* tree1, Tree* tree2);
void countVariants(Branch*** TAB, int** L_TAB, int** K_TAB, 
        BranchArray* brAr1, BranchArray* brAr2, 
        int a, int w, int b, int c, int x, int y,
    int* bestValue, int* bestPos, int* bestLval);

int** getAllRoots(Tree* tree);
int* getRandMaxBranch(Branch*** TAB, int rows, int cols);
Tree* makeMAST(Branch* br, Tree* tree1);
int MAST(Tree* intree1, 
        Tree* intree2, 
        Branch** common_set,
        unsigned* set1,
        unsigned* set2,
        unsigned* setPermutation1,
        unsigned* setPermutation2);
void UMAST(Tree* tree1, Tree* tree2);
#endif
