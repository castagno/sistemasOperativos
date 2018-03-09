/*
 ============================================================================
 Name        : memoria.c
 Author      : Castagno Gustavo Daniel
 Version     : 0.9
 Copyright   : Open Source
 Description : malloc & free implementations
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "management.h"

#define testStringFirst "string de prueba"
#define testStringSecond "segunda string de prueba"
#define testStringThird "tercera string de prueba"

void printMab(MabStruct *mab){
	printf("%p - offset : %ld \n", mab, mab->offset);
	printf("%p - size : %d \n", mab, mab->size);
	printf("%p - allocated : %d \n", mab, mab->allocated);
	printf("%p - prev : %p \n", mab, mab->prev);
	printf("%p - next : %p \n", mab, mab->next);
	printf("\n");
}

void testReserveFirst(){
	char *stringTest1 = reserve((strlen(testStringFirst) + 1) * sizeof(char));
	strcpy(stringTest1, testStringFirst);

	MabStruct *mabTemp = mabListPtr;

	assert(mabTemp->size == strlen(testStringFirst) + 1);
	assert(mabTemp->allocated != 0);
	assert(mabTemp->prev == NULL);
	assert(mabTemp->next == NULL);
	assert(strcmp(stringTest1, testStringFirst) == 0);

	printf("stringTest1 : %s \n", stringTest1);
	printMab(mabTemp);
}

void testReserveFirstAfterSecondRelease(){
	char *stringTest1 = reserve((strlen(testStringFirst) + 1) * sizeof(char));
	strcpy(stringTest1, testStringFirst);

	MabStruct *mabTemp = mabListPtr;

	assert(mabTemp->size == strlen(testStringFirst) + 1);
	assert(mabTemp->allocated != 0);
	assert(mabTemp->prev == NULL);
	assert(mabTemp->next != NULL);
	assert(strcmp(stringTest1, testStringFirst) == 0);

	printf("stringTest1 : %s \n", stringTest1);
	printMab(mabTemp);
}

void testReserveSecond(){
	char *stringTest2 = reserve((strlen(testStringSecond) + 1) * sizeof(char));
	strcpy(stringTest2, testStringSecond);

	MabStruct *mabTemp = mabListPtr->next;

	assert(mabTemp->size == strlen(testStringSecond) + 1);
	assert(mabTemp->allocated != 0);
	assert(mabTemp->prev == mabListPtr);
	assert(mabTemp->next == NULL);
	assert(strcmp(stringTest2, testStringSecond) == 0);

	printf("stringTest2 : %s \n", stringTest2);
	printMab(mabTemp);
}

void testReserveThird(){
	char *stringTest3 = reserve((strlen(testStringThird) + 1) * sizeof(char));
	strcpy(stringTest3, testStringThird);

	MabStruct *mabTemp = mabListPtr->next->next;

	assert(mabTemp->size == strlen(testStringThird) + 1);
	assert(mabTemp->allocated != 0);
	assert(mabTemp->prev == mabListPtr->next);
	assert(mabTemp->next == NULL);
	assert(strcmp(stringTest3, testStringThird) == 0);

	printf("stringTest3 : %s \n", stringTest3);
	printMab(mabTemp);
}

void testReleaseThird(){
	MabStruct *mabTemp = mabListPtr->next->next;

	int releaseTest = release((void*)mabTemp->offset);

	assert(releaseTest == 0);
	assert(mabTemp->size == strlen(testStringThird) + 1);
	assert(mabTemp->allocated == 0);
	assert(mabTemp->prev == mabListPtr->next);
	assert(mabTemp->next == NULL);

	printMab(mabTemp);
}

void testReleaseSecond(){
	MabStruct *mabTemp = mabListPtr->next;

	int releaseTest = release((void*)mabTemp->offset);

	assert(releaseTest == 0);
	assert(mabTemp->size == strlen(testStringSecond) + 1);
	assert(mabTemp->allocated == 0);
	assert(mabTemp->prev == mabListPtr);
	assert(mabTemp->next == NULL);

	printMab(mabTemp);
}

void testReleaseFirst(){
	MabStruct *mabTemp = mabListPtr;

	int releaseTest = release((void*)mabTemp->offset);

	assert(releaseTest == 0);
	assert(mabTemp->size == strlen(testStringFirst) + 1);
	assert(mabTemp->allocated == 0);
	assert(mabTemp->prev == NULL);
	assert(mabTemp->next == NULL);

	printMab(mabTemp);
}

void testReleaseFirstAfterSecond(){
	MabStruct *mabTemp = mabListPtr;

	int releaseTest = release((void*)mabTemp->offset);

	assert(releaseTest == 0);
	assert(mabTemp->size == strlen(testStringFirst) + strlen(testStringSecond) + 2);
	assert(mabTemp->allocated == 0);
	assert(mabTemp->prev == NULL);
	assert(mabTemp->next == NULL);

	printMab(mabTemp);
}

void testReleaseSecondAfterThird(){
	MabStruct *mabTemp = mabListPtr->next;

	int releaseTest = release((void*)mabTemp->offset);

	assert(releaseTest == 0);
	assert(mabTemp->size == strlen(testStringSecond) + strlen(testStringThird) + 2);
	assert(mabTemp->allocated == 0);
	assert(mabTemp->prev == mabListPtr);
	assert(mabTemp->next == NULL);

	printMab(mabTemp);
}

void testReleaseFirstAfterSecondAfterThird(){
	MabStruct *mabTemp = mabListPtr;

	int releaseTest = release((void*)mabTemp->offset);

	assert(releaseTest == 0);
	assert(mabTemp->size == strlen(testStringFirst) + strlen(testStringSecond) + strlen(testStringThird) + 3);
	assert(mabTemp->allocated == 0);
	assert(mabTemp->prev == NULL);
	assert(mabTemp->next == NULL);

	printMab(mabTemp);
}

void testReserveReleaseFirst(){
	/* Reserva y libera el primer nodo */
	testReserveFirst();

	testReleaseFirst();
}

void testReserveReleaseSecond(){
	/* Reserva el primer y segundo nodo */
	testReserveFirst();

	testReserveSecond();

	/* Libera el segundo nodo */
	testReleaseSecond();

	/* Libera el primer nodo corroborando el tamaño combinado */
	testReleaseFirstAfterSecond();
}

void testReserveReleaseThird(){
	/* Reserva el primer nodo dividiendo el bloque de memoria en dos nodos */
	testReserveFirstAfterSecondRelease();

	/* Reserva el segundo nodo, el tamaño deberia corresponder */
	testReserveSecond();

	/* Reserva el tercer nodo */
	testReserveThird();

	/* Libera el tercer nodo */
	testReleaseThird();

	/* Libera el segundo nodo corroborando el tamaño combinado */
	testReleaseSecondAfterThird();

	/* Libera el primer nodo corroborando el tamaño combinado */
	testReleaseFirstAfterSecondAfterThird();
}

int main(void) {
	char strChar[11] = {'s','t','r','i','n','g','C','h','a','r','\0'};
	printf("%s \n", strChar);

	testReserveReleaseFirst();

	testReserveReleaseSecond();

	testReserveReleaseThird();

	return EXIT_SUCCESS;
}
