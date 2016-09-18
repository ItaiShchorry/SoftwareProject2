#include "SPPoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct sp_point_t {
	double* data;
	unsigned int dim;
	int index;
}Point;


SPPoint spPointCreate(double* data, int dim, int index){

	if ((data==NULL || dim<=0) || index < 0) return NULL;

	//Allocating memory for the point itself
	SPPoint point = (SPPoint) malloc(sizeof(*point));
	if (point == NULL)return NULL;

	//Allocating memory for the point data
	int i;
	point->data = (double*) malloc(sizeof(double)*dim);
	if (point->data == NULL){
		return NULL;
	}
	//Copying the point data
	for (i=0 ; i<dim ; i++) (point->data)[i] = data[i];

	//Inserting the fields into the new point fields
	point->index = index;
	point->dim = dim;

	return point;
}

SPPoint spPointCopy(SPPoint source){
	assert(source != NULL);
	SPPoint pointCpy = spPointCreate(source->data , source->dim , source->index);
	return pointCpy;
}

void spPointDestroy(SPPoint point){
	if (point==NULL)return;
	free(point->data);
	free(point);
}

int spPointGetDimension(SPPoint point){
	assert(point != NULL);
	int result = point->dim;
	return result;
}

int spPointGetIndex(SPPoint point){
	assert(point != NULL);
	int result = point->index;
	return result;
}
double spPointGetAxisCoor(SPPoint point, int axis){
	assert ((point != NULL) && (axis < ((int)(point->dim))));
	double result = point->data[axis];
	return result;
}
double spPointL2SquaredDistance(SPPoint p, SPPoint q){
	assert ((p != NULL) && (q != NULL) && (p->dim == q->dim));

	double result=0;
	double temp=0;
	int i;
	int pDim = p->dim;

	//Calculating the distance
	for ( i=0 ; i < pDim ; i++){
		temp = p->data[i] - q->data[i];
		result += temp*temp;
	}

	return result;
}


