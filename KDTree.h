/*
 * KDTree.h
 *
 *  Created on: Aug 31, 2016
 *      Author: Itai Shchorry
 */

#include "string.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "SPBPriorityQueue.h"
#include "KDArray.h"
#include "SPConfig.h"

#ifndef KDTREE_H_
#define KDTREE_H_
#define INVALID_DIM -1
#define INVALID_VAL DBL_MAX


typedef struct kd_tree_node* KDTreeNode;

/**
 * gets the left KDTreeNode of node.
 * @return
 * NULL if a NULL is sent as an argument
 * node.Left in case of success.
 */
KDTreeNode KDTreeGetLeft(KDTreeNode node);

/**
 * gets the right KDTreeNode of node.
 * @return
 * NULL if a NULL is sent as an argument
 * node.Right in case of success.
 */
KDTreeNode KDTreeGetRight(KDTreeNode node);

/**
 * getter for the SPPoint of node.
 * @return
 * a NULL sent as an argument
 * node.Data in case of success.
 */
SPPoint* KDTreeGetData(KDTreeNode node);


/*a  getter for the dimension of node.
 * @return
 * INVALID_DIM if node == NULL
 * node.Dim in case of success*/
int KDTreeGetDim(KDTreeNode node);

/*a  getter for the node's value.
 * @return
 * INVALID_VAL if node == NULL
 * node.Val in case of success*/

double KDTreeGetVal(KDTreeNode node);

/**
 * builds a new KDTree based on given KDArray.
 * @return
 * 	NULL - If allocations failed or kd is NULL
 * 	A new KDArray in case of success.
 */
KDTreeNode buildKDTree(KDArray kd, SPConfig config, SP_TREE_SPLIT_METHOD splitMethod);

/**
 * returns the K nearest neighbors of p that are in KDTree that node is head of
 * @return
 * NULL if curr == NULL, P == NULL
 * if p has different number of dimensions
 * 	k closest neighbors of point p
 */
void KNearestNeighbors(KDTreeNode node,SPBPQueue bpq, SPPoint p);
#endif /* KDTREE_H_ */
