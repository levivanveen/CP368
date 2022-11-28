#Do not edit the contents of this file.
CC = gcc
CFLAGS = -Werror -Wall -g -std=gnu99
LDFLAGS = -lrt -lpthread
TARGET = banker 
OBJFILES = banker.o  
all: $(TARGET)
banker: banker.c
	$(CC) $(CFLAGS) -o banker banker.c $(LDFLAGS)
runq1: banker
	./banker 10 5 7 8
clean:
	rm -f $(OBJFILES) $(TARGET)  $(TARGET) *.exe  *~ *.out