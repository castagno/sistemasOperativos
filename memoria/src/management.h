/*
 * management.h
 *
 *  Created on: Nov 10, 2017
 *      Author: Castagno Gustavo Daniel
 */

#ifndef MANAGEMENT_H_
#define MANAGEMENT_H_


#include <stdio.h>
#include <unistd.h>

typedef struct mab MabStruct;
struct mab {
	long offset;
	int size;
	int allocated;
	MabStruct * prev;
	MabStruct * next;
};

MabStruct *mabListPtr;

MabStruct *createMab(long offset, int size, int allocated, MabStruct *prev, MabStruct *next);

void *reserve(int size);

int release(void *memOffset);

#endif /* MANAGEMENT_H_ */
