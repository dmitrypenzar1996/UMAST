#include "relatives.h"

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
        perror("Wrong node");
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
