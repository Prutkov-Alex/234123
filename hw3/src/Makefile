CC = gcc

CFLAGS = -Wall			 	\
	 -Werror			\
	 -g				\
	 -O0				\
	 -DNDEBUG

.PHONY: all

all: libuthreads.a

libuthreads.a: user_threads.o user_io.o user_mutex.o
	ar rcs user_threads.o user_io.o user_mutex.o
user_threads.o: user_threads.c user_threads.h
	$(CC) $(CFLAGS) -c user_threads.c
user_io.o: user_io.c user_io.h
	$(CC) $(CFLAGS) -c user_io.c
user_mutex.o: user_mutex.c user_mutex.h queue.c queue.h
	$(CC) $(CFLAGS) -c user_mutex.c queue.c

clean: 
	rm -rf *.o libuthreads.a doc
doc:
	doxygen Doxyfile; cd doc/latex; make;
