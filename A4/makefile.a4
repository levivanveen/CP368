#Do not edit the contents of this file.
CC = gcc
CFLAGS = -Wall -g -std=gnu99
LDFLAGS = -lrt -lpthread
TARGET = thread_synchronization stack 
OBJFILES = stack.o thread_synchronization.o 
all: $(TARGET)
stack: stack.c
	$(CC) $(CFLAGS) -o stack stack.c $(LDFLAGS)
thread_synchronization: thread_synchronization.c
	$(CC) $(CFLAGS) -o thread_synchronization thread_synchronization.c $(LDFLAGS)
runq1: stack
	./stack
runq2: thread_synchronization
	./thread_synchronization sample3_in.txt
clean:
	rm -f $(OBJFILES) $(TARGET) *~  *.exe *.out
