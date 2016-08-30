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
	SPPoint* P;
	int size;
	int** indexArray;
};

int cmpTuples(const void* a, const void* b) {
  const Tuple* ptr_a = a;
  const Tuple* ptr_b = b;

   return (int)(ptr_a->data[0] - ptr_b->data[0]);
}

SPPoint* KDGetP(KDArray kd){
	if (kd == NULL) return NULL;
	SPPoint* res = kd->P;
	return res;
}

int KDGetSize(KDArray kd){
	assert(kd != NULL);
	int result = kd->size;
	return result;
}

int** KDGetArray(KDArray kd){
	if (kd == NULL) return NULL;
	int** result = kd->indexArray;
	return result;
}

void KDSetArray(KDArray kd, int** arr){
	if (kd == NULL) return;
	kd->indexArray = arr;
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
	int** resArray = (int**) malloc(dim*sizeof(int*));
	if(resArray == NULL) return NULL;

	//making the d x n matrix
	Tuple* valuesArray = (Tuple*) malloc(size*sizeof(Tuple)); //value of ith dimension and index of each point
	if(valuesArray == NULL) return NULL;

	int i=0;
	int j;
	double tempCoord;
	for(; i < dim; i++){
		int* tempArray = (int*) malloc(size*sizeof(int)); //will hold correct order for each dimension
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
			*(tempArray + j) = k;
		}

		*(resArray + i) = tempArray;

/*		for(j=0; j < size; j++){
			printf("in iteration %d, resArray[%d][%d].data[%d] is %f\n", i, i, j, i, spPointGetAxisCoor(resArray[i][j], i));
			fflush(NULL);
		}*/

	}
	res->P = P;
	res->size = size;
	res->indexArray = resArray;

	//freeing all buffers

	free(valuesArray);
	return res;
}

/*
KDArray* Split(KDArray kdArr, int coor){
	if(kdArr == NULL) return NULL;
	if(coor<0) return NULL;

	KDArray* res = (KDArray*) malloc(2*sizeof(KDArray)); //the 2 KDArrays that will be returned
	SPPoint** srcArray = KDGetArray(kdArr); //the indexArray of src KDArray
	SPPoint* P = *srcArray; //first line of source Array will be regarded as indexer for SPPoints
	int KDSize = KDGetSize(kdArr);

	int* markerArray[KDSize] = {0}; //0 - left part, 1 - right part
	int i=0;
	SPPoint leftArr[(KDSize/2)+1] = {NULL};
	SPPoint rightArr[KDSize/2] = {NULL};
	int i=0;
	for(; i<KDSize; i++){
		if()
	}
	//splitting the indexArray to 2


	int mark = 0; //will mark if KDSize is odd (true) or not (false)
	for(i=0;i<(KDSize/2); i++){
		leftArr[i] = srcArray[coor][i];
	}
	if(KDSize%2 == 1){ //if KDSize is odd, need to add another element to leftArr
		mark = 1;
		leftArr[i] = srcArray[coor][i];
		i++;
	}
	for(;i<KDSize; i++){
		rightArr[i] = srcArray[coor][i];
		markerArray[]
	}

	//build the two maps
	SPPoint** mapLeft = (SPPoint**) malloc(KDSize*sizeof(SPPoint));
	SPPoint** mapRight = (SPPoint**) malloc(KDSize*sizeof(SPPoint));
	//initialize 2 new indexArrays for the 2 parts

	int dim = spPointGetDimension(leftArr[0]);
	SPPoint** leftIndex = (SPPoint**) malloc(dim*sizeof(SPPoint*));
	SPPoint** rightIndex = (SPPoint**) malloc(dim*sizeof(SPPoint*));

	//build the index arrays
	int j;
	for(i=0; i < dim; i++){
		for(j=0; j < KDSize; j++){

		}
	}

	free(mapLeft);
	free(mapRight);
	return res;
}
*/
