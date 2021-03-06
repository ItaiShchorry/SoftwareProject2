/*
 * KDArray.c
 *
 *  Created on: Aug 27, 2016
 *      Author: Itai Shchorry
 */

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "KDArray.h"


struct kd_array {
	SPPoint* P;
	int size;
	int** indexArray;
	int dim;
};

typedef struct
{
  double data[2];
} Tuple;

int cmpTuples(const void* a, const void* b) {
  const Tuple* ptr_a = a;
  const Tuple* ptr_b = b;
  double fa =  ptr_a->data[0];
  double fb =  ptr_b->data[0];

  return (fa > fb) - (fa < fb);
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

int KDGetDim(KDArray kd){
	if (kd == NULL) return -1;
	return kd->dim;
}

void KDArrayDestroy(KDArray kd){
	int i=0;
	for(; i < kd->dim; i++){
		free(kd->indexArray[i]);
	}
	free(kd->indexArray);

	for(i=0; i<kd->size; i++) spPointDestroy(kd->P[i]);
	free(kd->P);
	free(kd);
}

KDArray init(SPPoint* arr, int size){
	if(arr == NULL) return NULL;
	int i;
	int j;
	double tempCoord;
	int dim = spPointGetDimension(*arr);
	int** resArray;
	Tuple* valuesArray;
	int* tempArray;
	int k;

	//initialize KDArray
	KDArray res = (KDArray) malloc(sizeof(*res));
	if(res == NULL) return NULL;

	SPPoint* P = (SPPoint*) malloc(size*sizeof(SPPoint));
	if(P == NULL) return NULL;
	for(i=0; i<size; i++){
		*(P+i) = spPointCopy(*(arr+i));
	}

	//allocate memory for array
	resArray = (int**) malloc(dim*sizeof(int*));
	if(resArray == NULL) return NULL;


	//making the d x n matrix
	valuesArray = (Tuple*) malloc(size*sizeof(Tuple)); //value of ith dimension and index of each point
	if(valuesArray == NULL) return NULL;



	for(i=0; i < dim; i++){
		tempArray = (int*) malloc(size*sizeof(int)); //will hold correct order for each dimension
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

		for(j=0; j < size; j++){ //write the information to tempArray
			k = (int)(*(valuesArray + j)).data[1];
			*(tempArray + j) = k;
		}

		*(resArray + i) = tempArray;



	}
	res->dim = dim;
	res->P = P;
	res->size = size;
	res->indexArray = resArray;


	//freeing all buffers
	free(valuesArray);
	return res;
}

/*helper function to initialize a new KDArray instance given the indexArray and size as parameters
 * @return
 * NULL if P == NULL
 * sets kd->indexArray in case of success*/
KDArray init2(SPPoint *P, int** arr, int size, int dim){
	KDArray res = (KDArray) malloc(sizeof(*res));
	res->indexArray = arr;
	res->P = P;
	res->size = size;
	res->dim = dim;
	return res;
}

int* makeAndFillMarkerArray(int** srcArray, int i, int KDSize, int coor){
	int* markerArray = (int*) malloc(KDSize*sizeof(int)); //0 - left part, 1 - right part
	if(markerArray == NULL) return NULL;
	int j;
	for(j=0; j<KDSize; j++) *(markerArray+j)=0;
	int indexToLit = 0;
	for(; i<KDSize; i++){ //marking 1 for right part
		indexToLit = *(*(srcArray+coor) + i);
		*(markerArray+indexToLit) = 1;
	}
	return markerArray;
}

void splitAndMap(SPPoint* P, int* markerArray, int* mapLeft, int* mapRight, SPPoint* leftP, SPPoint* rightP, int KDSize){
	int i;
	int k1=0;
	int k2=0;
	for(i=0;i<KDSize; i++){
		if(markerArray[i]){
			*(rightP+k1) = spPointCopy(*(P+i));
			*(mapRight+i) = k1;
			*(mapLeft+i) = -1;
			k1++;
		}
		else{
			*(leftP+k2) = spPointCopy(*(P+i));
			*(mapLeft+i) = k2;
			*(mapRight+i) = -1;
			k2++;
		}
	}
}

int*** buildIndexArrays(int dim, int KDSize,int** srcArray, int* mapLeft, int* mapRight){
	int** leftIndex = (int**) malloc(dim*sizeof(int*));
	if(leftIndex == NULL) return NULL;
	int** rightIndex = (int**) malloc(dim*sizeof(int*));
	if(rightIndex == NULL) return NULL;
	int*** res = (int***) malloc(2*sizeof(int**));
	if(res== NULL) return NULL;
	int halfRoundUp = (KDSize + 1) / 2;
	int i;
	int j;
	int k1 = 0;
	int k2 = 0;
	for(i=0; i < dim; i++){
		int* leftRow = (int*) malloc(halfRoundUp*sizeof(int));
		int* rightRow = (int*) malloc((KDSize-halfRoundUp)*sizeof(int));
		for(j=0; j < KDSize; j++){
			int origPIndex = *(*(srcArray+i) +j);
			if((*(mapLeft + origPIndex)) != -1){//meaning we know that point belongs to leftP
				*(leftRow+k1) = *(mapLeft + origPIndex);
				k1++;
			}
			else{ //we know that the point is in the right side
				*(rightRow+k2) = *(mapRight + origPIndex);
				k2++;
			}
		}
		if(k1 != halfRoundUp){
				printf("k1 is not the right size, and is actually %d", k1);
				fflush(NULL);
				return NULL; //bug
			}
		if(k2 != (KDSize-halfRoundUp)){
				printf("k2 is not the right size, and is actually %d", k2);
				fflush(NULL);
				return NULL; //bug
			}
		k1=0;
		k2=0;
		*(leftIndex + i) = leftRow;
		*(rightIndex + i) = rightRow;
	}
	*res = leftIndex;
	*(res+1) = rightIndex;

	return res;
}

KDArray* Split(KDArray kdArr, int coor){
	if(kdArr == NULL) return NULL;
	SPPoint* P = KDGetP(kdArr);
	if((coor<0) && (coor>kdArr->dim)) return NULL;

	//initializations
	KDArray* res = (KDArray*) malloc(2*sizeof(KDArray)); //the 2 KDArrays that will be returned
	int** srcArray = KDGetArray(kdArr); //the indexArray of src KDArray
	int KDSize = KDGetSize(kdArr);
	int halfRoundUp = (KDSize + 1) / 2;
	int i=halfRoundUp;
	int dim = kdArr->dim;
	int*** indexArrays;

	SPPoint* leftP = (SPPoint*) malloc(halfRoundUp*sizeof(SPPoint)); // left set of points
	if(leftP == NULL) return NULL;
	SPPoint* rightP = (SPPoint*) malloc((KDSize-halfRoundUp)*sizeof(SPPoint)); // right set of points
	if(rightP == NULL) return NULL;
	int* mapLeft = (int*) malloc(KDSize*sizeof(int)); //mapping for left set
	if(mapLeft == NULL) return NULL;
	int* mapRight = (int*) malloc(KDSize*sizeof(int)); //mapping for right set
	if(mapRight == NULL) return NULL;
/*	printf("passed inits inside split\n");*/

	//make and fill markerArray
	int* markerArray = makeAndFillMarkerArray(srcArray, i, KDSize, coor);
/*	for(i=0; i<10; i++){ looks good
		printf("markerArray[%d] is %d ", i, markerArray[i]);
	}
	printf("\n");
	for(i=0; i<10; i++){
		printf("kd->array[coor][%d] is %d ", i, srcArray[coor][i]);
	}
	printf("\n");*/
/*	printf("marker array pass\n");*/
	//split P to  2 SPPoint* according to markerArray, and build the 2 maps
	splitAndMap(P, markerArray, mapLeft, mapRight, leftP, rightP, KDSize);
/*	for(i=0; i<halfRoundUp; i++){ looks good
		printf("splitandmap number %d\n", i);
		fflush(NULL);
		assert(spPointL2SquaredDistance(*(leftP+i), kdArr->P[srcArray[coor][i]]) == 0);
	}*/
/*	printf("splitandmap pass\n");*/
	//initialize and build the 2 new indexArrays for the 2 parts
	indexArrays = buildIndexArrays(dim, KDSize,srcArray, mapLeft, mapRight);
/*	printf("buildindexarrays pass\n");*/


	//make the 2 new KDArrays
	*res = init2(leftP, *indexArrays, halfRoundUp, dim);
	*(res+1) = init2(rightP, *(indexArrays+1), (KDSize - halfRoundUp), dim);
/*		for(int i=0; i<dim; i++){
		for(int j=0; j < halfRoundUp; j++){
			printf("left one: in iteration %d, P[resArray[%d][%d]].data[%d] is %f\n", i, i, j, i, spPointGetAxisCoor(KDGetP(*res)[KDGetArray(*res)[i][j]], i));
			fflush(NULL);
		}
		for(int j=0; j < (KDSize - halfRoundUp); j++){
			printf("right one: in iteration %d, P[resArray[%d][%d]].data[%d] is %f\n", i, i, j, i, spPointGetAxisCoor(KDGetP(*(res+1))[KDGetArray(*(res+1))[i][j]], i));
			fflush(NULL);
		}
		}*/
/*	printf("init2 both pass");*/

	free(mapLeft);
	free(mapRight);
	free(markerArray);
	free(indexArrays);
	return res;
}
