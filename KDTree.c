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
#include "SPListElement.h"



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
	cnt = (cnt+1) % (spPointGetDimension(*(KDGetP(kd))));
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




KDTreeNode buildKDTree(KDArray kd, SPConfig config, SP_TREE_MSG* msg){
	if(kd == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}

	int (*foo) (KDArray);
	SP_TREE_SPLIT_METHOD splitMethod = SPConfigGetSplitMethod(config, msg);
	if (*msg != SP_TREE_SUCCESS){
		//write what we want to happen. probably ok as it is.
		return NULL;
	}
	switch(splitMethod){
	case RANDOM:
		foo = randomFunc;
		break;

	case INCREMENTAL:
		foo = incrementalFunc;
		break;

	case MAX_SPREAD:
		foo = maxSpreadFunc;
	}

	KDTreeNode result = buildKDTreeRec(kd, foo);
	if(result != NULL){
		*msg = SP_CONFIG_SUCCESS;
		return result;
	}
	else{
		*msg = SP_TREE_BUILD_FAILURE;
		return NULL;
	}

}

bool isLeaf(KDTreeNode node){
	if((KDTreeGetLeft(node) == NULL) && (KDTreeGetRight(node) == NULL)) return 1;
	return 0;
}

//strictly according to suggested pseudo-code given to us
void KNearestNeighbors(KDTreeNode curr, SPBPQueue bpq, SPPoint p){
	if(curr == NULL) return;
	SPPoint* p1 = KDTreeGetData(curr);
	int currDim = KDTreeGetDim(curr);
	double currVal = KDTreeGetVal(curr);
	if(isLeaf(curr)){
		int data = spPointGetIndex(*p1);
		SPListElement newElem = spListElementCreate(data, spPointL2SquaredDistance(*p1, p));
		spBPQueueEnqueue(bpq, newElem);
		return;
	}

	bool mark = 0; // 0 - left, 1 - right; for going right direction in last part.
	double coord = spPointGetAxisCoor(*p1, currDim);
	if(coord <= currVal){
		KNearestNeighbors(curr->Left, bpq, p); //search left subtree
	}
	else{
		mark = 1;
		KNearestNeighbors(curr->Right, bpq, p); //search right subtree
	}

	//check if another search is needed.
	SPListElement lastElem = spBPQueuePeekLast(bpq);
	double lastElemVal = spListElementGetValue(lastElem);
	if((!spBPQueueIsFull(bpq)) || ( (pow(currVal - coord, 2) < lastElemVal) )){
		if(mark){ //meaning we've been to right side, now go to left side
			KNearestNeighbors(curr->Left, bpq, p);
		}
		else{ //meaning we've been to left side
			KNearestNeighbors(curr->Right, bpq, p);
		}
	}
}





