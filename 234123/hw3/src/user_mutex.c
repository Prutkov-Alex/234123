#include <signal.h>
#include "user_mutex.h"
#include "user_threads.h"
#include "queue.h"


/*****Structs********/

typedef enum{LOCKED,UNLOCKED}State;
typedef unsigned int umutex_id;

struct umutex_struct{
	Queue wait_queue;
	State state;
	umutex_id id;
	thread_id owner_thread;
};

typedef struct umutex_id_node_t{
	uthread_mutex_t mutex;
	struct umutex_id_node_t* next;
	struct umutex_id_node_t* prev;
}umutex_id_node;

typedef umutex_id_node* umutex_id_list;

/******* Globals ***************/
static umutex_id_list initialized_mutexes;//TODO: init at global level


/******* Predeclarations ********/
void uthread_alarm_handler(int);

/******* Macros *****************/

#define NO_THREAD (-1)

#define FIND_MINIMAL(curr,min)			\
	while(curr!=NULL)			\
	{					\
		if(curr->mutex->id == min)		\
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


#define MUTEX_EXAMINE(curr,mutex)					\
	MUTEX_NULLRET(mutex);						\
	FIND_BY_PTR(curr,mutex);					\
	if(curr==NULL)return MUTEX_UNINITIALIZED;			\


#define DISABLE_SIGALRM(time) time = ualarm(0,0)
#define ENABLE_SIGALRM(time)  ualarm(time,0)

int uthread_mutex_init(uthread_mutex_t* mutex)
{
	int time;
	umutex_id min=0;
	umutex_id_list curr = initialized_mutexes;
	uthread_mutex_t new_mutex=NULL;
	umutex_id_node* new_mutex_node=NULL;

	DISABLE_SIGALRM(time);
	new_mutex = (uthread_mutex_t)malloc(sizeof(struct umutex_struct));
	if(new_mutex==NULL)
	{
		ENABLE_SIGALRM(time);
		return MUTEX_FAILURE;
	}

	FIND_MINIMAL(curr,min);
	new_mutex->id = min;
	new_mutex->state = UNLOCKED;
	if(queue_init(&(new_mutex->wait_queue))!=QUEUE_SUCCESS) 
	{
		free(new_mutex);
		ENABLE_SIGALRM(time);
		return MUTEX_FAILURE;
	}
	new_mutex->owner_thread = NO_THREAD;

	new_mutex_node = (umutex_id_node*)malloc(sizeof(umutex_id_node));
	if(new_mutex_node==NULL) 
	{
		free(new_mutex->wait_queue);
		free(new_mutex);
		ENABLE_SIGALRM(time);

		return MUTEX_FAILURE;
	}
	new_mutex_node->mutex = new_mutex;
	new_mutex_node->prev = curr->prev;
	new_mutex_node->next = curr;

	if(curr->prev!=NULL) curr->prev->next = new_mutex_node;
	curr->prev= new_mutex_node;

	ENABLE_SIGALRM(time);
	return MUTEX_SUCCESS;


}

int uthread_mutex_destroy(uthread_mutex_t mutex)
{
	int time;
	umutex_id_node* curr=initialized_mutexes;

	DISABLE_SIGALRM(time);
	MUTEX_NULLRET(mutex);
	if(mutex->state==LOCKED || mutex->wait_queue->head!=NULL)
	{
		ENABLE_SIGALRM(time);		
		return MUTEX_LOCKED;
	}
	FIND_BY_PTR(curr,mutex); 

	MUTEX_NULLRET(mutex) // Probably he was just destroyed, so we need proper errorcode
	if(curr==NULL) 
	{
		ENABLE_SIGALRM(time);
		return MUTEX_UNINITIALIZED;
	}
	if(curr->next != NULL) curr->next->prev = curr->prev;
	if(curr->prev != NULL) curr->prev->next = curr->next;
	
	mutex=NULL;
	free(curr);
	ENABLE_SIGALRM(time);

	return MUTEX_SUCCESS;
	
}

int uthread_mutex_lock(uthread_mutex_t mutex)
{
	int time;
	umutex_id_node* curr = initialized_mutexes; 
	DISABLE_SIGALRM(time);
	MUTEX_EXAMINE(curr,mutex);
	if(mutex->state==LOCKED)
	{

		if(queue_enqueue(mutex->wait_queue,uthread_self())!=QUEUE_SUCCESS) 
		{
			ENABLE_SIGALRM(time);
			return MUTEX_FAILURE;
		}
		//TODO: need a way to suspend thread.
		/*current->state = THREAD_SUSPENDED;*/
		ENABLE_SIGALRM(time);
		return MUTEX_LOCKED;
	}
	mutex->state = LOCKED;
	mutex->owner_thread = uthread_self();
	ENABLE_SIGALRM(time);
	return MUTEX_SUCCESS;
}

int uthread_mutex_try_lock(uthread_mutex_t mutex)
{
	int time;
	umutex_id_node* curr = initialized_mutexes; 
	DISABLE_SIGALRM(time);
	MUTEX_EXAMINE(curr,mutex);
	if(mutex->state == LOCKED)
	{
		ENABLE_SIGALRM(time);
		return MUTEX_LOCKED;
	}
	mutex->owner_thread = uthread_self();
	mutex->state = LOCKED;
	ENABLE_SIGALRM(time);
	return MUTEX_SUCCESS;
}

int uthread_mutex_unlock(uthread_mutex_t mutex)
{
	int time;
	umutex_id_node* curr = initialized_mutexes; 
	DISABLE_SIGALRM(time);
	MUTEX_EXAMINE(curr,mutex);
       	if(mutex->state==LOCKED && mutex->owner_thread!=uthread_self())
	{
		ENABLE_SIGALRM(time);
		return MUTEX_LOCKED;
	}
	if(mutex->state==UNLOCKED)
	{
		ENABLE_SIGALRM(time);
		return MUTEX_UNLOCKED;
	}
	mutex->owner_thread = NO_THREAD;
	mutex->state = UNLOCKED;
	ENABLE_SIGALRM(time);
	return MUTEX_SUCCESS;
}
