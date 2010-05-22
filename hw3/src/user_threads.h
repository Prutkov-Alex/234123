#ifndef __USER_THREADS
#define __USER_THREADS


/**
 * Error codes
 */
  
#define UTHREAD_SUCCESS 0
#define UTHREAD_FAIL   -1
#define UTHREAD_INVALID -2


#define UTHREAD_CANCELLED -0x0BAD

/**
 * UTHREAD_DEF_STACK_SIZE is the default stack size for a newly created thread
 */
#define UTHREAD_DEF_STACK_SIZE  4096


/**
 * SECOND the definition of a time slice 
 */
#define MILLISECOND 1000
#define TIME_SLICE 50*MILLISECOND
 
 /**
 * thread_id is a thread identifier
 */

typedef int thread_id;

/**
 * UTHREAD_START_ROUTINE is the function type of a thread start routine.
 * It takes a single void pointer argument. It returns an int which is the
 * return value/ exit code of thread.
 */

typedef int (*UTHREAD_START_ROUTINE)(void*);



/**
 * thread_id uthread_create(UTHREAD_START_ROUTINE start_routine,void* arg)
 *
 * Description: Creates a new thread. The thread executes start_routine. 
 * The argument arg is passed to start_routine as a parameter. The new thread
 * is allocated a new stack of size UTHREAD_DEF_STACK_SIZE. The thread  
 * has access to all of the resources of the process. In particular it
 * can access all global variables.
 * Return value: The new process's TID is returned if the function is successful.
 * Otherwise, UTHREAD_FAIL is returned.
 */

thread_id uthread_create(UTHREAD_START_ROUTINE start_routine,void* arg);

/**
 * void uthread_exit(int retval)
 * 
 * Description: Kills the current thread. The threads exit code is retval.
 * If this thread is the last thread of this process the process exits with
 * return value retval. Otherwise, the process must not be killed.
 */

void uthread_exit(int retval);

/**
 * thread_id uthread_self()
 *
 * Description: returns the thread identifier of the currently running
 * thread.
 */

thread_id uthread_self();

/**
 * int uthread_join(thread_id th, int* thread_return)
 *
 * Description: Suspends the current thread until the thread whose identifier
 * is th is finished. A thread can be joined by only one thread. If there is no 
 * process that is willing to run (this would be a strong indicator of a deadlock) 
 * the process should yield the processor. thread_return may be NULL - in that 
 * case the thread return value is not kept.
 * Return value: UTHREAD_SUCCESS if successful; UTHREAD_INVALID if there is no 
 * thread whose identifier is th; UTHREAD_FAIL in case of any other error.
 *
 */

int uthread_join(thread_id th, int* thread_return);


/**
 * int uthread_cancel(thread_id th)
 *
 * Description: Kills the thread whose identifier is th. If there is 
 * another thread that called uthread_join() on the thread that is being
 * killed it should return from suspension. The return value of the killed
 * thread should be UTHREAD_CANCELLED.
 * Return value: UTHREAD_SUCCESS if the thread was killed successfully;
 * UTHREAD_INVALID if there is no thread whose identifier is th;
 * UTHREAD_FAIL in case of any other error/
 */

int uthread_cancel(thread_id th);

/**
 * int uthread_yield()
 *
 * Description: The current thread is replaced by another thread of the
 * the current process. If there is no other thread that is willing to run
 * the function returns immediately. The thread that is selected to run is
 * the thread with the minimal id whose state is THREAD_RUNNING and has a thread
 * identifier larger than the current thread (RR). If there is no such thread, the thread
 * with the minimal thread id that is smaller than the current thread's id
 * should be selected (cyclic order). 
 * Return value: UTHREAD_SUCCESS if there was a thread context-switch.
 * UTHREAD_FAIL othewise.  
 */
int uthread_yield();


#endif /* __USER_THREADS */
