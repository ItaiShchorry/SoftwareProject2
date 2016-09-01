/*
 * KDTreeTest.c
 *
 *  Created on: Aug 31, 2016
 *      Author: Itai Shchorry
 */


#include "testingUtils.h"
#include <stdbool.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include "../KDArray.h"
#include "../HelperFunctions.c"
#include "../KDTree.h"

void printTreeTest(KDTreeNode tree){
		if(tree == NULL) return;
/*		ASSERT_TRUE(tree.search(22));
		ASSERT_TRUE(tree.search(24));
		ASSERT_TRUE(tree.search(30));
		ASSERT_TRUE(tree.min);*/
		int ab[4]={1,2,3,4};
		/*(Arrays.toString(ab));*/
		printTree(KDTreeGetLeft(tree), "                                          ");
		printTree(KDTreeGetLeft(tree), "                                          ");
	}

void printTree(KDTreeNode node,char* rightspaces) {
		   if (node==NULL){
			   	return;
			   }
/*			  if (node.isRed){
				    System.err.println(rightspaces + node.key);
			   }
			   else{
				   (rightspaces + node.key);
			   }
			   if (node.getRight!=nill){
				   printTree(node.getRight,rightspaces+"     ");
			   }
			   if (node.getLeft!=nill){
				   String rightspaces1=new String(new char[rightspaces.length()-6]).replace("\0", " ");
				   printTree(node.getLeft,rightspaces1);
			   }*/
		   }

static bool KDTreeTest(){
	ASSERT_TRUE(buildKDTree(NULL) == NULL);

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
	KDTreeNode tree = buildKDTree(kd); //reminder - fix enums of functions
	KDTreeNode tempL = KDTreeGetLeft(tree);
	int median = 3;
	while((KDTreeGetLeft(tempL) != NULL)){
			ASSERT_TRUE(KDTreeGetData(tempL) == NULL);
			ASSERT_TRUE(KDTreeGetDim(tempL) == 0); //as we're using max spread, expected to catch 1st dim
			ASSERT_TRUE(KDTreeGetVal(tempL) == (median-1));
			median = (median+1) / 2;
			if(KDTreeGetLeft(tempL) != NULL)
				tempL = KDTreeGetLeft(tempL);
	}
		ASSERT_TRUE(*(KDTreeGetData(tempL)) == (*(KDGetP(kd))));
/*		double templVal = KDTreeGetVal(tempL); its ok - the value of it is infinity, as wanted
		ASSERT_TRUE(templVal == DBL_MAX);*/
		ASSERT_TRUE(KDTreeGetDim(tempL) == -1);
		ASSERT_TRUE(KDTreeGetRight(tempL) == KDTreeGetLeft(tempL));
		ASSERT_TRUE(KDTreeGetRight(tempL) == NULL);


	//print entire tree using in-order walk

	//free everything
/*	KDTreeDestroy(tree);*/
	free(arr);
	free(dataTemp);
	return 1;
}

int main(){
	RUN_TEST(KDTreeTest);

	return 1;
}
