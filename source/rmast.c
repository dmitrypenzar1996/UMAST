#include "rmast.h"

static void countVariants(Branch*** TAB,
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

int RMAST(Tree* tree1,
        Tree* tree2,
        Tree** mast_tree) {
    unsigned* set1 = treeTopologicalSort(tree1);;
    unsigned* set2 = treeTopologicalSort(tree2);
    unsigned* setPermutation1 =  calcUnsignedPermutation(set1, tree1->nodesNum);
    unsigned* setPermutation2 =  calcUnsignedPermutation(set2, tree2->nodesNum);

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
    BranchAllocator* brAllocator = branchAllocatorCreate(tree1->nodesNum * tree1->nodesNum * 2,
        tree1->leavesNum);

    Node* nodeA;
    Node* nodeW;

    permutation = getRange(0, tree1->leavesNum);
    branchArr1 = treeRootedToBranchArray(tree1, permutation);
    free(permutation);
    permutation = calculateLeavesPermutation(tree2, tree1);
    branchArr2 = treeRootedToBranchArray(tree2, permutation);

    leavesPosArr1 = treeGetLeavesPos(tree1);
    leavesPosArr2 = treeGetLeavesPos(tree2);
    TAB = (Branch***)calloc(sizeof(Branch**), tree1->nodesNum);
    TAB[0] = (Branch**)calloc(sizeof(Branch*), tree1->nodesNum * tree2->nodesNum);
    for (i = 1; i < tree1->nodesNum; i++) {
        TAB[i] = ((*TAB) + tree2->nodesNum * i);
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
                        L_TAB[elementA][elementW] = 2;
                    }
                } else //if a or w is a subtree
                {
                    intersection = branchAllocatorGetBranch(brAllocator);
                    if (nodeA->neiNum == 1) //a is a leaf, w is a subtree
                    {
                        branchAddLeafUnsafe(intersection, leavesPosArr1[elementA]);
                        branchAndDest(intersection, branchArr2->array[elementW], intersection);
                        TAB[elementA][elementW] = intersection;
                        if (branchGetLeavesPosNum(intersection) == 1){ // leaf is from tree
                            L_TAB[elementA][elementW] =
                                        branchGetLeavesPosNum(branchArr2->array[elementW]) - 1;
                        }else{ // leaf is not from tree
                            L_TAB[elementA][elementW] = 1 + branchGetLeavesPosNum(branchArr2->array[elementW]);
                        }
                    } else //a is a subtree, w is a leaf
                    {
                        branchAddLeafUnsafe(intersection, permutation[leavesPosArr2[elementW]]);
                        branchAndDest(intersection, branchArr1->array[elementA], intersection);
                        TAB[elementA][elementW] = intersection;
                        if (branchGetLeavesPosNum(intersection) == 1) { // leaf is from tree
                            L_TAB[elementA][elementW] =\
                                        branchGetLeavesPosNum(branchArr1->array[elementA]) - 1;
                        }else{
                            L_TAB[elementA][elementW] = 1 + branchGetLeavesPosNum(branchArr1->array[elementA]);
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


    bestL = L_TAB[tree1->rootId][tree2->rootId];
    intersection = branchCopy(TAB[tree1->rootId][tree2->rootId]);

    *mast_tree = treePruneByBranch(tree1, intersection);
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
    branchAllocatorDelete(brAllocator);
    free(TAB[0]);
    free(TAB);
    free(L_TAB[0]);
    free(L_TAB);
    free(K_TAB[0]);
    free(K_TAB);
    free(permutation);
    free(set1);
    free(set2);
    free(setPermutation1);
    free(setPermutation2);
    return bestL;
} //RMAST
