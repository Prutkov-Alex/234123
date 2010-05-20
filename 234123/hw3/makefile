CC = gcc


mutex: 		user_mutex.o queue.o user_threads.o

user_mutex.o:	user_mutex.c user_mutex.h queue.h user_threads.h
		$(CC) -c user_mutex.c
queue.o:      	queue.c queue.h 
		$(CC) -c queue.c
user_threads.o: user_threads.c user_threads.h
		$(CC) -c user_threads.c
