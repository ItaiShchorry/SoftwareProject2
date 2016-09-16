/*
 * KDTreeTest.c
 *
 *  Created on: Aug 31, 2016
 *      Author: Itai Shchorry
 */


#include "testingUtils.h"
#include <stdbool.h>
#include <float.h>
#include <String.h>
#include <stdlib.h>
#include <stdio.h>
#include "../KDArray.h"
#include "../KDTree.h"
#include "../SPConfig.h"



void printTree(KDTreeNode node, int size) {
/*	char* str[size*2] = " "*2;
	if (node==NULL)return;
		   printTree(KDTreeGetLeft(node));
		   printf("")
		   }*/
}

static bool KDTreeTest(){

	ASSERT_TRUE(buildKDTree(NULL, NULL, RANDOM) == NULL);
	SPPoint* arr = (SPPoint*) malloc(10*sizeof(SPPoint)); //check for 2 dimensions with 10 points
	if(arr == NULL) return NULL;
	int i=0;
	double* dataTemp = (double*) malloc(2*sizeof(double));
	if(dataTemp == NULL) return NULL;

	for(; i < 10; i++){
		*(dataTemp) = i;
		*(dataTemp + 1) = 10.0 - i;
		*(arr + i) = spPointCreate(dataTemp, 2, 1);
		printf("in point %d, 1st axis is %f and 2nd is %f\n", i, spPointGetAxisCoor(*(arr+i), 0), spPointGetAxisCoor(*(arr+i), 1));
		fflush(NULL);
		printf("1st point 1st axis %f, 2nd %f\n", spPointGetAxisCoor(*(arr), 0), spPointGetAxisCoor(*(arr), 1));
		fflush(NULL);
/*		printf("1st dim in %d element in the array is %f\n", i, spPointGetAxisCoor(*(arr + i), 0));
		fflush(NULL);
		printf("2nd dim in %d element in the array is %f\n", i, spPointGetAxisCoor(*(arr + i), 1));
		fflush(NULL);
		*/
	}


	//create KDArray and KDTree
	KDArray kd = init(arr, 10);
	KDTreeNode tree = buildKDTree(kd, NULL, RANDOM); //reminder - fix enums of functions
	KDTreeNode tempL = KDTreeGetLeft(tree);
	int median = 3;
	while((KDTreeGetLeft(tempL) != NULL)){
/*		printf("got to loop");
		fflush(NULL);*/
		ASSERT_TRUE(KDTreeGetData(tempL) == NULL);
			ASSERT_TRUE(KDTreeGetDim(tempL) == 0); //as we're using max spread, expected to catch 1st dim
			ASSERT_TRUE(KDTreeGetVal(tempL) == (median-1));
			median = (median+1) / 2;
			if(KDTreeGetLeft(tempL) != NULL)
				tempL = KDTreeGetLeft(tempL);
	}
/*		printf("got here");
		fflush(NULL);*/
		ASSERT_TRUE(*(KDTreeGetData(tempL)) == (*(KDGetP(kd))));
/*		double templVal = KDTreeGetVal(tempL); its ok - the value of it is infinity, as wanted
		ASSERT_TRUE(templVal == DBL_MAX);*/
		ASSERT_TRUE(KDTreeGetDim(tempL) == -1);
		ASSERT_TRUE(KDTreeGetRight(tempL) == KDTreeGetLeft(tempL));
		ASSERT_TRUE(KDTreeGetRight(tempL) == NULL);


	//print entire tree using in-order walk
//	printTree(tree, "                                   ", 35);
	//free everything
/*	KDTreeDestroy(tree);*/
	free(arr);
	free(dataTemp);
	return 1;
}


/*int main(){
	char* str = "a         b";
	sprintf(str, str);
	printf("%s", str);
}*/

