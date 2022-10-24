#Do not edit the contents of this file.
CC = gcc
CFLAGS = -Werror -Wall -g -std=gnu99
TARGET = filecopy  directory
OBJFILES = filecopy.o directory.o

all: $(TARGET)

filecopy: filecopy.c
	$(CC) $(CFLAGS) -o filecopy filecopy.c
	
directory: directory.c
	$(CC) $(CFLAGS) -o directory directory.c 
	
runq1: directory.c
	./directory

runq2-err: filecopy.c
	 ./filecopy 

runq2: filecopy.c
	 ./filecopy input.txt output.txt
	 
runq2-strace: filecopy.c
	 strace -c ./filecopy input.txt output.txt
	 
runq2-macOS: filecopy.c
	 sudo dtruss -c ./filecopy input.txt output.txt
	 
clean:
	rm -rf $(OBJFILES) $(TARGET) *~  *.exe *.dSYM
