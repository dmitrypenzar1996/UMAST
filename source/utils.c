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


int findSymbolPos(char* string, char symbol)
{
    int pos = 0;
    while (pos < strlen(string))
    {
        if (string[pos] == symbol)
        {
            return pos;
        }
        pos++;
    }
    return -1;
}

int pow_int(int base, int power)
{
    int i = 1;
    int result = 1;

    while(i <= power)
    {
        result *= base;
        i++;
    }
    return result;
}

unsigned int hash(char* string)
{
    int i = 0;
    unsigned int sum = 0;

    while(i <= strlen(string))
    {
        sum += (unsigned int)string[i] * pow_int(MUL_COEFF, i);
        i++;
    }

    return sum;
}

/*
char* readLine(FILE* inStream)
{
    char* temp;
    const size_t bufferSize = 100;
    size_t lineMaxSize = 300;
    size_t curLineSize = 0;
    size_t bufRealSize = 0;
    char* buffer = malloc(sizeof(char) * bufferSize);
    char* line = malloc(sizeof(char) * lineMaxSize);
    while(fgets(buffer, bufferSize, inStream))
    {
        bufRealSize = strlen(buffer);
	if (!curLineSize && bufRealSize == 1)
	{
		free(line);
		free(buffer);
		return NULL;
	}

        if (curLineSize + bufRealSize >= lineMaxSize)
        {
            lineMaxSize = lineMaxSize * 3 / 2 + 1;
            line = realloc(line, sizeof(char) * lineMaxSize);
        }
        strcpy(line + curLineSize, buffer);
        curLineSize += bufRealSize;
	if (bufferSize - 1> bufRealSize || buffer[bufRealSize - 1] == '\n') break;
    }
    free(buffer);

    if (curLineSize == 0)
    {
        free(line);
        return NULL;
    }

    if ( (temp = strchr(line, '\n')) != NULL) *temp = '\0';
    //line = realloc(line, sizeof(char) * (strlen(line) + 10));
    return line;
}
*/

char* readLine(FILE* inFile)
{
    int lineMaxSize = 400;
    char* line = (char*)malloc(sizeof(char) * lineMaxSize);

    char buffer[200];
    if (fgets(buffer, 200, inFile))
    {
        memcpy(line, buffer, strlen(buffer) + 1);
    }
    else
    {
        return 0;
    }

    while (strlen(buffer) == 200 && fgets(buffer, 200, inFile))
    {
        if (strlen(buffer) + strlen(line) >= lineMaxSize)
        {
            lineMaxSize = lineMaxSize * 3 / 2 + 1;
            line = realloc(line, sizeof(char) * lineMaxSize);
        }
        memcpy(line + strlen(line), buffer, strlen(buffer) + 1);
    }

    char* npos = strchr(line, '\n');
    if (npos != 0)
    {
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
    for (i = 0; i < size; ++i)
    {
        range[i] = a + i;
    }
    return range;
}

int* randomChoice(unsigned int numToChoose, int* array, unsigned int arraySize)
{
    static int init = 0;
    int* choice;
    unsigned int i, pos;
    int temp;

    if (arraySize < numToChoose)
    {
        perror("Number of elements to choose is more than number of elements in array");
        exit(1);
    }

    srand((init += time(0)));
    choice = (int*)malloc(sizeof(numToChoose));

    for (i = 0; i < numToChoose; ++i)
    {
        pos = rand() % (arraySize - i);
        choice[i] = array[pos];
        temp = array[pos];
        array[pos] = array[arraySize - i - 1];
        array[arraySize - i - 1] = temp;
    }
    return choice;
} /* randomChoice */

int* getPermutation(unsigned int arraySize)
{
    static int init = 0;
    int*  permutation = NULL;
    int i = 0;
    int j = 0;

    permutation = (int*)malloc(sizeof(int) * arraySize);
    init += time(0);
    srand(init);

    for (i = 0; i < arraySize; ++i)
    {
        j = rand() % (i + 1);
        permutation[i] = permutation[j];
        permutation[j] = i;
    }

    return permutation;
} /* getPermutation */

size_t* getPermutationPart(size_t arraySize, size_t partSize)
{
    int i = 0;
    int* permutation = getPermutation(arraySize);
    size_t* permutationPart = malloc(sizeof(size_t) * partSize);
    for(i = 0; i < partSize; ++i)
    {
        permutationPart[i] = permutation[i];
    }
    return permutationPart;
}

char* seqShuffle(char* seq)
{
    static int init = 0;
    int  len;
    char* shSeq;
    int i, j;

    len = strlen(seq);
    shSeq = calloc(sizeof(char), len + 1);
    shSeq[len] = '\0';

    init += time(0);
    srand(init);

    for(i = 0; i < len; ++i)
    {
        j = rand() % (i + 1);
        shSeq[i] = shSeq[j];
        shSeq[j] = seq[i];
    }

    return shSeq;
} /* seqShuffle */

int* calculatePermutation(char** leaves, char** seqNames, int size)
{
    int* permutation;
    int i, j;
    int found;

    permutation = (int*)calloc(sizeof(int), size);
    for(i = 0; i < size; ++i)
    {
        found = FALSE;
        for(j = 0; j < size && (!found); ++j)
        {
            if (strcmp(leaves[i], seqNames[j]) == 0)
            {
                permutation[i] = j;
                found = TRUE;
            }
        }
        if (!found)
        {
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

    for(i = 0; i < arraySize; ++i)
    {
        perm[i] = rand() % arraySize;
    }

    return perm;
}/*calculatePermutationRep*/


void raiseError(const char* message, const char* fileName, const char* funcName,
        int lineNum)
{
    fprintf(stderr, "Error: %s, File: %s, Function: %s, line: %d\n",
            message, fileName,  funcName, lineNum);
    exit(1);
}/*raiseError*/

/* test
void main(void)
{
   int* permutation;
   int i = 0, j;
   while (i < 10)
   {
       permutation = getPermutation(10);
       for(j=0; j < 10; j++)
       {
           printf(" %d ", permutation[j]);
       }
       printf("\n");
       free(permutation);
       i++;
   }
   return;
}
*/
