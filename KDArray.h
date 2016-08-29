/*
 * KDArray.h
 *
 *  Created on: Aug 27, 2016
 *      Author: Itai Shchorry
 */



#ifndef KDARRAY_H_
#define KDARRAY_H_
#include "SPPoint.h"



typedef struct kd_array* KDArray;

/**
 * Allocates a new KDArray based on given SPPoint array.
 * @return
 * 	NULL - If allocations failed or arr is NULL
 * 	A new List in case of success.
 */
KDArray init(SPPoint* arr, int size);

int KDGetSize(KDArray kd);

SPPoint** KDGetArray(KDArray kd);


#endif /* KDARRAY_H_ */
