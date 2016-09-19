/*
 * KDTree.c
 *
 *  Created on: Aug 31, 2016
 *      Author: Itai Shchorry
 */

#include "math.h"
#include <float.h>
#include "string.h"
#include "limits.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "KDTree.h"
#include "SPListElement.h"

#define INVALID_DIM -1
#define INVALID_VAL DBL_MAX

struct kd_tree_node {
	int Dim;
	double Val;
	KDTreeNode Left;
	KDTreeNode Right;
	SPPoint Data;
};

KDTreeNode KDTreeNodeInit(int dim, double val, KDTreeNode left, KDTreeNode right, SPPoint* point){
	KDTreeNode res = (KDTreeNode) malloc(sizeof(*res));
	if(res == NULL) return NULL;
	res->Dim = dim;
	res->Val = val;
	res->Left = left;
	res->Right = right;
	if(point != NULL)res->Data = spPointCopy(*point);
	else res->Data = NULL;
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

SPPoint KDTreeGetData(KDTreeNode node){
	if (node == NULL) return NULL;
	SPPoint res = node->Data;
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
	int dim = KDGetDim(kd);
	int** indexArray = KDGetArray(kd);
	int size = KDGetSize(kd);
/*	if(size == 1) return 0;*/
	double max=0; //saving highest spread
	int maxSpreadDim = 0; //returned dim with highest spread
	double leftMost;
	double rightMost;

	int i;
	double tempSpread;
	for(i=0; i<dim; i++){
		leftMost = spPointGetAxisCoor( *(P+indexArray[i][0] ), i);
		rightMost = spPointGetAxisCoor( *(P+ indexArray[i][size-1] ), i);
		tempSpread = fabs(rightMost - leftMost);
		if(tempSpread > max){
			max = tempSpread;
			maxSpreadDim = i;
		}
	}
	return maxSpreadDim;
}

int randomFunc(KDArray kd){
	int dim = KDGetDim(kd);
	int res = rand() % (dim);
	return res;
}

int incrementalFunc(KDArray kd){
	static int cnt = 0;
	int res = cnt;
	cnt = (cnt+1) % (KDGetDim(kd));
	return res;
}

KDTreeNode buildKDTreeRec(KDArray kd, int (*func) (KDArray)){
	KDTreeNode res;
	SPPoint* point;
	KDArray* splitted;
	KDTreeNode resLeft;
	KDTreeNode resRight;
	int halfRoundUp = 0;
	double resVal = 0;
	int splitDim = 0;
	int size = KDGetSize(kd);
	int** indexArray = KDGetArray(kd);
	if(size == 1){
		point = KDGetP(kd);
		res = KDTreeNodeInit(INVALID_DIM,((int)INVALID_VAL), NULL, NULL, point);
	}
	else{
		splitDim = func(kd);
		splitted = Split(kd, splitDim);
		halfRoundUp = ((size + 1) / 2) - 1;
		resVal = spPointGetAxisCoor(*(KDGetP(kd) + indexArray[splitDim][halfRoundUp]), splitDim);

		resLeft = buildKDTreeRec(*splitted, func);
		resRight = buildKDTreeRec(*(splitted+1), func);
		res = KDTreeNodeInit(splitDim, resVal, resLeft, resRight, NULL);
		KDArrayDestroy(*splitted);
		KDArrayDestroy(*(splitted+1));
		free(splitted);
	}
	return res;
}




KDTreeNode buildKDTree(KDArray kd, SPConfig config, SP_TREE_SPLIT_METHOD splitMethod){
	int (*foo) (KDArray);
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
	return result;
}

bool isLeaf(KDTreeNode node){
	if(node->Data != NULL) return 1;
	return 0;
}

//strictly according to suggested pseudo-code given to us
void KNearestNeighbors(KDTreeNode curr, SPBPQueue bpq, SPPoint p){
	if(curr == NULL) return;
	SPPoint p1 = curr->Data;
	if(isLeaf(curr)){
		int data = spPointGetIndex(p1);
		SPListElement newElem = spListElementCreate(data, spPointL2SquaredDistance(p1, p));
		spBPQueueEnqueue(bpq, newElem);
		spListElementDestroy(newElem);
		return;
	}

	bool mark = 0; // 0 - left, 1 - right; for going right direction in last part.
	double coord = spPointGetAxisCoor(p, curr->Dim);
	if(coord <= curr->Val){
		KNearestNeighbors(curr->Left, bpq, p); //search left subtree
	}
	else{
		mark = 1;
		KNearestNeighbors(curr->Right, bpq, p); //search right subtree
	}

	//check if another search is needed.
	SPListElement lastElem = spBPQueuePeekLast(bpq);
	double lastElemVal = spListElementGetValue(lastElem);
	if((!spBPQueueIsFull(bpq)) || ( (pow(curr->Val - coord, 2) < lastElemVal) )){
		if(mark){ //meaning we've been to right side, now go to left side
			KNearestNeighbors(curr->Left, bpq, p);
		}
		else{ //meaning we've been to left side
			KNearestNeighbors(curr->Right, bpq, p);
		}
	}
	spListElementDestroy(lastElem);
}

void KDTreeDestroy(KDTreeNode head){
	if(!isLeaf(head)){
		if(head->Left != NULL) KDTreeDestroy(head->Left);
		if(head->Right != NULL) KDTreeDestroy(head->Right);
		free(head->Data);
		free(head);
		}
	else{
		spPointDestroy(head->Data);
		free(head->Left); //possibly causes problems
		free(head->Right);
		free(head);
	}

}



