/*
 * KDTree.c
 *
 *  Created on: Aug 31, 2016
 *      Author: Itai Shchorry
 */
#include "KDTree.h"
#include "math.h"
#include <float.h>
#include "string.h"
#include "limits.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "SPPoint.h"

#define INVALID_DIM -1
#define INVALID_VAL DBL_MAX
struct kd_tree_node {
	int Dim;
	double Val;
	KDTreeNode Left;
	KDTreeNode Right;
	SPPoint* Data;
};

KDTreeNode KDTreeNodeInit(int dim, int val, KDTreeNode left, KDTreeNode right, SPPoint* point){
	KDTreeNode res = (KDTreeNode) malloc(sizeof(*res));
	if(res == NULL) return NULL;
	res->Dim = dim;
	res->Val = val;
	res->Left = left;
	res->Right = right;
	res->Data = point;
	return res;
}



KDTreeNode KDTreeGetLeft(KDTreeNode node){
	if(node == NULL) return NULL;
	return node->Left;
}

KDTreeNode KDTreeGetRight(KDTreeNode node){
	if(node == NULL) return NULL;
	return node->Right;
}

SPPoint* KDTreeGetData(KDTreeNode node){
	if (node == NULL) return NULL;
	SPPoint* res = node->Data;
	return res;
}

int KDTreeGetDim(KDTreeNode node){
	if(node == NULL) return INVALID_DIM;
	return node->Dim;
}

double KDTreeGetVal(KDTreeNode node){
	if(node == NULL) return INVALID_VAL;
	return node->Val;
}

int maxSpreadFunc(KDArray kd){
	SPPoint* P = KDGetP(kd);
	int dim = spPointGetDimension(*P);
	int size = KDGetSize(kd);
	double max=0; //saving highest spread
	int maxSpreadDim = 0; //returned dim with highest spread

	int i=0;
	double tempSpread;
	for(; i<dim; i++){
		tempSpread = fabs(spPointGetAxisCoor(*P, i) - spPointGetAxisCoor(*(P + size - 1), i));
		if(tempSpread > max){
			max = tempSpread;
			maxSpreadDim = 0;
		}
	}
	return maxSpreadDim;
}

int randomFunc(KDArray kd){
	int dim = spPointGetDimension((*KDGetP(kd)));
	int res = rand() % (dim + 1);
	return res;
}

int incrementalFunc(KDArray kd){
	static int cnt = 0;
	int res = cnt;
	cnt = (cnt+1) % (spPointGetDimension(*(KDGetP(kd)))); //probably dim is a mistake
	return res;
}

KDTreeNode buildKDTreeRec(KDArray kd, int (*func) (KDArray)){ //arguments for KDArray and calls recursive functions
	//lots of splits, going left or right. we send each time the iSplit and the KDArray
	//the spread/incremental and blah blah is used only once.
	KDTreeNode res;
	int size = KDGetSize(kd);
	if(size == 1){
		SPPoint* point = (KDGetP(kd));
		res = KDTreeNodeInit(INVALID_DIM,INVALID_VAL, NULL, NULL, point);
		return res;
	}
	else{
		int splitDim = func(kd);
		KDArray* splitted = Split(kd, splitDim);
		KDTreeNode resLeft = buildKDTreeRec(*splitted, func);
		KDTreeNode resRight = buildKDTreeRec(*(splitted+1), func);
		int halfRoundUp = ((size + 1) / 2) - 1;
		double resVal = spPointGetAxisCoor(*(KDGetP(kd) + halfRoundUp), splitDim);
		res = KDTreeNodeInit(splitDim, resVal, resLeft, resRight, NULL);
		return res;
	}
}

KDTreeNode buildKDTree(KDArray kd){ //arguments for KDArray and calls recursive functions
	//lots of splits, going left or right. we send each time the iSplit and the KDArray
	//the spread/incremental and blah blah is used only once.
	//getter from the config. takes the method and inserts it to enum
	if(kd == NULL) return NULL;
/*	char* str = NULL; //need to get spKDTreeSplitMethod from SPConfig*/
	int (*foo) (KDArray);
	foo = maxSpreadFunc;
/*
	else if(str == "RANDOM")foo = randomFunc;
	else if (str == "INCREMENTAL") foo = incrementalFunc; //make sure that str can't be something else
*/
	return buildKDTreeRec(kd, foo);
}




