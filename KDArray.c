/*
 * KDArray.c
 *
 *  Created on: Aug 27, 2016
 *      Author: Itai Shchorry
 */

#include "string.h"
#include "KDArray.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "HelperFunctions.c"


struct kd_array {
	int size;
	SPPoint** indexArray;
};

int cmpTuples(const void* a, const void* b) {
  const Tuple* ptr_a = a;
  const Tuple* ptr_b = b;

   return (int)(ptr_a->data[0] - ptr_b->data[0]);
}



int KDGetSize(KDArray kd){
	assert(kd != NULL);
	int result = kd->size;
	return result;
}

SPPoint** KDGetArray(KDArray kd){
	assert(kd != NULL);
	SPPoint** result = kd->indexArray;
	return result;
}

KDArray init(SPPoint* arr, int size){
	if(arr == NULL) return NULL;

	//initialize KDArray
	KDArray res = (KDArray) malloc(sizeof(*res));
	if(res == NULL) return NULL;

	SPPoint* P = (SPPoint*) malloc(size*sizeof(SPPoint));
	if(P == NULL) return NULL;
	memcpy(&P, &arr, sizeof(arr));

	//allocate memory for array
	int dim = spPointGetDimension(*P);
	SPPoint** resArray = (SPPoint**) malloc(dim*sizeof(SPPoint*));
	if(resArray == NULL) return NULL;

	//making the d x n matrix
	Tuple* valuesArray = (Tuple*) malloc(size*sizeof(Tuple)); //value of ith dimension and index of each point
	if(valuesArray == NULL) return NULL;

	int i=0;
	int j;
	double tempCoord;
	for(; i < dim; i++){
		SPPoint* tempArray = (SPPoint*) malloc(size*sizeof(SPPoint)); //will hold correct order for each dimension
		if(tempArray == NULL) return NULL;
		for(j=0; j < size; j++){ //fill valuesArray
			tempCoord = spPointGetAxisCoor(*(P+j), i);
			(*(valuesArray + j)).data[0] = tempCoord; //value of point in ith dimension
			(*(valuesArray + j)).data[1] = j; //index of point
		}
		qsort(valuesArray, size, sizeof(Tuple), cmpTuples); //sort the valuesArray by value

/*		for(m=0; m < size; m++){ //make sure array's sorted
			printf("the %d element in valuesArray is %f \n", m, (*(valuesArray+m)).data[0]);
			fflush(NULL);
		}*/

		int k;
		for(j=0; j < size; j++){ //write the information to tempArray
			k = (int)(*(valuesArray + j)).data[1];
			*(tempArray + j) = *(P + k);
		}

		*(resArray + i) = tempArray;

/*		for(j=0; j < size; j++){
			printf("in iteration %d, resArray[%d][%d].data[%d] is %f\n", i, i, j, i, spPointGetAxisCoor(resArray[i][j], i));
			fflush(NULL);
		}*/

	}

	res->size = size;
	res->indexArray = resArray;

	//freeing all buffers
	free(P);
	free(valuesArray);
	return res;
}
