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
#include <stdlib.h>
#include <unistd.h>

#define ROWARRAY 0
#define COLARRAY 1
#define GRIDARRAY 2

// Parameters struct for thread
typedef struct parameters {
    int row;
    int col;
    char* arr;
    int* threadArr;
} parameters;

void readSudoku(char* inFile, char arr[10][10]);
void* checkRow(void* arg);
void* checkCol(void* arg);
void* checkGrid(void* arg);

int main(int argc, char *argv[]) {
    // Confirm user gave args
    if (argc <= 1) {
        puts("Insufficient parameters passed");
        return 1;
    }

    char inFile[255], arr[10][10];
    int threadArr[3][9];
    int gridRow = 0, gridCol = 0;
    inFile[0] = '\0';

    // Prepend file name from argv
    strcat(inFile, "./");
    strcat(inFile, argv[1]);

    readSudoku(inFile, arr);

    // 1 thread per column, 1 thread per line, 1 thread per 3x3 -> 27 threads
    pthread_t tid[27];
    // Idea, create array of 27 parameters, then free them all at the end
    parameters *p[27];
    

    for (int i = 0; i < 9; i++) {
        // Create varibles so its easier assign
        int row = i * 3;
        int col = (i * 3) + 1;
        int grid = (i * 3) + 2;

        p[row] = malloc(sizeof(parameters)); // Row
        p[col] = malloc(sizeof(parameters)); // Column
        p[grid] = malloc(sizeof(parameters)); // Grid

        // Check row
        p[row]->row = i;
        p[row]->col = 0;
        p[row]->threadArr = threadArr[0];
        p[row]->arr = arr[0];
        pthread_create(&tid[row], NULL, checkRow, (void *)p[row]);

        // Check column
        p[col]->col = i;
        p[col]->row = 0;
        p[col]->threadArr = threadArr[0];
        p[col]->arr = arr[0];
        pthread_create(&tid[col], NULL, checkCol, (void *)p[col]);

        // Check grid
        p[grid]->row = gridRow;
        p[grid]->col = gridCol;
        p[grid]->arr = arr[0];
        p[grid]->threadArr = threadArr[0];
        gridCol++;
        if (gridCol == 3) {
            gridRow++;
            gridCol = 0;
        }
        pthread_create(&tid[grid], NULL, checkGrid, (void *)p[grid]);

        // Wait for all threads to finish before allowing next iteration
        pthread_join(tid[row], NULL);
        pthread_join(tid[col], NULL);
        pthread_join(tid[grid], NULL);
        puts(" ");
    }
    for (int i = 0; i < 27; i++) {
        //pthread_join(tid[i], NULL);
        free(p[i]);
        p[i] = NULL;
    }
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
    while ((rd = read(in_desc, c, 100)) != 0) {
        // Add items to array
        token = strtok(c, delimit);
        
        while (token != NULL) {
            if (column == 8) {
                // Move to next row and then get next token
                arr[row][column++] = *token;
                arr[row++][column] = '\0';
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

    free(c);
    c = NULL;
    return;
}

void* checkRow(void* arg) {
    // Row checkers use arr[row][0-9]
    char numbers[] = "1,2,3,4,5,6,7,8,9";
    char currentNum;
    int row = ((struct parameters* )arg)->row;

    // Keep looping until all 9 chars are found
    for (int i = 0; i < 9; i++) {
        currentNum = *(((struct parameters* )arg)->arr + row*9 + i + row);
        // Check if the current number is in the string
        if (strchr(numbers, currentNum) != NULL) {
            printf("current num: %c\n", currentNum);
        } else {
            // Invalid row, exit thread
            
        }
    }

    //printf("checl row%d\n", ((struct parameters* )arg)->row);
    //printf("ok lets check the arr\n%s\n", ((struct parameters *)arg)->arr);
    return NULL;
}

void* checkCol(void* arg) {
    // Column checkers use arr[0-9][col]
    //printf("check col %d\n", ((struct parameters* )arg)->col);
    return NULL;
}

void* checkGrid(void* arg) {
    //printf("grid row %d, ", ((struct parameters*)arg)->row);
    //printf("col grid %d\n", ((struct parameters*)arg)->col);
    return NULL;
}