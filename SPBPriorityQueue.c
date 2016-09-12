#include "SPBPriorityQueue.h"
#include "SPListElement.h"
#include "SPList.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

struct sp_bp_queue_t{
	int maxSize;
	SPList list;
	SPListElement last;
};

SPBPQueue spBPQueueCreate(int maxSize){
	SPList newList = spListCreate();

	SPBPQueue queue = (SPBPQueue) malloc(sizeof(*queue));
	if (queue == NULL)return NULL;

	queue->list = newList;
	queue->maxSize = maxSize;
	queue->last = NULL;

	return queue;
}

SPBPQueue spBPQueueCopy(SPBPQueue source){
	if(spBPQueueSize(source) == 0){
		return spBPQueueCreate(spBPQueueGetMaxSize(source));
	}
	SPList newList = spListCopy(source->list);

	SPBPQueue queue = (SPBPQueue) malloc(sizeof(*queue));
	if (queue == NULL)return NULL;

	SPListElement last = spBPQueuePeekLast(source);

	queue->list = newList;
	queue->maxSize = source->maxSize;
	queue->last = last;

	return queue;
}

void spBPQueueDestroy(SPBPQueue source){
	if (source==NULL) {
		free(source);
		return;
	}
	spListDestroy(source->list);
	spListElementDestroy(source->last);
	free(source);
}

void spBPQueueClear(SPBPQueue source){
	if (source==NULL)return;
	spListClear(spBPQueueGetOrigList(source));
}

int spBPQueueSize(SPBPQueue source){
	if (source==NULL)return SP_BPQUEUE_INVALID_ARGUMENT;
	return spListGetSize(spBPQueueGetOrigList(source));
}

int spBPQueueGetMaxSize(SPBPQueue source){
	return source->maxSize;
}

void removeLast(SPBPQueue source){
	if(source==NULL)return;
	if(spBPQueueIsEmpty(source))return;
	SPListElement elem = spListGetFirst(source->list);
	// This redundant "get index" operation must be done, otherwise we'l get an error of "elem set but not used"
	int x = spListElementGetIndex(elem);
	x++;

	int i;
	for(i=0; i < spBPQueueSize(source) - 1; i++){
		elem = spListGetNext(source->list);
	}
	spListRemoveCurrent(source->list);
}

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element){
	// Check for valid input arguments
	if (source == NULL || element == NULL)return SP_BPQUEUE_INVALID_ARGUMENT;
	// Checks that the inserted element is smaller than the max value and that the queue isn't full

	//Search loop for the insertion spot
	if(spBPQueueIsEmpty(source)==1){
		spListInsertFirst(source->list,element);
		return SP_BPQUEUE_SUCCESS;
	}

	int maxSize = spBPQueueGetMaxSize(source);
	int listSize = spListGetSize(source->list);
	bool isMax = (maxSize == listSize);

	if (isMax){
		SPListElement temp = spBPQueuePeekLast(source);
		int check = spListElementCompare(element, temp);
		if (check >= 0){
			spListElementDestroy(temp);
			return SP_BPQUEUE_FULL;
		}
		spListElementDestroy(temp);
	}

	SPListElement currentElem = spListGetFirst(source->list);
	int i;
	for (i=0 ; i<spListGetSize(source->list)-1 ; i++){
		// Check if the value of the inserted element is lower than the value of the current element
		if (spListElementGetValue(element) <= spListElementGetValue(currentElem)){
			//If the values are equal, the lower index will be first
			if (spListElementGetValue(element) == spListElementGetValue(currentElem)){
				if (spListElementGetIndex(element) < spListElementGetIndex(currentElem))spListInsertBeforeCurrent(source->list,element);
				else spListInsertAfterCurrent(source->list,element);
				if(isMax) removeLast(source);
				//spListElementDestroy(currentElem);
				currentElem = NULL;
				free(currentElem);
				return SP_BPQUEUE_SUCCESS;
			}
			// Insert before current
			spListInsertBeforeCurrent(source->list,element);
			if(isMax) removeLast(source);
			//spListElementDestroy(currentElem);
			currentElem = NULL;
			free(currentElem);
			return SP_BPQUEUE_SUCCESS;
		}

		else {
			currentElem = spListGetNext(source->list);
		}
	}

	//current = last node in list. at this point we know we have to get rid of last element
	if((spBPQueueGetMaxSize(source) == spListGetSize(source->list))){
		spListRemoveCurrent(source->list);
		spListInsertLast(source->list, element);
	}
	else{
		if (spListElementCompare(element, currentElem)>=0)
			spListInsertAfterCurrent(source->list,element);
		else spListInsertBeforeCurrent(source->list,element);
	}
	//spListElementDestroy(currentElem);
	currentElem = NULL;
	free(currentElem);
	return SP_BPQUEUE_SUCCESS;

}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source){
	if (source==NULL) return SP_BPQUEUE_INVALID_ARGUMENT;
	if (spListGetSize(source->list)==0)return SP_BPQUEUE_EMPTY;
	//The first element has the lowest value
	spListGetFirst(source->list);
	//Remove the first element
	SP_LIST_MSG msg = spListRemoveCurrent(source->list);
	if (msg==SP_LIST_SUCCESS)return SP_BPQUEUE_SUCCESS;
	return SP_BPQUEUE_SUCCESS;
}

SPListElement spBPQueuePeek(SPBPQueue source){
	if (source==NULL)return NULL;
	if (spBPQueueIsEmpty(source)) return NULL;
	//Get the first element in the queue
	SPListElement newElem =  spListGetFirst(source->list);
	//Return a new copy of it
	SPListElement result = spListElementCopy(newElem);
	if (result==NULL)return NULL;

	return result;
}

SPListElement spBPQueuePeekLast(SPBPQueue source){
	if (source==NULL)return NULL;
	if (spBPQueueIsEmpty(source)) return NULL;
	SPList newList = spListCopy(source->list);
	if (newList==NULL)return NULL;

	SPListElement last = spListGetFirst(newList);
	int i;
	for (i=0 ; i<spListGetSize(newList) ; i++){
		last = spListGetCurrent(newList);
		if (spListGetNext(newList) == NULL)break;
	}

	SPListElement result = spListElementCopy(last);
	spListDestroy(newList);

	return result;
}

double spBPQueueMinValue(SPBPQueue source){
	if (source==NULL)return SP_BPQUEUE_INVALID_ARGUMENT;
	if (spBPQueueIsEmpty(source)==1)return SP_BPQUEUE_EMPTY;
	//Returns the value of the first element in the list
	return spListElementGetValue(spListGetFirst(source->list));
}

double spBPQueueMaxValue(SPBPQueue source){
	if (source==NULL)return SP_BPQUEUE_INVALID_ARGUMENT;
	if (spBPQueueIsEmpty(source)==1)return SP_BPQUEUE_EMPTY;
	//Returns the value of the last element in the list
	SPListElement temp = spBPQueuePeekLast(source);
	double result = spListElementGetValue(temp);
	spListElementDestroy(temp);
	return result;
}

bool spBPQueueIsEmpty(SPBPQueue source){
	if (source==NULL) return NULL;
	if (spListGetSize(source->list)==0)return 1;
	else return 0;
}

bool spBPQueueIsFull(SPBPQueue source){
	if (source==NULL)return NULL;
	if (spListGetSize(source->list)==source->maxSize)return 1;
	else return 0;
}

SPList spBPQueueGetList(SPBPQueue source){
	SPList newList = spListCopy(source->list);
	return newList;
}

SPList spBPQueueGetOrigList(SPBPQueue source){
	return source->list;
}


