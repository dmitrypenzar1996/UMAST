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

#ifndef __RMQ__H__
#define __RMQ__H__
#include "utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    unsigned* segment;
    unsigned** table;
    unsigned length;
    unsigned height;
} SparseTable;

void sparseTableDelete(SparseTable* sparseTable);

SparseTable* sparseTableCalculate(unsigned* segment, unsigned length);

unsigned sparseTableRMQ(SparseTable* sparseTable, unsigned i, unsigned j);

#endif
