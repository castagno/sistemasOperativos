/*
 * management.c
 *
 *  Created on: Nov 10, 2017
 *      Author: Castagno Gustavo Daniel
 */


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

MabStruct *createMab(long offset, int size, int allocated, MabStruct *prev, MabStruct *next){
	if(size <= 0){
		return NULL;
	}
	MabStruct *mab = sbrk(sizeof(MabStruct));
	if(mab == (void*) -1){
		return NULL;
	}
	mab->size = size;
	mab->allocated = allocated;
	mab->prev = prev;
	mab->next = next;

	if(offset <= 0){
		void *ptr = sbrk(size);
		mab->offset = (long)ptr;
	} else {
		mab->offset = offset;
	}

	return mab;
}

void *reserve(int size){
	if(size <= 0){
		return NULL;
	}
	void *memoryPtr = NULL;

	MabStruct *mabNode = mabListPtr;
	MabStruct *mabPrevious = mabNode;
	while (mabNode != NULL){
		if(mabNode->allocated != 0){
			mabPrevious = mabNode;
			mabNode = mabNode->next;
		} else if(mabNode->size == size){
			memoryPtr = (void*)mabNode->offset;
			mabNode->allocated = 1;
			break;
		} else if(mabNode->size > size){
			MabStruct *mabSplit = createMab(mabNode->offset+size, mabNode->size-size, 0, mabNode, mabNode->next);

			mabNode->size = size;
			mabNode->allocated = 1;
			mabNode->next = mabSplit;
			memoryPtr = (void*)mabNode->offset;
			break;
		} else if(mabNode->next == NULL){
			memoryPtr = sbrk(size - mabNode->size);
			mabNode->allocated = 1;
			mabNode->size = size;
			break;
		} else {
			mabPrevious = mabNode;
			mabNode = mabNode->next;
		}
	}

	if(mabNode == NULL){
		MabStruct *mabNew = createMab(0, size, 1, mabPrevious, NULL);
		memoryPtr = (void*)mabNew->offset;

		if(mabPrevious != NULL){
			mabPrevious->next = mabNew;
			printf("mabPrevious->next : %p \n", mabPrevious->next);
		}

		if(mabNew->prev == NULL && mabNew->next == NULL){
			mabListPtr = mabNew;
		}
	}

	if(memoryPtr == (void*)-1){
		return NULL;
	}
	return memoryPtr;
}

int release(void *memOffset){
	if(memOffset == NULL || mabListPtr == NULL){
		return -1;
	}
	MabStruct *mabNode = mabListPtr;

	while(mabNode != NULL && mabNode->offset != (long)memOffset){
		mabNode = mabNode->next;
	}
	if(mabNode == NULL){
		return -1;
	}

	mabNode->allocated = 0;

	if(mabNode->next != NULL && mabNode->next->allocated == 0){
		mabNode->size += mabNode->next->size;
		mabNode->next = mabNode->next->next;
	}

	if(mabNode->prev != NULL && mabNode->prev->allocated == 0){
		mabNode->prev->size += mabNode->size;
		mabNode->prev->next = mabNode->next;
	}

	return 0;
}
