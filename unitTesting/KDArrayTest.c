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

int compareSPPoint(SPPoint a, SPPoint b){
	int i=0;
	for(; i < spPointGetDimension(a); i++){
		if(spPointGetAxisCoor(a, i) != spPointGetAxisCoor(b, i)) return 0;
	}
	return 1;
}

int cmpThirdDim (const void* a, const void* b)
{
  const SPPoint* ptr_a = a;
  const SPPoint* ptr_b = b;

   return (int)(spPointGetAxisCoor(*ptr_a, 2) - spPointGetAxisCoor(*ptr_b, 2));
}

static bool compareSPPointTest(){
	double* data = (double*) malloc(sizeof(double));
	double* data2 = (double*) malloc(sizeof(double));
	*data = 1.0;
	*data2 = 2.0;
	SPPoint a = spPointCreate(data,1,1);
	SPPoint b = spPointCreate(data,1,1);
	SPPoint c = spPointCreate(data2, 1, 1);
	ASSERT_TRUE(compareSPPoint(a,b));
	ASSERT_FALSE(compareSPPoint(a,c));

	free(data);
	free(data2);
	return 1;
}

static bool initTest(){
	ASSERT_TRUE(init(NULL, 10) == NULL);

	SPPoint* arr = (SPPoint*) malloc(50*sizeof(SPPoint));
	if(arr == NULL) return NULL;
	SPPoint* arr1 = (SPPoint*) malloc(50*sizeof(SPPoint));
	if(arr1 == NULL) return NULL;
	SPPoint* arr2 = (SPPoint*) malloc(50*sizeof(SPPoint));
	if(arr2 == NULL) return NULL;
	SPPoint* arr3 = (SPPoint*) malloc(50*sizeof(SPPoint));
	if(arr3 == NULL) return NULL;

	int i=0;
	double* dataTemp = (double*) malloc(3*sizeof(double));
	if(dataTemp == NULL) return NULL;

	for(; i < 50; i++){
		*(dataTemp) = i;
		*(dataTemp + 1) = 49.0 - i;
		*(dataTemp + 2) = (double)rand();

		*(arr + i) = spPointCreate(dataTemp, 3, 1);
		*(arr1 + i) = *(arr + i);
		*(arr2 + i) = *(arr + i);
		*(arr3 + i) = *(arr + i);
		printf("1st dim in %d element in the array is %f\n", i, spPointGetAxisCoor(*(arr + i), 0));
		fflush(NULL);
		printf("2nd dim in %d element in the array is %f\n", i, spPointGetAxisCoor(*(arr + i), 1));
		fflush(NULL);
		printf("3rd dim in %d element in the array is %f\n\n", i, spPointGetAxisCoor(*(arr + i), 2));
		fflush(NULL);
	}

	//reverse arr2
	SPPoint temp = NULL;
	for(i=0; i < 25; i++){
		temp = *(arr2 + 49-i);
		*(arr2 + 49-i) = *(arr2 + i);
		*(arr2 + i) = temp;
	}

	qsort(arr3, 50, sizeof(SPPoint), cmpThirdDim);
/*	for(i=0; i < 50; i++){ //check arr3 is ordered
		printf("3rd dim in %d element in the array is %f\n\n", i, spPointGetAxisCoor(*(arr3 + i), 2));
		fflush(NULL);
	}*/

	//create KDArray, and check correctness of rows
	KDArray kd = init(arr, 50);
	SPPoint** Points = KDGetArray(kd);
	SPPoint* points1 = *Points;
	SPPoint* points2 = *(Points+1);
	SPPoint* points3 = *(Points+2);
	for(i=0; i < 50; i++){
		ASSERT_TRUE(compareSPPoint(*(arr1+i), *(points1+i)));
		ASSERT_TRUE(compareSPPoint(*(arr2+i), *(points2+i)));
		ASSERT_TRUE(compareSPPoint(*(arr3+i), *(points3+i)));
	}



	free(arr);
	free(arr1);
	free(arr2);
	free(arr3);
	free(kd);

	return 1;
}

int main(){
	RUN_TEST(compareSPPointTest);
	RUN_TEST(initTest);

	return 1;
}
