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
#include <time.h>

char** findCommonNamesSet(char** names1, size_t l1, char** names2, size_t l2, size_t* common_size) {
    int i = 0;
    int j = 0;
    int mx = l1 > l2 ? l1 : l2;
    char** common_set = malloc(sizeof(char**) * mx);
    size_t cm_set_size = 0;
    for (i = 0; i < l1; ++i) {
        for (j = 0; j < l2; ++j) {
            if (strcmp(names1[i], names2[j]) == 0) {
                common_set[cm_set_size++] = names1[i];
                break;
            }
        }
    }
    if (cm_set_size != 0) {
        common_set = realloc(common_set, sizeof(char**) * cm_set_size);
    } else {
        free(common_set);
        common_set = NULL;
    }

    *common_size = cm_set_size;
    return common_set;
} //findCommonNamesSet

unsigned* treeRootAndTopSort(Tree* tree, unsigned nodeID, unsigned neighbourID, unsigned* setPermutation) {
    int i, j;
    unsigned* result;

    tree = treeRoot(tree, nodeID, neighbourID, 0);
    result = treeTopologicalSort(tree);
    for (i = 0; i < tree->nodesNum; i++) {
        for (j = 0; j < tree->nodesNum; j++) {
            if (i == result[j]) {
                setPermutation[i] = j;
            }
        }
    }
    return result;
} //treeRootAndTopSort

int* calculateLeavesPermutation(Tree* tree1, Tree* tree2) {
    int i;
    char** leaves1;
    char** leaves2;
    int* permutation;

    leaves1 = treeGetNames(tree1);
    leaves2 = treeGetNames(tree2);

    permutation = calculatePermutation(leaves1, leaves2, tree1->leavesNum);
    free(leaves1);
    free(leaves2);
    return permutation;
} //calculateLeavesPermutation

int** getAllRoots(Tree* tree) {
    int i, j, count;
    unsigned* sortedSet;
    int** result;
    Node* curNode;

    result = (int**)calloc(sizeof(int*), (tree->nodesNum - 1));

    for (i = 0; i < (tree->nodesNum - 1); i++) {
        result[i] = (int*)calloc(sizeof(int), 2);
    }
    for (i = 0; i < (tree->nodesNum - 1); i++) {
        for (j = 0; j < 2; j++) {
            result[i][j] = -1;
        }
    }

    treeWash(tree);
    for (i = 0; i < tree->nodesNum; i++) {
        if (tree->nodes[i]->neiNum > 1) {
            break;
        }
    }

    sortedSet = treeDFS(tree, i, AddOnExit);

    treeWash(tree);
    count = 0;
    for (i = 0; i < tree->nodesNum; i++) {

        curNode = tree->nodes[sortedSet[i]];
        curNode->color = BLACK;
        for (j = 0; j < curNode->neiNum; j++) {
            if (curNode->neighbours[j]->color == WHITE) {
                result[count][0] = curNode->pos;
                result[count][1] = j;
                count++;
            }
        }
    }
    free(sortedSet);
    return result;
} //getAllRoots

// this can be simplified
int* getRandMaxBranch(Branch*** TAB, int rows, int cols) {
    int i, j, k = -1, count = 0, a = 0;
    int** coords;
    int* result;

    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            if (branchGetLeavesPosNum(TAB[i][j]) > k) {
                k = branchGetLeavesPosNum(TAB[i][j]);
            }
        }
    }

    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            if (branchGetLeavesPosNum(TAB[i][j]) == k) {
                count++;
            }
        }
    }

    coords = (int**)calloc(sizeof(int*), count);
    for (i = 0; i < count; i++) {
        coords[i] = (int*)calloc(sizeof(int), 2);
    }
    for (i = 0; i < count; i++) {
        for (j = 0; j < 2; j++) {
            coords[i][j] = -1;
        }
    }

    result = (int*)calloc(sizeof(int), 2);
    for (j = 0; j < 2; j++) {
        result[j] = -1;
    }

    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            if (branchGetLeavesPosNum(TAB[i][j]) == k) {
                coords[a][0] = i;
                coords[a][1] = j;
                a++;
            }
        }
    }

    if (a > count) {
        fprintf(stderr, "Error, failed to collect all branches of maximum size, umast:getRandMaxBranch\n");
        exit(1);
    }

    k = rand() % count;
    result[0] = coords[k][0];
    result[1] = coords[k][1];

    for (i = 0; i < count; i++) {
        free(coords[i]);
    }
    free(coords);
    return (result);
} //getRandMaxBranch

Tree* makeMAST(Branch* br, Tree* tree1) {
    int i, count;
    unsigned leavesToSaveAmount = 0;
    unsigned* leavesPosArr;
    char** leavesToSave;
    char* temp_string;
    Tree* result;
    FILE* logfile;
    FILE* treefile;
    logfile = fopen("umast.log", "a+");
    treefile = fopen("umast.tre", "w");

    treeWash(tree1);
    result = treeCreate();
    leavesPosArr = branchToLeavesArr(br, tree1->leavesNum);
    for (i = 0; i < tree1->leavesNum; i++) {
        if (leavesPosArr[i] == 1) {
            leavesToSaveAmount++;
        }
    }

    if (leavesToSaveAmount == tree1->leavesNum) {
        fprintf(logfile, "%s\n", "pruned trees match completely");
        result = treeCopy(tree1, 0);
        temp_string = treeToString(result);
        fprintf(logfile, "%s\n", temp_string);
        fprintf(treefile, "%s\n", temp_string);
        free(temp_string);
        fclose(logfile);
        fclose(treefile);

        free(leavesPosArr);

        return result;
    } else if (leavesToSaveAmount == 0) {
        fprintf(logfile, "%s\n", "trees don't match");
        temp_string = treeToString(result);
        fprintf(treefile, "%s\n", temp_string);
        free(temp_string);
        fclose(logfile);
        fclose(treefile);

        free(leavesPosArr);
        return result;
    } else //some leaves are to be deleted but not all
    {
        leavesToSave = (char**)malloc(sizeof(char*) * (leavesToSaveAmount));
        result = treeCopy(tree1, 0);
        count = 0;
        for (i = 0; i < tree1->leavesNum; i++) {
            if (leavesPosArr[i] == 1) {
                leavesToSave[count] = tree1->leaves[i]->name;
                count++;
            }
        }

        if (count > (leavesToSaveAmount)) {
            fprintf(stderr, "Error, count exceeds umast size, umast:makeMast\n");
            exit(1);
        }

        fprintf(logfile, "%d%s\n", tree1->leavesNum - count, " leaves are to be deleted");
        result = treePrune(tree1, leavesToSave, (size_t)leavesToSaveAmount, 0);

        fprintf(logfile, "%s\n", "Maximum agreement subtree found:");
        temp_string = treeToString(result);
        fprintf(logfile, "%s\n", temp_string);
        fprintf(treefile, "%s\n", temp_string);
        free(temp_string);
        fprintf(logfile, "%d\n", result->leavesNum);
        fclose(logfile);
        fclose(treefile);

        free(leavesPosArr);
        free(leavesToSave);
        return result;
    }
} //makeMAST

int find3MaxPos(int a0, int a1, int a2) {
    int maxPos = 0;
    int maxVal = a0;
    if (a1 > maxVal) {
        maxVal = a1;
        maxPos = 1;
    }
    if (a2 > maxVal) {
        maxPos = 2;
    }
    return maxPos;
}

int findParent(int elementID, Tree* tree, unsigned* setPermutation) {
    Node* node = tree->nodes[elementID];
    if (node->neiNum == 1) {
        if (node->neighbours[0]->neiNum != 2) {
            return 0;
        } 
        return 4;
    }

    if (node->neiNum == 2) { // is root
        return 6;
    }

    if (node->neiNum > 3) {
        perror("Wrond node");
        printf("%d\n", node->neiNum);
        exit(1);
    }

    int nei0Pos = node->neighbours[0]->pos;
    int nei1Pos = node->neighbours[1]->pos;
    int nei2Pos = node->neighbours[2]->pos;

    int parentNeiID = find3MaxPos(setPermutation[nei0Pos], setPermutation[nei1Pos], setPermutation[nei2Pos]);

    switch (parentNeiID) {
    case 0:
        if (node->neighbours[0]->neiNum == 2) {
            parentNeiID = 3;
        }
        break;
    case 1:
        if (node->neighbours[1]->neiNum == 2) {
            parentNeiID = 4;
        }
        break;
    case 2:
        if (node->neighbours[2]->neiNum == 2) {
            parentNeiID = 5;
        }
        break;
    default:
        perror("Undefined behaviour");
        exit(1);
    }

    return parentNeiID;
}
// returns number of parent-neighbour or 3 if elemenent is root
int findChilds(int elementID, Tree* tree, unsigned* setPermutation, int* child1,
    int* child2) {
    Node* node = tree->nodes[elementID];
    if (node->neiNum == 2) { // is root
        *child1 = node->neighbours[0]->pos;
        *child2 = node->neighbours[1]->pos;
        return 6;
    }

    if (node->neiNum > 3) {
        perror("Wrond node");
        printf("%d\n", node->neiNum);
        exit(1);
    }

    int nei0Pos = node->neighbours[0]->pos;
    int nei1Pos = node->neighbours[1]->pos;
    int nei2Pos = node->neighbours[2]->pos;

    int parentNeiID = find3MaxPos(setPermutation[nei0Pos], setPermutation[nei1Pos], setPermutation[nei2Pos]);

    switch (parentNeiID) {
    case 0:
        *child1 = nei1Pos;
        *child2 = nei2Pos;
        if (node->neighbours[0]->neiNum == 2) {
            parentNeiID = 3;
        }
        break;
    case 1:
        *child1 = nei0Pos;
        *child2 = nei2Pos;
        if (node->neighbours[1]->neiNum == 2) {
            parentNeiID = 4;
        }
        break;
    case 2:
        *child1 = nei0Pos;
        *child2 = nei1Pos;
        if (node->neighbours[2]->neiNum == 2) {
            parentNeiID = 5;
        }
        break;
    default:
        perror("Undefined behaviour");
        exit(1);
    }

    return parentNeiID;
}

void countVariants(Branch*** TAB, 
                   int** L_TAB,
                   int** K_TAB, 
                   BranchArray* brAr1,
                   BranchArray* brAr2,
                   int a, 
                   int w, 
                   int b, 
                   int c, 
                   int x, 
                   int y,
                   int* bestValue, 
                   int* bestPos,
                   int* bestLval){
    int k1;
    int k2;
    int k_b;
    int k_c;
    int k_x;
    int k_y;
    INT temp;
    int i;
    int bestVariant;
    int curVariant;
    int bestL;
    int curL;
    int p = 0;
    int p_b = 0;
    int p_c = 0;
    int p_x = 0;
    int p_y = 0; 
    int bestVariantId = 0;
    Branch* br_b = brAr1->array[b]; 
    Branch* br_c = brAr1->array[c];
    Branch* br_x = brAr2->array[x];
    Branch* br_y = brAr2->array[y]; 
    int brIntSize = branchGetIntSize(br_b);

    brIntSize = branchGetIntSize(br_b);
    for (i = 0; i < brIntSize; ++i) {
        temp = (br_b->branch[i] & (~ (br_c->branch[i] | br_x->branch[i] | br_y->branch[i]))); 
        p_b += countOnes(temp);
        temp = (br_c->branch[i] & (~ (br_b->branch[i] | br_x->branch[i] | br_y->branch[i]))); 
        p_c += countOnes(temp);
        temp = (br_x->branch[i] & (~ (br_b->branch[i] | br_c->branch[i] | br_y->branch[i]))); 
        p_x += countOnes(temp);
        temp = (br_y->branch[i] & (~ (br_b->branch[i] | br_c->branch[i] | br_x->branch[i]))); 
        p_y += countOnes(temp);
    }
    p = p_b + p_c + p_x + p_y;

    k1 = K_TAB[b][y] + K_TAB[c][x];
    k2 = K_TAB[b][x] + K_TAB[c][y];
    k_b = K_TAB[b][w];
    k_c = K_TAB[c][w];
    k_x = K_TAB[a][x];
    k_y = K_TAB[a][y];

    bestVariant = branchGetLeavesPosNum(TAB[b][x]) + branchGetLeavesPosNum(TAB[c][y]);
    bestL = L_TAB[b][x] + L_TAB[c][y] + k1 * 2 + p;
    bestVariantId = 0;

    curVariant = branchGetLeavesPosNum(TAB[b][y]) + branchGetLeavesPosNum(TAB[c][x]);
    curL = L_TAB[b][y] + L_TAB[c][x] + k2 * 2 + p;
    if ((curVariant > bestVariant)\
            || (curVariant == bestVariant && curL < bestL)) {
        bestVariant = curVariant;
        bestL = curL;
        bestVariantId = 1;
    }

    curVariant = branchGetLeavesPosNum(TAB[a][x]);
    curL = L_TAB[a][x] + k_y * 2 + p_y;
    if ((curVariant > bestVariant)\
            || (curVariant == bestVariant && curL < bestL)) {
        bestVariant = curVariant;
        bestL = curL;
        bestVariantId = 2;
    }

    curVariant = branchGetLeavesPosNum(TAB[a][y]);
    curL = L_TAB[a][y] + k_x * 2 + p_x;
    if ((curVariant > bestVariant)\
            || (curVariant == bestVariant && curL < bestL)) {
        bestVariant = curVariant;
        bestL = curL;
        bestVariantId = 3;
    }

    curVariant = branchGetLeavesPosNum(TAB[b][w]);
    curL = L_TAB[b][w] + k_c * 2 + p_c;
    if ((curVariant > bestVariant)\
            || (curVariant == bestVariant && curL < bestL)) {
        bestVariant = curVariant;
        bestL = curL;
        bestVariantId = 4;
    }

    curVariant = branchGetLeavesPosNum(TAB[c][w]);
    curL = L_TAB[c][w] + k_b * 2 + p_b;
    if ((curVariant > bestVariant)\
            || (curVariant == bestVariant && curL < bestL)) {
        bestVariant = curVariant;
        bestL = curL;
        bestVariantId = 5;
    }

    *bestValue = bestVariant;
    *bestPos = bestVariantId;
    *bestLval = bestL;
} //countVariants

Branch* MAST(Tree* intree1, Tree* intree2, unsigned* set1, unsigned* set2, unsigned* setPermutation1, unsigned* setPermutation2) {
    int b, c, x, y;
    int i; 
    int a, w;
    int* permutation; //permutation of leaf sets between tree1 and tree2
    int* leavesPosArr1; //size = nodes; for every node has -1, for leaf has its pos in leaves
    int* leavesPosArr2; //the same for tree2
    int elementA;
    int elementW;
    int bestValue;
    int bestCase;
    int bestL;
    BranchArray* branchArr1;
    BranchArray* branchArr2;
    Branch* intersection;
    Branch* temp;
    Branch*** TAB;
    int** L_TAB;
    int** K_TAB;
    BranchAllocator* brAllocator = branchAllocatorCreate(intree1->nodesNum * intree1->nodesNum * 2,
        intree1->leavesNum);

    Node* nodeA;
    Node* nodeW;
    Tree* tree1;
    Tree* tree2;

    tree1 = treeCopy(intree1, 0);
    tree2 = treeCopy(intree2, 0);

    permutation = getRange(0, tree1->leavesNum);
    branchArr1 = treeRootedToBranchArray(tree1, permutation);
    free(permutation);
    permutation = calculateLeavesPermutation(tree2, tree1);
    branchArr2 = treeRootedToBranchArray(tree2, permutation);
    free(permutation);

    leavesPosArr1 = treeGetLeavesPos(tree1);
    leavesPosArr2 = treeGetLeavesPos(tree2);
    //TODO dmitrypenzar1996 rewrite this to one allocation
    //
    TAB = (Branch***)calloc(sizeof(Branch**), tree1->nodesNum);
    TAB[0] = (Branch**)calloc(sizeof(Branch*), tree1->nodesNum * tree2->nodesNum);
    for (i = 1; i < tree1->nodesNum; i++) {
        TAB[i] = ((*TAB) + tree2->nodesNum * i);
        //TAB[i] = (Branch**)calloc(sizeof(Branch*), tree2->nodesNum);
    }

    L_TAB = (int**)calloc(tree1->nodesNum, sizeof(int*));
    L_TAB[0] = (int*)calloc(tree1->nodesNum * tree2->nodesNum, sizeof(int));
    for(i = 1; i < tree2->nodesNum; ++i){
        L_TAB[i] = ((*L_TAB) + tree2->nodesNum * i); 
    }

    K_TAB = (int**)malloc(sizeof(int*) * tree1->nodesNum);
    K_TAB[0] = (int*)malloc(sizeof(int) * tree1->nodesNum * tree2->nodesNum);
    for(i = 1; i < tree2->nodesNum; ++i){
        K_TAB[i] = ((*K_TAB) + tree2->nodesNum * i); 
    }

    for (a = 0; a < tree1->nodesNum; ++a){
        elementA = set1[a];
        for (w = 0; w < tree2->nodesNum; ++w ){
            elementW = set2[w];
            temp = branchAllocatorGetBranch(brAllocator);
            branchAndDest(branchArr1->array[elementA], 
                          branchArr2->array[elementW],
                          temp);
            K_TAB[elementA][elementW] = branchGetLeavesPosNum(temp); 
        }
    }

    for (a = 0; a < tree1->nodesNum; ++a) //Until all lines in TAB are filled
    {
        elementA = set1[a];
        nodeA = tree1->nodes[elementA];
        for (w = 0; w < tree2->nodesNum; ++w) //Until all cells in line are filled
        {
            elementW = set2[w];
            nodeW = tree2->nodes[elementW];

            if ((nodeA->neiNum == 1) || (nodeW->neiNum == 1)) //if a or w is a leaf
            {
                if ((nodeA->neiNum == 1) && (nodeW->neiNum == 1)) //if a AND w are leaves
                {
                    if (strcmp(nodeA->name, nodeW->name) == 0) //a and w are equal leaves
                    {
                        intersection = branchAllocatorGetBranch(brAllocator);
                        branchAddLeafUnsafe(intersection, leavesPosArr1[elementA]);
                        TAB[elementA][elementW] = intersection;
                        L_TAB[elementA][elementW] = 0;
                    } else //leaves are not equal
                    {
                        intersection = branchAllocatorGetBranch(brAllocator);
                        TAB[elementA][elementW] = intersection;
                        L_TAB[elementA][elementW] = 0;
                    }
                } else //if a or w is a subtree
                {
                    intersection = branchAllocatorGetBranch(brAllocator);
                    if (nodeA->neiNum == 1) //a is a leaf, w is a subtree
                    {
                        branchAddLeafUnsafe(intersection, leavesPosArr1[elementA]);
                        branchAndDest(intersection, branchArr2->array[elementW], intersection);
                        TAB[elementA][elementW] = intersection;
                        if (branchGetLeavesPosNum(intersection) == 1){
                            L_TAB[elementA][elementW] =\
                                        branchGetLeavesPosNum(branchArr2->array[elementW]) - 1;
                        }else{
                            L_TAB[elementA][elementW] = 0;
                        }
                    } else //a is a subtree, w is a leaf
                    {
                        branchAddLeafUnsafe(intersection, leavesPosArr2[elementW]);
                        branchAndDest(intersection, branchArr1->array[elementA], intersection);
                        TAB[elementA][elementW] = intersection;
                        if (branchGetLeavesPosNum(intersection) == 1){
                            L_TAB[elementA][elementW] =\
                                        branchGetLeavesPosNum(branchArr1->array[elementA]) - 1;
                        }else{
                            L_TAB[elementA][elementW] = 0;
                        }
                    }
                }
            } else //nor a nor w are leaves
            {
                bestValue = -1;
                bestCase = -1;
                bestL = -1;
                findChilds(elementA, tree1, setPermutation1, &b, &c);
                findChilds(elementW, tree2, setPermutation2, &x, &y);
                countVariants(TAB, L_TAB, K_TAB, branchArr1, branchArr2,
                        elementA, elementW, b, c, x, y, &bestValue, &bestCase, &bestL);
                L_TAB[elementA][elementW] = bestL;
                    switch (bestCase) {
                    case 0:
                        temp = branchAllocatorGetBranch(brAllocator);
                        branchOrDest(TAB[b][x], TAB[c][y], temp);
                        TAB[elementA][elementW] = temp;
                        break;
                    case 1:
                        temp = branchAllocatorGetBranch(brAllocator);
                        branchOrDest(TAB[b][y], TAB[c][x], temp);
                        TAB[elementA][elementW] = temp;
                        break;
                    case 2:
                        TAB[elementA][elementW] = TAB[elementA][x];
                        break;
                    case 3:
                        TAB[elementA][elementW] = TAB[elementA][y];
                        break;
                    case 4:
                        TAB[elementA][elementW] = TAB[b][elementW];
                        break;
                    case 5:
                        TAB[elementA][elementW] = TAB[c][elementW];
                        break;
                    default:
                        fprintf(stderr, "Critical error in main loop, umast:MAST\n");
                        exit(1);
                    }
            }
        }
    }


    printf("L is %d\n", L_TAB[tree1->rootId][tree2->rootId]);
    intersection = branchCopy(TAB[tree1->rootId][tree2->rootId]);


    free(leavesPosArr1);
    free(leavesPosArr2);

    for (i = 0; i < branchArr1->size; ++i) {
        branchDelete(branchArr1->array[i]);
    }
    for (i = 0; i < branchArr2->size; ++i) {
        branchDelete(branchArr2->array[i]);
    }

    branchArrayDelete(branchArr1);
    branchArrayDelete(branchArr2);
    treeDelete(tree1);
    treeDelete(tree2);
    branchAllocatorDelete(brAllocator);
    free(TAB[0]);
    free(TAB);
    free(L_TAB[0]);
    free(L_TAB);
    free(K_TAB[0]);
    free(K_TAB);
    return intersection;
} //MAST

Branch* UMASTStep(Tree* intree1, Tree* intree2, unsigned* set1, unsigned* set2,
    unsigned* setPermutation1, unsigned* setPermutation2, Branch**** TAB, Branch** rootRow,
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
    Tree* tree1;
    Tree* tree2;
    Branch** aRow;
    int leavesNum;

    tree1 = treeCopy(intree1, 1);
    tree2 = treeCopy(intree2, 1);
    permutation = getRange(0, tree1->leavesNum);
    branchArr1 = treeRootedToBranchArray(tree1, getRange(0, tree1->leavesNum));
    free(permutation);
    permutation = calculateLeavesPermutation(tree2, tree1);
    branchArr2 = treeRootedToBranchArray(tree2, permutation);
    free(permutation);
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
        if (!nodesNeedToCalculate[a]) {
            continue;
        }

        variant = caseTable[a];
        if (variant != 6) {
            aRow = TAB[caseTable[a]][a];
        } else {
            aRow = rootRow;
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
                    } else //leaves are not equal
                    {
                        intersection = branchAllocatorGetBranch(brAllocator);
                        aRow[w] = intersection;
                    }
                } else //if a or w is a subtree
                {

                    intersection = branchAllocatorGetBranch(brAllocator);
                    if (nodeA->neiNum == 1) //a is a leaf, w is a subtree
                    {
                        branchAddLeafUnsafe(intersection, leavesPosArr1[a]);
                        branchAndDest(intersection, branchArr2->array[w], intersection);
                        aRow[w] = intersection;
                    } else //a is a subtree, w is a leaf
                    {
                        branchAddLeafUnsafe(intersection, permutation[leavesPosArr2[w]]);
                        branchAndDest(intersection, branchArr1->array[a], intersection);
                        aRow[w] = intersection;
                    }
                }

            } else //nor a nor w are leaves
            {
                findChilds(a, tree1, setPermutation1, &b, &c);
                findChilds(w, tree2, setPermutation2, &x, &y);

                bestValue = branchGetLeavesPosNum(TAB[caseTable[b]][b][x]) + branchGetLeavesPosNum(TAB[caseTable[c]][c][y]);
                bestCase = 0;

                int curValue = branchGetLeavesPosNum(TAB[caseTable[b]][b][y]) + branchGetLeavesPosNum(TAB[caseTable[c]][c][x]);
                if (curValue > bestValue) {
                    bestValue = curValue;
                    bestCase = 1;
                }

                curValue = branchGetLeavesPosNum(aRow[x]);
                if (curValue > bestValue) {
                    bestValue = curValue;
                    bestCase = 2;
                }

                curValue = branchGetLeavesPosNum(aRow[y]);
                if (curValue > bestValue) {
                    bestValue = curValue;
                    bestCase = 3;
                }

                curValue = branchGetLeavesPosNum(TAB[caseTable[b]][b][w]);
                if (curValue > bestValue) {
                    bestValue = curValue;
                    bestCase = 4;
                }

                curValue = branchGetLeavesPosNum(TAB[caseTable[c]][c][w]);
                if (curValue > bestValue) {
                    bestValue = curValue;
                    bestCase = 5;
                }

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
    branchArrayDelete(branchArr1);
    branchArrayDelete(branchArr2);
    leavesNum = tree1->nodesNum - 1;
    treeDelete(tree1);
    treeDelete(tree2);
    return rootRow[leavesNum]; // root always should be max
} //UMASTStep

Tree* makeUMASTTree(Branch* br, Tree* tree1) {
    int i;
    Tree* result;
    FILE* logfile;
    FILE* treefile;
    size_t commonLeavesNum = 0;
    size_t* requiredLeavesPoses = branchGetLeavesPos(br, &commonLeavesNum, tree1->leavesNum);
    char** requiredLeavesNames;
    char* temp_string;

    //TODO make them arguments
    logfile = fopen("umast.log", "a+");
    treefile = fopen("umast.tre", "w");

    if (commonLeavesNum == tree1->leavesNum) {
        fprintf(logfile, "%s\n", "pruned trees match completely");
        result = treeCopy(tree1, 0);
        temp_string = treeToString(result);
        fprintf(logfile, "%s\n", temp_string);
        fprintf(treefile, "%s\n", temp_string);
        free(temp_string);
        fclose(logfile);
        fclose(treefile);
        free(requiredLeavesPoses);
        return result;
    } else if (commonLeavesNum == 0) {
        fprintf(logfile, "%s\n", "trees don't match");
        fprintf(treefile, "();\n");
        fclose(logfile);
        fclose(treefile);
        return 0;
    } else {
        fprintf(logfile, "%d%s\n", tree1->leavesNum - commonLeavesNum, " leaves are to be deleted");
        requiredLeavesNames = calloc(commonLeavesNum, sizeof(char*));
        for (i = 0; i < commonLeavesNum; ++i) {
            requiredLeavesNames[i] = tree1->leaves[requiredLeavesPoses[i]]->name;
        }

        result = treePrune(tree1, requiredLeavesNames, commonLeavesNum, 0);

        fprintf(logfile, "%s\n", "Maximum agreement subtree is found:");
        temp_string = treeToString(result);
        fprintf(logfile, "%s\n", temp_string);
        fprintf(treefile, "%s\n", temp_string);
        free(temp_string);
        fprintf(logfile, "%d\n", result->leavesNum);
        fclose(logfile);
        fclose(treefile);

        free(requiredLeavesPoses);
        free(requiredLeavesNames);

        return result;
    }
} //makeUMASTTree

void treesToCommonLeaves(Tree* tree1, 
                         Tree* tree2,
                         Tree** outtree1,
                         Tree** outtree2){
    char** names1 = treeGetNames(tree1);
    char** names2 = treeGetNames(tree2);
    size_t cm_size = 0;
    char** common_leaves = findCommonNamesSet(names1,
        tree1->leavesNum,
        names2,
        tree2->leavesNum,
        &cm_size);
    free(names1);
    free(names2);

    *outtree1 = treePrune(tree1, common_leaves, cm_size, 0);
    *outtree2 = treePrune(tree2, common_leaves, cm_size, 0);
    free(common_leaves);
}

void UMAST(Tree* intree1, Tree* intree2) {
    int i, j, rootNum;
    int** rootPositions;
    unsigned* sortedSet1; //topologically sorted nodes from rooted tree1
    unsigned* sortedSet2; //topologically sorted nodes from rooted tree2
    unsigned* setPermutation1; //for every node of tree1 tells its pos in topologically sorted set
    unsigned* setPermutation2; //the same for tree2
    Tree* tree1;
    Tree* tree2;
    Tree* result;
    Branch** umastSet;
    Branch* bestBranch;
    int bestLeavesNum;
    Branch* curBranch;
    int curLeavesNum;
    BranchAllocator* brAllocator;

    treesToCommonLeaves(intree1, intree2, &tree1, &tree2);

    rootNum = tree1->nodesNum - 1;
    brAllocator = branchAllocatorCreate(intree1->nodesNum * intree1->nodesNum * rootNum,
        intree1->leavesNum);

    rootPositions = getAllRoots(tree1);
    umastSet = (Branch**)calloc(sizeof(Branch*), rootNum);
    //setPermutation1 = (unsigned*)calloc(sizeof(unsigned), tree1->nodesNum + 1);
    //setPermutation2 = (unsigned*)calloc(sizeof(unsigned), tree2->nodesNum + 1);

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

    Branch*** rootTable = (Branch***)calloc(sizeof(Branch**), rootNum);
    rootTable[0] = (Branch**)calloc(sizeof(Branch*), tree2->nodesNum * rootNum);

    for (i = 1; i < rootNum; ++i) {
        rootTable[i] = rootTable[i - 1] + tree2->nodesNum;
    }

    for (i = 0; i < rootNum; i++){
        tree1 = treeRoot(tree1, rootPositions[i][0], rootPositions[i][1], 0);
        sortedSet1 = treeTopologicalSort(tree1);
        setPermutation1 = calcUnsignedPermutation(sortedSet1, tree1->nodesNum);

        umastSet[i] = UMASTStep(tree1, tree2, sortedSet1, sortedSet2, setPermutation1, setPermutation2, TAB, rootTable[i],
            brAllocator);
        tree1 = treeUnRoot(tree1, 0);
        free(sortedSet1);
    }

    bestBranch = umastSet[0];
    bestLeavesNum = branchGetLeavesPosNum(bestBranch);
    for (i = 1; i < rootNum; ++i) {
        curBranch = umastSet[i];
        curLeavesNum = branchGetLeavesPosNum(curBranch);
        if (curLeavesNum > bestLeavesNum) {
            bestBranch = curBranch;
            bestLeavesNum = curLeavesNum;
        }
    }

    printf("D is %d\n", bestLeavesNum);
    result = makeUMASTTree(bestBranch, tree1);
    if (result) {
        treeDelete(result);
    }

    free(rootPositions);
    free(sortedSet2);
    free(setPermutation1);
    free(setPermutation2);
    free(umastSet);
    free(rootTable[0]);
    free(rootTable);
    free(brAllocator);
    free(TAB[0][0]);
    free(TAB[0]);
    free(TAB);
} //UMAST

void RMAST(Tree* intree1, Tree* intree2)
{
    unsigned* sortedSet1;           //topologically sorted nodes from rooted tree1
    unsigned* sortedSet2;           //topologically sorted nodes from rooted tree2
    unsigned* setPermutation1;      //for every node of tree1 tells its pos in topologically sorted set
    unsigned* setPermutation2;      //the same for tree2
    Branch* maxbranch;
    Tree* tree1;
    Tree* tree2;
    Tree* result;

    treesToCommonLeaves(intree1, intree2, &tree1, &tree2);

    sortedSet1 = treeTopologicalSort(tree1);
    setPermutation1 = calcUnsignedPermutation(sortedSet1, tree1->nodesNum);
    sortedSet2 = treeTopologicalSort(tree2);
    setPermutation2 = calcUnsignedPermutation(sortedSet2, tree2->nodesNum);

    maxbranch = MAST(tree1, tree2, sortedSet1, sortedSet2, setPermutation1, setPermutation2);
    result = makeUMASTTree(maxbranch, tree1);
    branchDelete(maxbranch);
    free(sortedSet1);
    free(sortedSet2); 
    free(setPermutation1); 
    free(setPermutation2);
    treeDelete(tree1);
    treeDelete(tree2);
    if (result)
    {
        printf("%d\n", result->leavesNum);
        treeDelete(result);
    }
} /* RMAST */

int main(int argc, char** argv) {
    Tree* tree1;
    Tree* tree2;
    FILE* logfile;
    char* temp_string;

    if (argc != 3) {
        fprintf(stderr, "Wrong number of arguments\n");
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "%s tree1 tree2\n", argv[0]);
        exit(1);
    }

    logfile = fopen("umast.log", "w");
    time_t t = time(NULL);
    struct tm* aTm = localtime(&t);

    fprintf(logfile, "UMAST execution started\n");
    fprintf(logfile, "%04d/%02d/%02d %02d:%02d:%02d\n", aTm->tm_year + 1900, aTm->tm_mon + 1,
        aTm->tm_mday, aTm->tm_hour, aTm->tm_min, aTm->tm_sec);
    tree1 = treeRead(argv[1], 1);
    tree2 = treeRead(argv[2], 1);
    fprintf(logfile, "Trees are read successfully\n");
    temp_string = treeToString(tree1);
    fprintf(logfile, "%s\n", temp_string);
    free(temp_string);
    temp_string = treeToString(tree2);
    fprintf(logfile, "%s\n", temp_string);
    free(temp_string);
    fclose(logfile);
    //UMAST(tree1, tree2);
    
    RMAST(tree1, tree2);
    treeDelete(tree1);
    treeDelete(tree2);
    return 0;
}
