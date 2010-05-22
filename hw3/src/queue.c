#include "queue.h"


#define NULLCHECK(k) if((k)==NULL) return QUEUE_FAIL;
#define EMPTYCHECK(k) if((k)==NULL) return QUEUE_EMPTY;


int queue_init(Queue* queue)
{
	NULLCHECK(queue);
	*queue = (queue_t*)malloc(sizeof(queue_t));
	(*queue)->head = (*queue)->tail = NULL;
	return QUEUE_SUCCESS;
}

int queue_enqueue(Queue queue, data_t thread)
{
	qnode_t* node;

	NULLCHECK(queue);
	node = (qnode_t*)malloc(sizeof(qnode_t));
	NULLCHECK(node);
	node->thread = thread;
	node->prev = NULL;
	node->next = (qnode_t*)queue->head;
	queue->head = node;

	if(queue->head->next!=NULL) 
		queue->head->next->prev=node;
	if(queue->tail==NULL) 
		queue->tail = node;

        return QUEUE_SUCCESS;
}

int queue_dequeue(Queue queue)
{
	NULLCHECK(queue);
	EMPTYCHECK(queue->tail);
	if(queue->tail==queue->head)
	{
		queue->tail=queue->head=NULL;
		return QUEUE_SUCCESS;
	}

	queue->tail = (qnode_t*)queue->tail->prev;
	queue->tail->next=NULL;
	return QUEUE_SUCCESS;
}

int queue_get(Queue queue, data_t* thread)
{
	NULLCHECK(queue);
	EMPTYCHECK(queue->tail);
	*thread = ((data_t)(queue->tail->thread));
	return QUEUE_SUCCESS;
}

void queue_destroy(Queue queue)
{
	free(queue);
}
