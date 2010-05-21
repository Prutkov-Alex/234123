#include <stdio.h>
#include "user_threads.h"
#include "user_io.h"

int global = 0;

void worker();

int main(int argc, char** argv)
{
	int i;
	int filedes[2];
	thread_id self_id = uthread_self();
	
	uthread_pipe(filedes);
	uthread_write(filedes[1], &self_id, sizeof(thread_id));

	for(i = 0; i < 19; i++){
		thread_id uthread_create(worker, filedes);
	}

	// When the main thread dies what will be with it's children? 
	// Should we wait till all other threads finish?


	return 0;
}

void worker(int filedes[2])
{
	int i;
	thread_id other_id, self_id = uthread_self();

	for(i = 0; i < 10; i++) {
		uthread_read(filedes[0], &other_id, sizeof(thread_id));

		printf("Thread Id= %d global= %d got from pipe %d", self_id, global, other_id);
		
		global++;
		uthread_write(filedes[1], &self_id, sizeof(thread_id));
		uthread_yield();
	}
}
