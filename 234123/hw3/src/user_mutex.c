#include "user_mutex.h"
#include "queue.h"
#include <signal.h>

typedef enum{LOCKED,UNLOCKED}State;
typedef unsigned int umutex_id;

typedef struct {
	Queue wait_queue;
	State state;
	umutex_id id;
	thread_t* owner_thread;
}umutex_struct;

typedef struct umutex_id_node{
	umutex_struct* mutex;
	struct umutex_id_node* next;
	struct umutex_id_node* prev;
}umutex_id_list;

static umutex_id_list* initialized_mutexes;//TODO: init at global level

#define FIND_MINIMAL(curr,min)			\
	while(curr!=NULL)			\
	{					\
		if(curr->id == min)		\
		{				\
			min++;			\
			curr=curr->next;	\
			continue;		\
		}				\
		break;				\
	}					\


#define FIND_BY_PTR(curr,ptr)				\
	while(curr!=NULL)				\
	{						\
		if(curr->mutex == ptr)			\
			break;				\
		curr = curr->next;			\
        }						\

#define MUTEX_NULLRET(ptr) if(ptr==NULL)return MUTEX_INVALID;


#define MUTEX_EXAMINE(curr,mutex) 	umutex_id_node* curr = initialized_mutexes; \
	MUTEX_NULLRET(mutex);						\
	FIND_BY_PTR(curr,mutex);					\
	if(curr==NULL)return MUTEX_UNINITIALIZED;			\

int uthread_mutex_init(uthread_mutex_t* mutex)
{
	umutex_id min=0;
	umutex_id_node* curr = initialized_mutexes;
	uthread_mutex_t* new_mutex=NULL;

	new_mutex = (uthread_mutex_t*)malloc(sizeof(uthread_mutex_t));
	if(new_mutex==NULL)return MUTEX_FAILURE;

	FIND_MINIMAL(curr,min);
	new_mutex->id = min;
	new_mutex->state = UNLOCKED;
	if(queue_init(&(new_mutex->wait_queue))!=QUEUE_SUCCESS) return MUTEX_FAILURE;
	new_mutex->owner_thread = NULL;

	new_mutex_node = (umutex_id_node*)malloc(sizeof(umutex_id_node));
	if(new_mutex_node==NULL)return MUTEX_FAILURE;
	new_mutex_node->mutex = new_mutex;
	new_mutex_node->prev = curr->prev;
	new_mutex_node->next = curr;

	if(curr->prev!=NULL) curr->prev->next = new_mutex_node;
	curr->prev= new_mutex_node;

	return MUTEX_SUCCESS;


}

int uthread_mutex_destroy(uthread_mutex_t* mutex)
{
	umutex_id_node* curr=initialized_mutexes;

	MUTEX_NULLRET(mutex);
	if(mutex->state==LOCKED || mutex->wait_queue->head!=NULL) return MUTEX_LOCKED;
	

	FIND_BY_PTR(curr,mutex);
	if(curr==NULL) return MUTEX_UNINITIALIZED;
	if(curr->next != NULL) curr->next->prev = curr->prev;
	if(curr->prev != NULL) curr->prev->next = curr->next;
	
	free(curr);
	return MUTEX_SUCCESS;
	
}

int uthread_mutex_lock(uthread_mutex_t* mutex)
{
	MUTEX_EXAMINE(curr,mutex);
	if(mutex->state==LOCKED)
	{
		//TODO: Some kind of 'current' needed
		if(queue_enqueue(mutex->wait_queue,current)!=QUEUE_SUCCESS) return MUTEX_FAILURE;
		current->state = THREAD_SUSPENDED;
		return MUTEX_LOCKED;
	}
	mutex->state = LOCKED;
	mutex->owner_thread = current_thread;
	return MUTEX_SUCCESS;
}

int uthread_mutex_try_lock(uthread_mutex_t* mutex)
{
	MUTEX_EXAMINE(curr,mutex);
	if(mutex->state == LOCKED) return MUTEX_LOCKED;
	mutex->owner_thread = current_thread;
	mutex->state = LOCKED;
	return MUTEX_SUCCESS;
}

int uthread_mutex_unlock(uthread_mutex_t* mutex)
{
	MUTEX_EXAMINE(curr,mutex);
       	if(mutex->state==LOCKED && mutex->owner_thread!=current_thread)return MUTEX_LOCKED;
	if(mutex->state==UNLOCKED)return MUTEX_UNLOCKED;
	
	mutex->owner_thread = NULL;
	mutex->state = UNLOCKED;
	return MUTEX_SUCCESS;
}
