/**
-------------------------------------------------
Authors:    Levi Van Veen
ID:         200852490
Email:      vanv2490@mylaurier.ca
Version     25-10-2022
-------------------------------------------------
*/

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

void readSudoku(char* inFile, char arr[10][10]);

int main(int argc, char *argv[]) {
    // Confirm user gave args
    if (argc <= 1) {
        puts("Insufficient parameters passed");
        return 1;
    }

    char inFile[255];
    char arr[10][10];
    inFile[0] = '\0';

    // Prepend file name from argv
    strcat(inFile, "./");
    strcat(inFile, argv[1]);

    // Read the sudodu file
    readSudoku(inFile, arr);

    
    return 0;
}

// Read sudoku file and put into array
void readSudoku(char* inFile, char arr[10][10]) {
    int in_desc, rd, column = 0, row = 0;
    char* token;
    char delimit[] = " \n\r";
    char *c = malloc(100*sizeof(char)); 

    if ((in_desc = open(inFile, O_RDONLY, 0777)) < 0) {
        perror("File open error");
        return;
    }
    else {
        while ((rd = read(in_desc, c, 100)) != 0) {
            // Add items to array
            printf("line about to be parsed:\n%s\n\n", c);
            token = strtok(c, delimit);
            
            while (token != NULL) {
                if (column == 8) {
                    // Move to next row and then get next token
                    arr[row++][column] = *token;
                    column = 0;
                } else
                    arr[row][column++] = *token;
                token = strtok(NULL, delimit);
            }

            // Overwrite c buffer so last write doesn't repeat chars
            for (int i = 0; i < 100; i++) {
                c[i] = '\0';
            }
        }
    }

    free(c);
    c = NULL;
    return;
}