/*
 * KDArrayTest.c
 *
 *  Created on: Aug 27, 2016
 *      Author: Itai Shchorry
 */
#include "testingUtils.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "../KDArray.h"
#include "../HelperFunctions.c"



int cmpThirdDim (const void* a, const void* b)
{
  const SPPoint* ptr_a = a;
  const SPPoint* ptr_b = b;

   return (int)(spPointGetAxisCoor(*ptr_a, 2) - spPointGetAxisCoor(*ptr_b, 2));
}


static bool KDArrayTest(){
	SP_TREE_MSG msg;
	ASSERT_TRUE(init(NULL, 10, &msg) == NULL);

	SPPoint* arr = (SPPoint*) malloc(50*sizeof(SPPoint));
	if(arr == NULL) return NULL;
	int* arr1 = (int*) malloc(50*sizeof(int));
	if(arr1 == NULL) return NULL;
	int* arr2 = (int*) malloc(50*sizeof(int));
	if(arr2 == NULL) return NULL;
	Tuple* arr3 = (Tuple*) malloc(50*sizeof(Tuple));
	if(arr3 == NULL) return NULL;

	int i=0;
	double* dataTemp = (double*) malloc(3*sizeof(double));
	if(dataTemp == NULL) return NULL;

	for(; i < 50; i++){
		*(dataTemp) = i;
		*(dataTemp + 1) = 49.0 - i;
		*(dataTemp + 2) = (double)rand();

		*(arr + i) = spPointCreate(dataTemp, 3, 1);
		*(arr1 + i) = i;
		*(arr2 + i) = 49-i;
		(*(arr3 + i)).data[0] = *(dataTemp+2); //value
		(*(arr3 + i)).data[1] = i; //index

/*		printf("1st dim in %d element in the array is %f\n", i, spPointGetAxisCoor(*(arr + i), 0));
		fflush(NULL);
		printf("2nd dim in %d element in the array is %f\n", i, spPointGetAxisCoor(*(arr + i), 1));
		fflush(NULL);
		printf("3rd dim in %d element in the array is %f\n\n", i, spPointGetAxisCoor(*(arr + i), 2));
		fflush(NULL);*/
	}


	qsort(arr3, 50, sizeof(Tuple), cmpTuples);
/*
	for(i=0; i < 50; i++){ //check arr3 is ordered
		printf("value is %f and index is %d\n", (*(arr3 + i)).data[0], (int)(*(arr3 + i)).data[1]);
		fflush(NULL);
	}
*/

	//create KDArray, and check correctness of rows
	KDArray kd = init(arr, 50, &msg);
	int** points = KDGetArray(kd); //entire indexArray. now we'll get each row separately
	for(i=0; i < 50; i++){
		int a = *(arr1+i);
		ASSERT_TRUE((*(*(points)+i)) == a);
		int b = *(arr2+i);
		ASSERT_TRUE((*(*(points+1)+i)) == b);
		int k = (int)(*(arr3 + i)).data[1];
		int cmp3 = *(*(points+2)+i);
		ASSERT_TRUE(cmp3 == k);
	}

	//check split correctness
	KDArray* kdSplit1 = Split(kd, 0);
	KDArray* kdSplit2 = Split(kd, 1);
	KDArray* kdSplit3 = Split(kd, 2);

	//compare sizes
	ASSERT_TRUE(KDGetSize(*kdSplit1) == 25);
	ASSERT_TRUE(KDGetSize(*kdSplit2) == 25);
	ASSERT_TRUE(KDGetSize(*kdSplit3) == 25);
	ASSERT_TRUE(KDGetSize(*(kdSplit1+1)) == 25);
	ASSERT_TRUE(KDGetSize(*(kdSplit2+1)) == 25);
	ASSERT_TRUE(KDGetSize(*(kdSplit3+1)) == 25);

	//make wanted arrays using init, and compare them to returned values of splits
	int** leftSplit1 = KDGetArray(*kdSplit1); //first 25 in arr1
	int** leftSplit2 = KDGetArray(*kdSplit2); //25-49 in arr1
	int** leftSplit3 = KDGetArray(*kdSplit3); //points with original index 25-49
	int** rightSplit1 = KDGetArray(*(kdSplit1+1)); //25-49
	int** rightSplit2 = KDGetArray(*(kdSplit2+1)); //similar to first 25 points in arr3
	int** rightSplit3 = KDGetArray(*(kdSplit3+1)); //the remaining 25

	int leftDim = KDGetSize(*kdSplit1);
	SPPoint* PLeft1 = KDGetP(*kdSplit1);
	for(i=0; i < leftDim; i++){
		ASSERT_TRUE((*(arr1+i) == *(*(leftSplit1)+i))); //sanity check - first dimension is same
		ASSERT_TRUE(*(arr+i) == *(PLeft1+i)); //sanity check - check PLeft1 has first half of arr
		//add for rest of splits
	}

	int rightDim = KDGetSize(*(kdSplit1+1));
	SPPoint* PRight1 = KDGetP(*(kdSplit1+1));
	for(i=0; i < rightDim; i++){
		ASSERT_TRUE((i == *(*(rightSplit1)+i))); //sanity check - first dimension is same
		ASSERT_TRUE(*(arr+leftDim+i) == *(PRight1+i)); //sanity check - check PLeft1 has first half of arr
		//add for rest of splits
	}

	//
/*	free(arr);*/
	free(arr1);
	free(arr2);
	free(arr3); //need to free each item and then free pointer
	KDDestroy(kd);
/*	KDDestroy(*kdSplit1); //currently i just assigned new space without copying the information. to copy the info, i need to use memcpy
	KDDestroy(*(kdSplit1+1));
	KDDestroy(*kdSplit2);
	KDDestroy(*(kdSplit2+1));
	KDDestroy(*kdSplit3);
	KDDestroy(*(kdSplit3+1));
	free(kdSplit1);
	free(kdSplit2);
	free(kdSplit3);*/

	return 1;
}

int main(){
	RUN_TEST(KDArrayTest);

	return 1;
}
