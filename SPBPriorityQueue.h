#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include "SPListElement.h"
#include "SPList.h"
#include <stdbool.h>
#include <stdio.h>
/**
 * SP Bounded Priority Queue summary:
 *
 * this priority queue implementation has several properties:
 * 1. the queue is always sorted - the first element has the the lowest value,
 *    and the last element the highest.
 * 2. the number of elements in the queue is bounded.
 *
 * this code implements a priority queue type, bounded by value attained as a parameter from the user.
 * the implementation is represented using an SPList, please refer
 * to SPList.h for usage.
 *
 * The following functions are available:
 *
 *   spBPQueueCreate            - Creates a new empty priority queue
 *   spBPQueueCopy              - copies an existing queue
 *   spBPQueueDestroy           - Deletes an existing queue and frees all resources
 *   spBPQueueClear	       	    - Clears all the data from the queue
 *   spBPQueueSize              - Returns the size of a given queue
 *   spBPQueueGetMaxSize		- returns the max number of elements permitted in the queue
 *   							  (was set at the initialization of the queue)
 *   spBPQueueEnqueue           - Inserts an element in the queue while maintaining the properties of
 *   							  a priority queue
 *   spBPQueueDequeue           - removes the element with the lowest value in the priority queue
 *   spBPQueuePeek              - returns a copy of the element with the lowest value -
 *   							  the first element in the queue
 *   spBPQueuePeekLast			- returns a copy of the element with the highest value -
 *   							  the last element in the queue
 *   spBPQueueMinValue          - returns the lowest value in the priority queue
 *   spBPQueueMaxValue          - returns the highest value in the priority queue
 *   spBPQueueIsEmpty           - returns true iff the priority queue is empty
 *   spBPQueueIsFull            - returns true iff the priority queue is full
 *   spBPQueueGetList			- returns a copy of the SPList which represents the priority queue
 */


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t* SPBPQueue;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * Allocates and creates a new empty priority queue.
 * @return
 * 	NULL - If allocations failed.
 * 	A new priority queue in case of success.
 */
SPBPQueue spBPQueueCreate(int maxSize);

/**
 * Creates a copy of target queue.
 * The new copy will contain all the elements from the source queue in the same
 * order.
 *
 * @param source - the priority queue to copy
 * @return
 * NULL if a NULL was sent or a memory allocation failed.
 * A priority queue containing the same elements with same order as source otherwise.
 */
SPBPQueue spBPQueueCopy(SPBPQueue source);

/**
 * Deallocates an existing priority queue. Clears all elements by using the
 * stored free function.
 *
 * @param source - Target queue to be deallocated. If list is NULL nothing will be
 * done
 *
 */
void spBPQueueDestroy(SPBPQueue source);

/**
 * Removes all elements from target priority queue.
 *
 * The elements are deallocated using the stored freeing function
 * @param source - Target queue to remove all element from
 *
 */
void spBPQueueClear(SPBPQueue source);

/**
 * Returns the number of elements in a priority queue.
 *
 * @param source - The target priority queue which size is requested.
 * @return
 * INVALID ARGUMENT if a NULL pointer was sent.
 * Otherwise the number of elements in the priority queue.
 */
int spBPQueueSize(SPBPQueue source);

/**
 * Returns the maximal number of elements permitted in a priority queue.
 *
 * @param source - The target priority queue which its maximal size is requested.
 * @return
 * INVALID ARGUMENT if a NULL pointer was sent.
 * Otherwise the boundary on the number of elements permitted in the priority queue.
 */
int spBPQueueGetMaxSize(SPBPQueue source);

/**
 * inserts a new element to the queue while maintaining the queue's properties.
 * A copy of the element will be inserted to the queue iff
 * the program'll return SP_BPQUEUE_SUCCESS. see details in @return.
 *
 * @param source - The priority queue for which to add an element
 * @param element The element to insert.
 *
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if a NULL was sent as queue
 * SP_BPQUEUE_OUT_OF_MEMORY if an allocation failed
 * SP_BPQUEUE_FULL if the queue is already full && the value of the element is higher then
 * the highest value in queue
 * SP_BPQUEUE_SUCCESS otherwise (meaning the element has been inserted successfully)
 *
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element);

/**
 * removes the element with the lowest value from the queue
 *
 * @param source - The priority queue we want to remove the lowest valued element from
 *
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if a NULL was sent as queue
 * SP_BPQUEUE_EMPTY if the queue is empty
 * SP_BPQUEUE_SUCCESS the element has been removed successfully
 *
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source);

/**
 * retrieves a copy of the first element in the priority queue.
 *
 * @param source - The target priority queue that we want to look at it's first element
 * @return
 * NULL if a NULL pointer was sent, the queue is empty or an allocation failure
 * copy of the first element of the queue otherwise
 */
SPListElement spBPQueuePeek(SPBPQueue source);

/**
 * retrieves a copy of the last element in the priority queue.
 *
 * @param source - The target priority queue that we want to look at it's last element
 * @return
 * NULL if a NULL pointer was sent, the queue is empty or an allocation failure
 * copy of the last element of the queue otherwise
 */
SPListElement spBPQueuePeekLast(SPBPQueue source);
/**
 * retrieves the value of the first element in the priority queue.
 *
 * @param source - The target priority queue that we want it's lowest value
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if a NULL pointer was sent
 * SP_BPQUEUE_EMPTY if the queue is empty.
 * the value of the first element of the queue otherwise
 */
double spBPQueueMinValue(SPBPQueue source);
/**
 * retrieves the value of the last element in the priority queue.
 *
 * @param source - The target priority queue that we want it's highest value
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if a NULL pointer was sent
 * SP_BPQUEUE_EMPTY if the queue is empty.
 * the value of the last element of the queue otherwise
 */
double spBPQueueMaxValue(SPBPQueue source);

/**
 * retrieves the value of the last element in the priority queue.
 *
 * @param source - The target priority queue to check for emptiness
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if a NULL pointer was sent
 * true iff the queue is empty.
 */
bool spBPQueueIsEmpty(SPBPQueue source);

/**
 * retrieves the value of the last element in the priority queue.
 *
 * @param source - The target priority queue to check if it is full
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if a NULL pointer was sent
 * true iff the queue is full.
 */
bool spBPQueueIsFull(SPBPQueue source);

/**
 * Creates a copy of the SPList used to represent the priority queue
 * The new copy will contain all the elements from the source SPList in the same
 * order.
 *
 * @param source - the priority queue to copy the SPList from
 * @return
 * NULL if a NULL was sent or a memory allocation failed.
 * An SPList containing the same elements with same order as the SPList of the source otherwise.
 */
SPList spBPQueueGetList(SPBPQueue source);

/**
 * returns original copy of source's list
 *
 * @param source - the priority queue we want the list from
 * @return
 * the SPList used by the source otherwise
 */
SPList spBPQueueGetOrigList(SPBPQueue source);

/**
 * remove the last element from the queue
 *
 * @param source - the priority queue we want to remove from
 */
void removeLast(SPBPQueue source);
#endif
