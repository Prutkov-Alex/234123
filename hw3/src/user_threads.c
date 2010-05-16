/**
 * @file   user_threads.c
 * @author Dmitry Zbarski <dmitry.zbarski@gmail.com>
 * @date   Sun May 16 18:52:48 2010
 * 
 * @brief  Userspace threads implementation.
 * 
 * This is implementation of userspace threads as defined in homework 3.
 */
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <sched.h>
#include <stdlib.h>
#include "user_threads.h"

#define THREAD_RUNNING 0
#define THREAD_SUSPENDED 1
#define THREAD_ZOMBIE 2

typedef struct thread_struct_t thread_t;
typedef struct thread_id_list_struct_t thread_id_list_t;

/** 
 * thread_t structure represents thread.
 * This structure holds information related to thread.
 */
struct thread_struct_t {
    thread_id id;		/**< thread id */
    sigjmp_buf stack;		/**< holds stack, used by sigsetjmp/siglongjmp */
    int state;			/**< one of THREAD_RUNNING, THREAD_SUSPENDED or
				 * THREAD_ZOMBIE */
    int waiting_flag;		/**< flag specifies whether some other thread
				 * joined this thread */
    thread_t *waiting_thread;	/**< pointer to thread which called join on
				 * this thread */
    int *waiting_retval_ptr;	/**< pointer to address where return value of
				 * this thread should be stored */
    int retval;			/**< return value of this thread, stored if no
				 * thread joined this thread before it exited */

    thread_t *next;		/**< pointer to next thread in circular list of
				 * all threads */
    thread_t *prev;		/**< pointer to previous thread in circular
				 * list of all threads */
};

/** 
 * thread_id_list_t structure represents thread id list.
 * This structure holds thread ids in circular linked list.
 */
struct thread_id_list_struct_t {
    thread_id id;		/**< holds thread id */
    thread_id_list_t *next;	/**< pointer to next element in list */
};

/// threads points to start of threads collection
static thread_t *threads;
/// current points to current thread that is being executed
static thread_t *current;
/// thread_id_list holds list of thread ids that exist
static thread_id_list_t *thread_id_list;
/// remaining time holds the remaining time slice of thread in case uthread is
/// disabled
static int remaining_time;

/** 
 * This function allocates and stores new thread id in global list of thread
 * ids.
 * 
 * @param id new thread id
 * 
 * @return UTHREAD_SUCCESS if function succeeds, UTHREAD_FAIL otherwise.
 */
int uthread_id_list_add(thread_id id) {
    thread_id_list_t *id_list = thread_id_list;
    thread_id_list_t *new_id = malloc(sizeof(thread_id_list_t));
    if(new_id == NULL)
	return UTHREAD_FAIL;

    while(id_list->next != thread_id_list)
	id_list = id_list->next;
    new_id->next = id_list->next;
    id_list->next = new_id;
    return UTHREAD_SUCCESS;
}

/** 
 * This function removes thread id from global list of thread ids and frees any
 * memory that was allocated to store it.
 * 
 * @param id thread id to remove
 * 
 * @return UTHREAD_SUCCESS if thread id was found and deleted, UTHREAD_FAIL
 * otherwise.
 */
int uthread_id_list_remove(thread_id id) {
    
}

/** 
 * Find minimal unused thread id. Does this using scan of global list of thread
 * ids.
 * 
 * 
 * @return minimal unused thread id. 
 */
int uthread_new_id() {
    thread_id_list_t *id_list;
    int new_id = 0;
    for(id_list=thread_id_list;id_list!=thread_id_list;id_list=id_list->next) {
	if(id_list->id != new_id) {
	    uthread_id_list_add(new_id);
	    return new_id;
	}
	new_id++;
    }
    uthread_id_list_add(++new_id);
    return new_id;
}

/** 
 * Disable temporary multi threading. This is done by disabling scheduler, which
 * is run in alarm signal handler. Remaining time slice is stored in global
 * variable remaining_time.
 * 
 */
void uthread_disable() {
    remaining_time = ualaram(0,0);
    return;
}

/** 
 * Enables back multi threading. This is done by reinstalling alarm timer with
 * time taken from remaining_time (therefore restores time slice). To prevent
 * threads starvation remaining time is decremented by one.
 * 
 */
void uthread_enable() {
    if(!remaining_time)
	remaining_time = 2;
    ualaram(--remaining_time,0);
    return;
}

/** 
 * Resets alarm timer to generate signal after time slice. Typically called by
 * scheduler to start new thread's time slice.
 * 
 */
void uthread_reset_alarm() {
    ualaram(TIME_SLICE,0);
}

/** 
 * Alarm signal handler. This function is a main scheduler function. When alarm
 * signal is recieved, this function saves executing thread state and restores
 * executing state of next thread to run. If there are no threads in
 * THREAD_RUNNING state, then process yields processor.
 * 
 * @param signum signal number that process recieved
 */
void uthread_alarm_handler(int signum) {
    thread_t *prev = current;
    if(sigsetjmp(current->stack,1)) {
	uthread_reset_alarm();
	return;
    }
    current = current->next;
    while(current != prev) {
	if(current->state == THREAD_RUNNING)
	    siglongjmp(current->stack,1);
	current = current->next;
    }
    /* TODO: No runnable threads. How to yield? */
    sched_yield();
}

/** 
 * This function checks if there is another thread that called uthread_join on
 * thread in question, and if there is then the return value of thread in
 * question is copied to supplied pointer to uthread_join call. After that wakes
 * up thread that was waiting and frees memory of thread that finished it's run.
 * 
 * @param thread thread that finished it's execution
 * @param retval return value of thread
 */
void uthread_notify_exit(thread_t *thread, int retval) {
    if(thread->waiting_flag) {
	if(thread->waiting_retval_ptr != NULL)
	    *thread->waiting_retval_ptr = retval;
	thread->waiting_thread->state = THREAD_RUNNING;
	uthread_free(thread);
	return;
    }
}    

/** 
 * This is the main initialization function. It initializes global lists of
 * threads and thread ids. Creates primary thread. Installs threads scheduler
 * and gives first time slice to primary thread.
 * 
 * 
 * @return UTHREAD_SUCCESS if initialization succeeds, UTHREAD_FAIL otherwise.
 */
int uthread_init() {
    thread_t *primary = malloc(sizeof(thread_t));
    if(primary == NULL) {
	return UTHREAD_FAIL;
    }
    thread_id_list = malloc(sizeof(thread_id_list_t));
    if(thread_id_list == NULL) {
	free(primary);
	return UTHREAD_FAIL;
    }
    thread_id_list->id = 0;
    thread_id_list->next = thread_id_list;
    primary->id = 0;
    primary->state = THREAD_RUNNING;
    primary->waiting_flag = 0;
    primary->waiting_thread = NULL;
    primary->next = primary;
    primary->prev = primary;

    if(signal(SIGALRM, uthread_alarm_handler) == SIG_ERR) {
	free(primary);
	return UTHREAD_FAIL;
    }
    uthread_reset_alarm();

    threads = primary;
    current = primary;
    return UTHREAD_SUCCESS;
}

/** 
 * Allocates new thread structure, initializes it with default values and adds
 * to the and of threads queue.
 * 
 * 
 * @return pointer to new thread_t structure or NULL if allocation failed.
 */
thread_t *uthread_alloc() {
    thread_t *thread = malloc(sizeof(thread_t));
    if(thread == NULL)
	return NULL;
    thread->id = uthread_new_id();
    thread->state = THREAD_RUNNING;
    thread->waiting_flag = 0;
    thread->waiting_thread = NULL;
    thread->waiting_retval_ptr = NULL;

    /* Add new thread to the end of list, so that new thread will start
     * running after all existing threads */
    thread->next = current;
    thread->prev = current->prev;
    current->prev->next = thread;
    current->prev = thread;
    return thread;
}

/** 
 * This function is a wrapper to start new threads. It calls start_routine with
 * arg argument and after the execution finished, update thread_t structure
 * associated with thread. If no thread called uthread_join before start_routine
 * finished, then this function makes thread zombie.
 * 
 * @param thread newly allocated thread_t structure for this thread
 * @param start_routine main function of new thread
 * @param arg argument to pass to start_routine when calling it
 */
void uthread_spawn(thread_t *thread, UTHREAD_START_ROUTINE start_routine, void *arg) {
    int retval;
    retval = start_routine(arg);
    uthread_notify_exit(thread,retval);
    thread->state = THREAD_ZOMBIE;
    thread->retval = retval;
    return;
}

/** 
 * Search and return thread_t structure identified by thread_id.
 * 
 * @param tid id of thread to find
 * 
 * @return thread_t structure of thread with tid id. If no thread with such id
 * exists then NULL is returned.
 */
thread_t *uthread_find(thread_id tid) {
    thread_t *thread;
    for(thread = threads; thread = thread->next ;thread != threads)
	if(thread->id == tid)
	    return thread;
    return NULL;
}


thread_id uthread_create(UTHREAD_START_ROUTINE start_routine,void* arg) {
    thread_t *thread;

    if(threads == 0) {
	if(uthread_init() == UTHREAD_FAIL)
	    return UTHREAD_FAIL;
    }
    uthread_disable();

    thread = uthread_alloc();

    /* We save this point as new thread execution start. When we return here,
     * we spawn the thread and start it's execution. */
    if(sigsetjmp(thread->stack,1)) {
	/* Execute this if we returned with siglongjmp. This will happen when
	 * jumping from alarm handler. */
	uthread_spawn(thread,start_routine,arg);
    }
    return thread->id;
}

void uthread_exit(int retval) {
    thread_t *thread;
    uthread_disable();
    thread = current;
    current->prev->next = current->next;
    current->next->prev = current->prev;
    current = current->prev;
    uthread_notify_exit(thread, retval);
    if(thread == threads)
	threads = threads->next;
    free(thread);
    uthread_alarm_handler(SIGALRM);
}

thread_id uthread_self() {
    return current->id;
}

int uthread_join(thread_id th, int* thread_return) {
    thread_t *thread;
    uthread_disable();
    thread = uthread_find(th);
    if(thread == NULL) {
	uthread_enable();
	return UTHREAD_INVALID;
    }
    thread->waiting_flag = 1;
    thread->waiting_retval_ptr = thread_return;
    thread->waiting_thread = current;
    current->state = THREAD_SUSPENDED;
    uthread_alarm_handler(SIGALRM);
    return UTHREAD_SUCCESS;
}

int uthread_cancel(thread_id th) {
    thread_t *thread;
    uthread_disable();
    thread = current;
    current->prev->next = current->next;
    current->next->prev = current->prev;
    current = current->prev;
    uthread_notify_exit(thread, UTHREAD_CANCELLED);
    if(thread == threads)
	threads = threads->next;
    free(thread);
    uthread_alarm_handler(SIGALRM);
}

int uthread_yield() {
    uthread_disable();
    uthread_alarm_handler(SIGALRM);
}
