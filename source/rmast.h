#ifndef _RMAST_H_
#define _RMAST_H_

#include "Branch.h"
#include "BranchAllocator.h"
#include "BranchArray.h"
#include "Tree.h"
#include "utils.h"
#include "relatives.h"
#include <stdlib.h>
#include <string.h>


int RMAST(Tree* tree1, Tree* tree2, Tree** mast_tree);
#endif
