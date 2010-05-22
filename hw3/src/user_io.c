#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "user_io.h"

int uthread_open(const char* filename, int flags)
{
	return open(filename, flags | O_NONBLOCK);
}

int uthread_pipe(int filedes[2])
{
    int retval = pipe(filedes);
    if(retval) {
	return retval;
    }
    retval = fcntl(filedes[0],F_SETFL,O_NONBLOCK);
    if(retval) {
	close(filedes[0]);
	close(filedes[1]);
	return retval;
    }
    retval = fcntl(filedes[1],F_SETFL,O_NONBLOCK);
    if(retval) {
	close(filedes[0]);
	close(filedes[1]);
	return retval;
    }
    return 0;
}

int uthread_read(int fd, void* buf, size_t count)
{
	int result;
	while (result = read(fd, buf, count) == -1) {
		if(errno != EAGAIN) return -1;
	}

	return result;
}

int uthread_write(int fd, const void* buf, size_t count)
{
	int result;
	while (result = write(fd, buf, count) == -1) {
		if(errno != EAGAIN) return -1;
	}

	return result;
}
