# Do not edit the contents of this file.
CC = gcc
CFLAGS = -Werror -Wall -g  -std=gnu99
LDFLAGS = -lrt 
TARGET =  assignment_average  process_management
OBJFILES =  assignment_average.o process_management2.0

all:  $(TARGET)

process_management: process_management.c
	$(CC) $(CFLAGS)  -o process_management process_management.c $(LDFLAGS)

assignment_average: assignment_average.c
	$(CC) $(CFLAGS)  -o assignment_average  assignment_average.c

runq1: assignment_average
	./assignment_average sample_in_grades.txt
	
runq2: process_management
	./process_management sample_in_process.txt 

clean: *.c
	rm -rf assignment_average process_management2 z_creator z_terminator  *.exe *.dSYM output.txt redirected_output.txt
