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

#include "utils.h"
#define MUL_COEFF 17
#define TRUE 1
#define FALSE 0

int findSymbolPos(char* string, char symbol) {
    int pos = 0;
    while (pos < strlen(string)) {
        if (string[pos] == symbol) {
            return pos;
        }
        pos++;
    }
    return -1;
}

int pow_int(int base, int power) {
    int i = 1;
    int result = 1;

    while (i <= power) {
        result *= base;
        i++;
    }
    return result;
}

unsigned int hash(char* string) {
    int i = 0;
    unsigned int sum = 0;

    while (i <= strlen(string)) {
        sum += (unsigned int)string[i] * pow_int(MUL_COEFF, i);
        i++;
    }

    return sum;
}

char* readLine(FILE* inFile) {
    int lineMaxSize = 400;
    char* line = (char*)malloc(sizeof(char) * lineMaxSize);

    char buffer[200];
    if (fgets(buffer, 200, inFile)) {
        memcpy(line, buffer, strlen(buffer) + 1);
    } else {
        free(line);
        return 0;
    }

    while (strlen(buffer) == 200 && fgets(buffer, 200, inFile)) {
        if (strlen(buffer) + strlen(line) >= lineMaxSize) {
            lineMaxSize = lineMaxSize * 3 / 2 + 1;
            line = realloc(line, sizeof(char) * lineMaxSize);
        }
        memcpy(line + strlen(line), buffer, strlen(buffer) + 1);
    }

    char* npos = strchr(line, '\n');
    if (npos != 0) {
        *npos = '\0'; // delete \n from line
    }

    line = realloc(line, strlen(line) + 1);

    return line;
}

int* getRange(int a, int b) // return array with elements from a to b-1
{
    int size;
    int* range;
    int i;

    size = b - a;
    range = (int*)malloc(sizeof(int) * size);
    for (i = 0; i < size; ++i) {
        range[i] = a + i;
    }
    return range;
}

int* randomChoice(unsigned int numToChoose, int* array, unsigned int arraySize) {
    static int init = 0;
    int* choice;
    unsigned int i, pos;
    int temp;

    if (arraySize < numToChoose) {
        perror("Number of elements to choose is more than number of elements in array");
        exit(1);
    }

    srand((init += time(0)));
    choice = (int*)malloc(sizeof(numToChoose));

    for (i = 0; i < numToChoose; ++i) {
        pos = rand() % (arraySize - i);
        choice[i] = array[pos];
        temp = array[pos];
        array[pos] = array[arraySize - i - 1];
        array[arraySize - i - 1] = temp;
    }
    return choice;
} /* randomChoice */

int* getPermutation(unsigned int arraySize) {
    static int init = 0;
    int* permutation = NULL;
    int i = 0;
    int j = 0;

    permutation = (int*)malloc(sizeof(int) * arraySize);
    init += time(0);
    srand(init);

    for (i = 0; i < arraySize; ++i) {
        j = rand() % (i + 1);
        permutation[i] = permutation[j];
        permutation[j] = i;
    }

    return permutation;
} /* getPermutation */

size_t* getPermutationPart(size_t arraySize, size_t partSize) {
    int i = 0;
    int* permutation = getPermutation(arraySize);
    size_t* permutationPart = malloc(sizeof(size_t) * partSize);
    for (i = 0; i < partSize; ++i) {
        permutationPart[i] = permutation[i];
    }
    return permutationPart;
}

char* seqShuffle(char* seq) {
    static int init = 0;
    int len;
    char* shSeq;
    int i, j;

    len = strlen(seq);
    shSeq = calloc(sizeof(char), len + 1);
    shSeq[len] = '\0';

    init += time(0);
    srand(init);

    for (i = 0; i < len; ++i) {
        j = rand() % (i + 1);
        shSeq[i] = shSeq[j];
        shSeq[j] = seq[i];
    }

    return shSeq;
} /* seqShuffle */

int* calculatePermutation(char** leaves, char** seqNames, int size) {
    int* permutation;
    int i, j;
    int found;

    permutation = (int*)calloc(sizeof(int), size);
    for (i = 0; i < size; ++i) {
        found = FALSE;
        for (j = 0; j < size && (!found); ++j) {
            if (strcmp(leaves[i], seqNames[j]) == 0) {
                permutation[i] = j;
                found = TRUE;
            }
        }
        if (!found) {
            free(permutation);
            return NULL;
        };
    }
    return permutation;
} /* calculatePermutation */

size_t* getPermutationRep(size_t arraySize)
//permutations with repetitions
{
    static int init = 0;
    int i = 0;
    size_t* perm = malloc(sizeof(size_t*) * arraySize);

    init += time(0);
    srand(init);

    for (i = 0; i < arraySize; ++i) {
        perm[i] = rand() % arraySize;
    }

    return perm;
} /*calculatePermutationRep*/

unsigned* calcUnsignedPermutation(unsigned* set, size_t size){
    unsigned* permutation = malloc(sizeof(unsigned) * size);
    int i;


    for (i = 0; i < size; ++i){
        permutation[set[i]] = i;
    }
    return permutation;
}

void raiseError(const char* message, const char* fileName, const char* funcName,
    int lineNum) {
    fprintf(stderr, "Error: %s, File: %s, Function: %s, line: %d\n",
        message, fileName, funcName, lineNum);
    exit(1);
} /*raiseError*/


int countOnes(uint64_t b){
    b = (b & 0x5555555555555555LU) + (b >> 1 & 0x5555555555555555LU);
    b = (b & 0x3333333333333333LU) + (b >> 2 & 0x3333333333333333LU);
    // don't place brackets here
    b = b + (b >> 4) & 0x0F0F0F0F0F0F0F0FLU;
    b = b + (b >> 8);
    b = b + (b >> 16);
    b = b + (b >> 32) & 0x0000007F;
    return (int) b;
}

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
