/* Our queue implementation for mutex queue. It's 
   a 'shallow' implementation. I.e. the user responsible
   to clean up memory of members after destruction/dequeueing */

#ifndef _QUEUE_H_
#define _QUEUE_H_

//#include "user_threads.h"
#include <stdlib.h>

#define QUEUE_SUCCESS (0)
#define QUEUE_EMPTY   (-1)
#define QUEUE_FAIL    (-2)


typedef int* data_t;


typedef struct qnode{
	data_t  thread;
	struct qnode*  prev;
        struct qnode*  next;
}qnode_t;

typedef struct{
	qnode_t* head;
	qnode_t* tail;
}queue_t;


typedef queue_t* Queue;

int queue_init(Queue* queue);

int queue_enqueue(Queue queue, data_t thread);

int queue_dequeue(Queue queue);

int queue_get(Queue queue, data_t* thread);

void queue_destroy(Queue queue);

#endif //_QUEUE_H_
