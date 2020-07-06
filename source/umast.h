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
#include <time.h>
#include "relatives.h"

int UMAST(Tree* intree1, Tree* intree2, Tree** mast_tree);
#endif
