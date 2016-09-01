/*
 * KDTree.h
 *
 *  Created on: Aug 31, 2016
 *      Author: Itai Shchorry
 */

#include "KDArray.h"
#include "string.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#ifndef KDTREE_H_
#define KDTREE_H_

typedef struct kd_tree_node* KDTreeNode;

KDTreeNode KDTreeGetLeft(KDTreeNode node);

KDTreeNode KDTreeGetRight(KDTreeNode node);

SPPoint* KDTreeGetData(KDTreeNode node);

int KDTreeGetDim(KDTreeNode node);

double KDTreeGetVal(KDTreeNode node);

KDTreeNode buildKDTree(KDArray kd);

#endif /* KDTREE_H_ */
