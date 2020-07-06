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

#ifndef __ADD__H
#define __ADD__H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define TRUE 1
#define FALSE 0
#define true 1
#define false 0

//typedef uint_fast64_t INT; add #include <stdint.h> if uncomment this
//typedef uint_fast64_t INT;

typedef unsigned long INT; /* enough for protein alignments up to 3000 sequences */

int findSymbolPos(char* string, char symbol);

char* readLine(FILE* inFile);

int pow_int(int base, int power);

unsigned int hash(char* string);

int* randomChoice(unsigned int numToChoose, int* array, unsigned int arraySize);

int* getRange(int a, int b); // return array with elements from a to b-1

int* getPermutation(unsigned int arraySize);

size_t* getPermutationRep(size_t arraySize);

int* calculatePermutation(char** leaves, char** seqNames, int size);

char* seqShuffle(char* seq);

void raiseError(const char* message, const char* fileName, const char* funcName,
    int lineNum);

size_t* getPermutationPart(size_t arraySize, size_t partSize);

unsigned* calcUnsignedPermutation(unsigned* set, size_t size);

int countOnes(uint64_t b);

char** findCommonNamesSet(char** names1, size_t l1, char** names2, size_t l2, size_t* common_size);

int find3MaxPos(int a0, int a1, int a2);
#endif
