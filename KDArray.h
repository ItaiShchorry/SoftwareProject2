/*
 * KDArray.h
 *
 *  Created on: Aug 27, 2016
 *      Author: Itai Shchorry
 */



#ifndef KDARRAY_H_
#define KDARRAY_H_
#include "SPPoint.h"

typedef enum sp_tree_msg_t {
	SP_TREE_ALLOC_FAIL,
	SP_TREE_INVALID_ARGUMENT,
	SP_TREE_BUILD_FAILURE,
	SP_TREE_SUCCESS
} SP_TREE_MSG;

typedef struct kd_array* KDArray;

/**
 * Allocates a new KDArray based on given SPPoint array.
 * @pre
 * size == length of arr
 * a point's dim in arr equals all other points dimensions
 * @return
 * 	NULL - If allocations failed or arr is NULL
 * 	A new KDArray in case of success.
 */
KDArray init(SPPoint* arr, int size, SP_TREE_MSG* msg);

/*method of getting the number of SPPoints in KDArray
 * @return
 * NULL if kd == NULL
 * number of SPPoints in the KDArray in case of success*/
int KDGetSize(KDArray kd);

/*getter for indexArray
 * @return
 * NULL if kd == NULL
 * kd->indexArray in case of success*/
int** KDGetArray(KDArray kd);



SPPoint* KDGetP(KDArray kd);

void KDDestroy(KDArray kd);

int cmpTuples(const void* a, const void* b);
/**
 * splits the KDArray to 2 KDArrays according to coor given
 * @return
 * 	NULL - If allocations failed, arr is NULL or coor<0
 * 	2 KDArrays splitted with respect to coor dimension of their points
 */
KDArray* Split(KDArray kdArr, int coor);


#endif /* KDARRAY_H_ */
