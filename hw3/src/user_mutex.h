#ifndef __USER_MUTEX_H
#define __USER_MUTEX_H

/**
 * Error codes
 */
#define MUTEX_SUCCESS            0
#define MUTEX_FAILURE           (-1)
#define MUTEX_INVALID           (-2)
#define MUTEX_UNINITIALIZED     (-3)
#define MUTEX_LOCKED            (-4)
#define MUTEX_UNLOCKED          (-5)


typedef struct umutex_struct* uthread_mutex_t;


/**
 * int uthread_mutex_init(uthread_mutex_t* mutex)
 *
 * Description: creates a new mutex. The only type of mutex that you should 
 * implement is a failure checking mutex.
 * Return value: MUTEX_FAILURE if the function fails, MUTEX_SUCCESS otherwise.
 * This function allocates the mutex.
 */

int uthread_mutex_init(uthread_mutex_t* mutex);


/**
 * int uthread_mutex_destroy(uthread_mutex_t mutex)
 *
 * Description: Destroys an existing mutex.The function should fail if the 
 * mutex is uninitialized or is locked. The function should also fail if
 * the mutex is not locked but there are threads waiting for it.
 * Return value: MUTEX_SUCCESS if the function succeeds. 
 *               MUTEX_INVALID if mutex is null
 *               MUTEX_UNINITIALIZED if the mutex is uninitalized
 *               MUTEX_LOCKED if the mutex is locked or there
 *                  threads waiting on this mutex
 *               MUTEX_FAILURE for any other failure
 * This function frees the allocated memory.
 */

int uthread_mutex_destroy(uthread_mutex_t mutex);


/**
 * int uthread_mutex_lock(uthread_mutex_t mutex)
 * 
 * Description: Locks the mutex if it is unlocked.If the mutex is locked, the 
 * thread is suspended (its state is set to THREAD_SUSPENDED) and it is inserted
 * into the mutex's wait queue. The queue should be FIFO.
 * Return value: MUTEX_SUCCESS if the function succeeds
 *               MUTEX_INVALID mutex is NULL
 *               MUTEX_UNINITIALIZED if the mutex is uninitialized
 *               MUTEX_FAILURE if the function fails.
 */

int uthread_mutex_lock(uthread_mutex_t mutex);


/**
 * int uthread_mutex_try_lock(uthread_mutex_t mutex)
 *
 * Description: Locks the mutex if it unlocked. If the mutex is locked the 
 *              function fails.
 * Return value: MUTEX_SUCCESS - if the function succeeded and the mutex was 
 *                               locked
 *               MUTEX_INVALID mutex is NULL
 *               MUTEX_UNINITIALIZED if the mutex is uninitialized
 *               MUTEX_LOCKED - if the function failed because the mutex is
 *                              already locked by another thread
 *               MUTEX_FAILURE - the mutex is already locked by this thread or
 *                              any other failure
 */

int uthread_mutex_try_lock(uthread_mutex_t mutex);


/**
 * int uthread_mutex_unlock(uthread_mutex_t mutex)
 *
 * Description: unlocks the mutex. If the mutex is unlocked or is locked by
 * another thread, the function fails. The function should not cause a thread
 * context switch.
 * Return value: MUTEX_SUCESS - if the function succeeds and the mutex is unlocked
 *               MUTEX_INVALID mutex is NULL
 *               MUTEX_UNINITIALIZED if the mutex is uninitialized
 *               MUTEX_LOCKED - the mutex is locked by another thread
 *               MUTEX_UNLOCKED - if the mutex is unlocked 
 *               MUTEX_FAILURE - any other failure
 */     

int uthread_mutex_unlock(uthread_mutex_t mutex);


#endif /*__USER_MUTEX_H */
