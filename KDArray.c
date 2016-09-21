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
	printf("freed index array completely\n");
	for(i=0; i<kd->size; i++) spPointDestroy(kd->P[i]);
	free(kd->P);
	printf("freed P completely\n");
	free(kd);
}

KDArray init(SPPoint* arr, int size){
	if(arr == NULL) return NULL;
	int i;
	int j;
	double tempCoord;
	int dim = spPointGetDimension(*arr);
	Tuple* valuesArray;
	int k;

	//initialize KDArray
	KDArray res = (KDArray) malloc(sizeof(*res));
	if(res == NULL) return NULL;

	res->P = (SPPoint*) malloc(size*sizeof(SPPoint));
	if(res->P == NULL) return NULL;
	for(i=0; i<size; i++){
		*(res->P+i) = spPointCopy(*(arr+i));
	}

	//allocate memory for array
	res->indexArray = (int**) malloc(dim*sizeof(int*));
	if(res->indexArray == NULL) return NULL;


	//making the d x n matrix
	valuesArray = (Tuple*) malloc(size*sizeof(Tuple)); //value of ith dimension and index of each point
	if(valuesArray == NULL) return NULL;


	for(i=0; i < dim; i++){
		*(res->indexArray + i) = (int*) malloc(size*sizeof(int)); //will hold correct order for each dimension
		if(*(res->indexArray + i) == NULL) return NULL;
		for(j=0; j < size; j++){ //fill valuesArray
			tempCoord = spPointGetAxisCoor(*(res->P+j), i);
			(*(valuesArray + j)).data[0] = tempCoord; //value of point in ith dimension
			(*(valuesArray + j)).data[1] = j; //index of point
		}
		qsort(valuesArray, size, sizeof(Tuple), cmpTuples); //sort the valuesArray by value
		/*for(m=0; m < size; m++){ //make sure array's sorted
			printf("the %d element in valuesArray is %f \n", m, (*(valuesArray+m)).data[0]);
			fflush(NULL);
		}*/

		for(j=0; j < size; j++){ //write the information to tempArray
			k = (int)(*(valuesArray + j)).data[1];
			*(*(res->indexArray + i) + j) = k;
		}
	/*	for(j=0; j < size; j++){
			double d = spPointGetAxisCoor(res->P[res->indexArray[i][j]], i);
			printf("in iteration %d, resArray[%d][%d].data[%d] is %f\n", i, i, j, i, d);
			fflush(NULL);
		}*/

	}
	res->dim = dim;
	res->size = size;

	//freeing all buffers
	free(valuesArray);

	return res;
}

/*helper function to initialize a new KDArray instance given the indexArray and size as parameters
 * @return
 * NULL if P == NULL
 * sets kd->indexArray in case of success*/
void init2(KDArray res, SPPoint *P, int** arr, int size, int dim){
	res = (KDArray) malloc(sizeof(*res));
	res->indexArray = arr;
	res->P = P;
	res->size = size;
	res->dim = dim;
	return;
}

//0 - left part, 1 - right part
void makeAndFillMarkerArray(int* markerArray, int** srcArray, int i, int KDSize, int coor){
	int j;
	for(j=0; j<KDSize; j++) *(markerArray+j)=0;
	int indexToLit;
	for(; i<KDSize; i++){ //marking 1 for right part
		indexToLit = *(*(srcArray+coor) + i);
		*(markerArray+indexToLit) = 1;
	}
	return;
}

void splitAndMap(SPPoint* P, int* markerArray, int** mapLeft, int** mapRight, SPPoint** leftP, SPPoint** rightP, int KDSize){
int i;
	int k1=0;
int k2=0;
for(i=0;i<KDSize; i++){
	if(markerArray[i]){
		*(*(rightP)+k1) = spPointCopy(*(P+i));
		*(*(mapRight)+i) = k1;
		*(*(mapLeft)+i) = -1;
		k1++;
	}
	else{
		*(*(leftP)+k2) = spPointCopy(*(P+i));
		*(*(mapLeft)+i) = k2;
		*(*(mapRight)+i) = -1;
		k2++;
	}
}
}

void buildIndexArrays(int*** res,int dim, int KDSize,int** srcArray, int* mapLeft, int* mapRight){
	if(res== NULL) return;
	*(res) = (int**) malloc(dim*sizeof(int*));
	if(*(res) == NULL) return;
	*(res+1) = (int**) malloc(dim*sizeof(int*));
	if(*(res+1) == NULL) return;
	int halfRoundUp = (KDSize + 1) / 2;
	int i;
	int j;
	int k1 = 0;
	int k2 = 0;
	for(i=0; i < dim; i++){
		*(*(res)+i) = (int*) malloc(halfRoundUp*sizeof(int));
		*(*(res+1)+i) = (int*) malloc((KDSize-halfRoundUp)*sizeof(int));
		for(j=0; j < KDSize; j++){
			int origPIndex = *(*(srcArray+i) +j);
			if((*(mapLeft + origPIndex)) != -1){//meaning we know that point belongs to leftP
				*((*(*(res)+i))+k1) = *(mapLeft + origPIndex);
				k1++;
			}
			else{ //we know that the point is in the right side
				*((*(*(res+1)+i))+k2) = *(mapRight + origPIndex);
				k2++;
			}
		}
		if(k1 != halfRoundUp){
				printf("k1 is not the right size, and is actually %d", k1);
				fflush(NULL);
				return; //bug
			}
		if(k2 != (KDSize-halfRoundUp)){
				printf("k2 is not the right size, and is actually %d", k2);
				fflush(NULL);
				return; //bug
			}
		k1=0;
		k2=0;
	}
}

KDArray* Split(KDArray kdArr, int coor){
	if(kdArr == NULL) return NULL;
	SPPoint* P = KDGetP(kdArr);
	if((coor<0) || (coor>kdArr->dim)) return NULL;

	//initializations
	KDArray* res = (KDArray*) malloc(2*sizeof(KDArray)); //the 2 KDArrays that will be returned
	int** srcArray = KDGetArray(kdArr); //the indexArray of src KDArray
	int KDSize = KDGetSize(kdArr);
	int halfRoundUp = (KDSize + 1) / 2;
	int i=halfRoundUp;
	int dim = kdArr->dim;
	int* markerArray = NULL;
	int*** indexArrays;

	SPPoint* leftP = (SPPoint*) malloc(halfRoundUp*sizeof(SPPoint)); // left set of points
	if(leftP == NULL) return NULL;
	SPPoint* rightP = (SPPoint*) malloc((KDSize-halfRoundUp)*sizeof(SPPoint)); // right set of points
	if(rightP == NULL) return NULL;
	int* mapLeft = (int*) malloc(KDSize*sizeof(int)); //mapping for left set
	if(mapLeft == NULL) return NULL;
	int* mapRight = (int*) malloc(KDSize*sizeof(int)); //mapping for right set
	if(mapRight == NULL) return NULL;

	//make and fill markerArray

	markerArray = (int*) malloc(KDSize*sizeof(int));
	if(markerArray == NULL) return NULL;
	makeAndFillMarkerArray(markerArray, srcArray, i, KDSize, coor);
	printf("******markarray ok*******\n");
	fflush(NULL);
	//split P to  2 SPPoint* according to markerArray, and build the 2 maps
	splitAndMap(P, markerArray, &mapLeft, &mapRight, &leftP, &rightP, KDSize);
	printf("******splitandmap ok*******\n");
	fflush(NULL);

	//initialize and build the 2 new indexArrays for the 2 parts
	indexArrays = (int***) malloc(2*sizeof(int**));
	buildIndexArrays(indexArrays, dim, KDSize,srcArray, mapLeft, mapRight);
	printf("******buildindexarray ok*******\n");
	fflush(NULL);
	//make the 2 new KDArrays
	init2(*res, leftP, *indexArrays, halfRoundUp, dim);
	init2(*(res+1), rightP, *(indexArrays+1), (KDSize - halfRoundUp), dim);
	printf("******init ok*******\n");
	fflush(NULL);
	return res;
}
