/*  Copyright 2016, 2017 Andrey Sigorskikh, Sergei Spirin
    Contact: sas@belozersky.msu.ru
    Homepage: http://mouse.belozersky.msu.ru/tools/umast.html

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
    along with UMAST in a file named "COPYING.txt".
    If not, see <http://www.gnu.org/licenses/>.
*/
#include "umast.h"

static const int nodeTypeCount = 6;

static Branch* UMASTStep(Tree* tree1, Tree* tree2, unsigned* set1, unsigned* set2,
    unsigned* setPermutation1, unsigned* setPermutation2, Branch**** TAB, int*** L_TAB,
    Branch** rootRow, int* L_rootRow,
    BranchAllocator* brAllocator) {
    int b, c, x, y;
    int i, j;
    int a, w;
    int* permutation; //permutation of leaf sets between tree1 and tree2
    int* leavesPosArr1; //size = nodes; for every node has -1, for leaf has its pos in leaves
    int* leavesPosArr2; //the same for tree2
    int variant;
    int bestCase;
    int bestValue;
    int* caseTable;
    BranchArray* branchArr1;
    BranchArray* branchArr2;
    Branch* intersection;
    Branch* temp;
    Node* nodeA;
    Node* nodeW;
    Branch** aRow;
    int* L_aRow;
    int leavesNum;
    int bestL;
    int k1;
    int k2;
    int k_b;
    int k_c;
    int k_x;
    int k_y;
    int curL;
    int p = 0;
    int p_b = 0;
    int p_c = 0;
    int p_x = 0;
    int p_y = 0;

    permutation = getRange(0, tree1->leavesNum);
    branchArr1 = treeRootedToBranchArray(tree1, permutation);
    free(permutation);
    permutation = calculateLeavesPermutation(tree2, tree1);
    branchArr2 = treeRootedToBranchArray(tree2, permutation);
    leavesPosArr1 = treeGetLeavesPos(tree1);
    leavesPosArr2 = treeGetLeavesPos(tree2);

    caseTable = calloc(tree1->nodesNum, sizeof(int));

    int* nodesNeedToCalculate = calloc(tree1->nodesNum, sizeof(int));

    for (i = 0; i < tree1->nodesNum - 1; ++i) {
        variant = findParent(i, tree1, setPermutation1);
        caseTable[i] = variant;
        if (TAB[variant][i][0] == NULL) {
            nodesNeedToCalculate[i] = 1;
        }
    }

    caseTable[tree1->nodesNum - 1] = 6;
    nodesNeedToCalculate[tree1->nodesNum - 1] = 1;
    for (i = 0; i < tree1->nodesNum; i++) //Until all lines in TAB are filled
    {
        a = set1[i];
        if (!nodesNeedToCalculate[a]){
            continue;
        }

        variant = caseTable[a];
        if (variant != 6) {
            aRow = TAB[caseTable[a]][a];
	    L_aRow = L_TAB[caseTable[a]][a];
        } else {
            aRow = rootRow;
	    L_aRow = L_rootRow;
        }

        nodeA = tree1->nodes[a];
        for (j = 0; j < tree2->nodesNum; j++) //Until all cells in line are filled
        {

            w = set2[j];

            nodeW = tree2->nodes[w];

            b = -1;
            c = -1;
            x = -1;
            y = -1;
            if ((nodeA->neiNum == 1) || (nodeW->neiNum == 1)) //if a or w is a leaf
            {
                if ((nodeA->neiNum == 1) && (nodeW->neiNum == 1)) //if a AND w are leaves
                {

                    if (strcmp(nodeA->name, nodeW->name) == 0) //a and w are equal leaves
                    {
                        intersection = branchAllocatorGetBranch(brAllocator);
                        branchAddLeafUnsafe(intersection, leavesPosArr1[a]);
                        aRow[w] = intersection;
			L_aRow[w] = 0;
                    } else //leaves are not equal
                    {
                        intersection = branchAllocatorGetBranch(brAllocator);
                        aRow[w] = intersection;
			L_aRow[w] = 2;
                    }
                } else //if a or w is a subtree
                {

                    intersection = branchAllocatorGetBranch(brAllocator);
                    if (nodeA->neiNum == 1) //a is a leaf, w is a subtree
                    {
                        branchAddLeafUnsafe(intersection, leavesPosArr1[a]);
                        branchAndDest(intersection, branchArr2->array[w], intersection);
                        aRow[w] = intersection;
			if (branchGetLeavesPosNum(intersection) == 1){
			    L_aRow[w] = branchGetLeavesPosNum(branchArr2->array[w]) - 1;
			}else{
			    L_aRow[w] = 1 + branchGetLeavesPosNum(branchArr2->array[w]);
			}
                    } else //a is a subtree, w is a leaf
                    {
                        branchAddLeafUnsafe(intersection, permutation[leavesPosArr2[w]]);
                        branchAndDest(intersection, branchArr1->array[a], intersection);
                        aRow[w] = intersection;
			if (branchGetLeavesPosNum(intersection) == 1){
                            L_aRow[w] = branchGetLeavesPosNum(branchArr1->array[a]) - 1;
                        }else{
                            L_aRow[w] = 1 + branchGetLeavesPosNum(branchArr1->array[a]);
                        }
                    }
                }

            } else //nor a nor w are leaves
            {
		INT tempInt;
		k1 = 0;
		k2 = 0;
		k_b = 0;
		k_c = 0;
		k_x = 0;
		k_y = 0;
		p = 0;
		p_b = 0;
		p_c = 0;
		p_x = 0;
		p_y = 0;


                findChilds(a, tree1, setPermutation1, &b, &c);
                findChilds(w, tree2, setPermutation2, &x, &y);

		Branch* br_b = branchArr1->array[b];
                Branch* br_c = branchArr1->array[c];
                Branch* br_x = branchArr2->array[x];
                Branch* br_y = branchArr2->array[y];
		Branch* br_w = branchArr2->array[w];
		Branch* br_a = branchArr1->array[a];


		int root_multiplier = 2;
		if (w == tree1->nodesNum - 1 || aRow == rootRow){
		    root_multiplier = 1; // root is fake so can be deleted thus reducing L distance
		}


		int brIntSize = branchGetIntSize(br_b);
		int t;
		for (t = 0; t < brIntSize; ++t) {
		    tempInt = (br_b->branch[t] & (~ (br_c->branch[t] | br_x->branch[t] | br_y->branch[t])));
                    p_b += countOnes(tempInt);
		    tempInt = (br_c->branch[t] & (~ (br_b->branch[t] | br_x->branch[t] | br_y->branch[t])));
		    p_c += countOnes(tempInt);
		    tempInt = (br_x->branch[t] & (~ (br_b->branch[t] | br_c->branch[t] | br_y->branch[t])));
		    p_x += countOnes(tempInt);
		    tempInt = (br_y->branch[t] & (~ (br_b->branch[t] | br_c->branch[t] | br_x->branch[t])));
		    p_y += countOnes(tempInt);
		}
		p = p_b + p_c + p_x + p_y;

		temp = branchAllocatorGetBranch(brAllocator);

		branchAndDest(br_b, br_y, temp);
		k1 = branchGetLeavesPosNum(temp);
		branchAndDest(br_c, br_x, temp);
		k1 += branchGetLeavesPosNum(temp);

		branchAndDest(br_b, br_x, temp);
		k2 = branchGetLeavesPosNum(temp);
		branchAndDest(br_c, br_y, temp);
		k2 += branchGetLeavesPosNum(temp);

		branchAndDest(br_b, br_w, temp);
		k_b = branchGetLeavesPosNum(temp);

		branchAndDest(br_c, br_w, temp);
		k_c = branchGetLeavesPosNum(temp);

		branchAndDest(br_x, br_a, temp);
		k_x = branchGetLeavesPosNum(temp);

		branchAndDest(br_y, br_a, temp);
		k_y = branchGetLeavesPosNum(temp);

                bestValue = branchGetLeavesPosNum(TAB[caseTable[b]][b][x]) + branchGetLeavesPosNum(TAB[caseTable[c]][c][y]);
		bestL = L_TAB[caseTable[b]][b][x] + L_TAB[caseTable[c]][c][y] + k1 * root_multiplier + p;
                bestCase = 0;
                int curValue = branchGetLeavesPosNum(TAB[caseTable[b]][b][y]) + branchGetLeavesPosNum(TAB[caseTable[c]][c][x]);
		curL = L_TAB[caseTable[b]][b][y] + L_TAB[caseTable[c]][c][x] + k2 * root_multiplier + p;

                if (curValue > bestValue || (curValue == bestValue && curL < bestL)) {
                    bestValue = curValue;
		    bestL = curL;
                    bestCase = 1;
                }

                curValue = branchGetLeavesPosNum(aRow[x]);
		curL = L_aRow[x] + k_y * root_multiplier + p_y;

                if (curValue > bestValue || (curValue == bestValue && curL < bestL)) {
                    bestValue = curValue;
		    bestL = curL;
                    bestCase = 2;
                }

                curValue = branchGetLeavesPosNum(aRow[y]);
		curL = L_aRow[y] + k_x * root_multiplier + p_x;

                if (curValue > bestValue || (curValue == bestValue && curL < bestL)) {
                    bestValue = curValue;
		    bestL = curL;
                    bestCase = 3;
                }

                curValue = branchGetLeavesPosNum(TAB[caseTable[b]][b][w]);
		curL = L_TAB[caseTable[b]][b][w] + k_c * root_multiplier + p_c;

                if (curValue > bestValue || (curValue == bestValue && curL < bestL)) {
                    bestValue = curValue;
		    bestL = curL;
                    bestCase = 4;
                }

                curValue = branchGetLeavesPosNum(TAB[caseTable[c]][c][w]);
		curL = L_TAB[caseTable[c]][c][w] + k_b * root_multiplier + p_b;

                if (curValue > bestValue || (curValue == bestValue && curL < bestL)) {
                    bestValue = curValue;
		    bestL = curL;
                    bestCase = 5;
                }

		L_aRow[w] = bestL;

                switch (bestCase) {
                case 0:
                    temp = branchAllocatorGetBranch(brAllocator);
                    branchOrDest(TAB[caseTable[b]][b][x], TAB[caseTable[c]][c][y], temp);
                    aRow[w] = temp;
                    break;
                case 1:
                    temp = branchAllocatorGetBranch(brAllocator);
                    branchOrDest(TAB[caseTable[b]][b][y], TAB[caseTable[c]][c][x], temp);
                    aRow[w] = temp;
                    break;
                case 2:
                    aRow[w] = aRow[x];
                    break;
                case 3:
                    aRow[w] = aRow[y];
                    break;
                case 4:
                    aRow[w] = TAB[caseTable[b]][b][w];
                    break;
                case 5:
                    aRow[w] = TAB[caseTable[c]][c][w];
                    break;
                default:
                    fprintf(stderr, "Critical error in main loop, umast:MAST\n");
                    exit(1);
                }
            }
        }
    }


    free(leavesPosArr1);
    free(leavesPosArr2);
    free(nodesNeedToCalculate);

    for (i = 0; i < branchArr1->size; ++i) {
        branchDelete(branchArr1->array[i]);
    }
    for (i = 0; i < branchArr2->size; ++i) {
        branchDelete(branchArr2->array[i]);
    }

    free(caseTable);
    free(permutation);
    branchArrayDelete(branchArr1);
    branchArrayDelete(branchArr2);

    return rootRow[tree1->nodesNum - 1]; // root always should be max
} //UMASTStep

/*
returns L distance and write MAST to mast_tree
*/
int UMAST(Tree* tree1, Tree* tree2, Tree** mast_tree){
    int i, j, k, rootNum;
    int** rootPositions;
    unsigned* sortedSet1; //topologically sorted nodes from rooted tree1
    unsigned* sortedSet2; //topologically sorted nodes from rooted tree2
    unsigned* setPermutation1; //for every node of tree1 tells its
    // pos in topologically sorted set
    unsigned* setPermutation2; //the same for tree2
    Tree* result;
    Branch** umastSet;
    int* L_umastSet;
    Branch* bestBranch;
    int bestL;
    int curL;
    int bestLeavesNum;
    Branch* curBranch;
    int curLeavesNum;
    BranchAllocator* brAllocator;

    rootNum = tree1->nodesNum - 1;
    brAllocator = branchAllocatorCreate(tree1->nodesNum * tree1->nodesNum * rootNum * nodeTypeCount * 2,
        tree1->leavesNum);

    rootPositions = getAllRoots(tree1);
    umastSet = (Branch**)calloc(sizeof(Branch*), rootNum);
    L_umastSet = (int*)calloc(sizeof(int), rootNum);

    tree2 = treeRoot(tree2, 1, 0, 0);
    sortedSet2 = treeTopologicalSort(tree2);
    setPermutation2 = calcUnsignedPermutation(sortedSet2, tree2->nodesNum);

    Branch**** TAB = (Branch****)calloc(nodeTypeCount, sizeof(Branch***));

    TAB[0] = (Branch***)calloc(nodeTypeCount * tree2->nodesNum, sizeof(Branch**));
    TAB[0][0] = (Branch**)calloc(nodeTypeCount * tree2->nodesNum * tree2->nodesNum,
        sizeof(Branch*));
    for (j = 1; j < tree2->nodesNum; ++j) {
        TAB[0][j] = TAB[0][j - 1] + tree2->nodesNum;
    }
    for (i = 1; i < nodeTypeCount; ++i) {
        TAB[i] = TAB[i - 1] + tree2->nodesNum;
        TAB[i][0] = TAB[i - 1][tree2->nodesNum - 1] + tree2->nodesNum;
        for (j = 1; j < tree2->nodesNum; ++j) {
            TAB[i][j] = TAB[i][j - 1] + tree2->nodesNum;
        }
    }

    int*** L_TAB = (int***)calloc(nodeTypeCount, sizeof(int**));
    L_TAB[0] = (int**)calloc(nodeTypeCount * tree2->nodesNum, sizeof(int*));
    L_TAB[0][0] = (int*)calloc(nodeTypeCount * tree2->nodesNum * tree2->nodesNum, sizeof(int));
    for (j = 1; j < tree2->nodesNum; ++j){
    L_TAB[0][j] = L_TAB[0][j - 1] + tree2->nodesNum;
    }

    for (i = 1; i < nodeTypeCount; ++i) {
        L_TAB[i] = L_TAB[i - 1] + tree2->nodesNum;
        L_TAB[i][0] = L_TAB[i - 1][tree2->nodesNum - 1] + tree2->nodesNum;
        for (j = 1; j < tree2->nodesNum; ++j) {
            L_TAB[i][j] = L_TAB[i][j - 1] + tree2->nodesNum;
        }
    }

    for (i = 0; i < nodeTypeCount; ++i){
    for (j = 0; j < tree2->nodesNum; ++j){
        for (k = 0; k < tree2->nodesNum; ++k){
        L_TAB[i][j][k] = 0;
        }
    }
    }


    Branch*** rootTable = (Branch***)calloc(sizeof(Branch**), rootNum);
    rootTable[0] = (Branch**)calloc(sizeof(Branch*), tree2->nodesNum * rootNum);
    for (i = 1; i < rootNum; ++i) {
        rootTable[i] = rootTable[i - 1] + tree2->nodesNum;
    }

    int** L_rootTable = (int**)calloc(sizeof(int*), rootNum);
    L_rootTable[0] = (int*)calloc(sizeof(int), tree2->nodesNum * rootNum);
    for (i = 1; i < rootNum; ++i){
    L_rootTable[i] = L_rootTable[i - 1] + tree2->nodesNum;
    }

    for (i = 0; i < rootNum; ++i){
        for (j = 0; j < tree2->nodesNum; ++j){
        L_rootTable[i][j] = 0;
    }
    }

    for (i = 0; i < rootNum; i++){
        tree1 = treeRoot(tree1, rootPositions[i][0], rootPositions[i][1], 0);
        sortedSet1 = treeTopologicalSort(tree1);
        setPermutation1 = calcUnsignedPermutation(sortedSet1, tree1->nodesNum);

        umastSet[i] = UMASTStep(tree1, tree2, sortedSet1, sortedSet2, setPermutation1, setPermutation2, TAB, L_TAB,
            rootTable[i], L_rootTable[i], brAllocator);
        L_umastSet[i] = L_rootTable[i][tree2->nodesNum - 1];
        tree1 = treeUnRoot(tree1, 0);
        free(setPermutation1);
            free(sortedSet1);
    }

    bestBranch = umastSet[0];
    bestL = L_umastSet[0];
    bestLeavesNum = branchGetLeavesPosNum(bestBranch);
    for (i = 1; i < rootNum; ++i) {
        curBranch = umastSet[i];
    curL = L_umastSet[i];
        curLeavesNum = branchGetLeavesPosNum(curBranch);
        if (curLeavesNum > bestLeavesNum || (curLeavesNum == bestLeavesNum && curL < bestL)) {
            bestBranch = curBranch;
            bestLeavesNum = curLeavesNum;
        bestL = curL;
        }
    }

    *mast_tree = treePruneByBranch(tree1, bestBranch);

    for(i = 0; i < tree1->nodesNum - 1; i++){
        free(rootPositions[i]);
    }
    free(rootPositions);
    free(sortedSet2);
    free(setPermutation2);
    free(umastSet);
    free(L_umastSet);
    free(rootTable[0]);
    free(rootTable);
    free(L_rootTable[0]);
    free(L_rootTable);
    branchAllocatorDelete(brAllocator);
    free(L_TAB[0][0]);
    free(L_TAB[0]);
    free(L_TAB);
    free(TAB[0][0]);
    free(TAB[0]);
    free(TAB);
    return bestL;
}// UMAST
