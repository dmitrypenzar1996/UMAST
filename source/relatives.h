#ifndef _RELATIVES_H_
#define _RELATIVES_H_

#include "Tree.h"
#include "utils.h"
int findParent(int elementID, Tree* tree, unsigned* setPermutation);
int findChilds(int elementID, Tree* tree, unsigned* setPermutation,
    int* child1, int* child2);

#endif
