#ifndef __USER_IO_H
#define __USER_IO_H

#include <unistd.h>

/**
 * The functions declared in this file should have the same semantics as 
 * their original Linux counterparts except that blocking calls should
 * block only the current user thread and not the entire process
 */

int uthread_open(const char* filename, int flags);

int uthread_pipe(int filedes[2]);

int uthread_read(int fd, void* buf, size_t count);

int uthread_write(int fd, const void* buf, size_t count);



#endif /*__USER_IO_H */
