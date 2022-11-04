/**
-------------------------------------------------
Authors:    Levi Van Veen, Vandan Alpesh Thacker
ID:         200852490, 203220140
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
void printSudoku(char arr[10][10]);
void checkValidSudoku(int arr[3][9]);
void* checkRow(void* arg);
void* checkCol(void* arg);
void* checkGrid(void* arg);
char* findChar(char* numbers, char currentNum);

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
    printSudoku(arr);

    // 1 thread per column, 1 thread per line, 1 thread per 3x3 -> 27 threads
    pthread_t tid[27];
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
        p[grid]->row = gridRow * 3;
        p[grid]->col = gridCol * 3;
        p[grid]->arr = arr[0];
        p[grid]->threadArr = threadArr[0];
        gridCol++;
        if (gridCol == 3) {
            gridRow++;
            gridCol = 0;
        }
        pthread_create(&tid[grid], NULL, checkGrid, (void *)p[grid]);
    }

    // Wait for all threads to exit, then free all parameters
    for (int i = 0; i < 27; i++) {
        pthread_join(tid[i], NULL);
        free(p[i]);
        p[i] = NULL;
    }
    checkValidSudoku(threadArr);
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

void printSudoku(char arr[10][10]) {
    puts("Sudoku Puzzle Solution is:");
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (j == 8)
                printf("%c", arr[i][j]);
            else
                printf("%c ", arr[i][j]);
        }
        printf("\n");
    }
    return;
}

void checkValidSudoku(int arr[3][9]) {
    // Check threads return for validity
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 9; j++) {
            if (arr[i][j] != 1) {
                puts("Sudoku puzzle is invalid");
                return;
            }
        }
    }
    puts("Sudoku puzzle is valid");
}

void* checkRow(void* arg) {
    // Row checkers use arr[row][0-9]
    char numbers[] = "123456789";
    char currentNum;
    int row = ((struct parameters* )arg)->row;

    for (int i = 0; i < 9; i++) {
        // Row * 10 brings to row, add i to bring to index in row
        currentNum = *(((struct parameters* )arg)->arr + row*10 + i);
        if (findChar(numbers, currentNum) == NULL) {
            // Invalid row, exit thread
            *(((struct parameters* )arg)->threadArr + ROWARRAY * 9 + row) = 0;
            return NULL;
        } 
        
    }
    *(((struct parameters* )arg)->threadArr + ROWARRAY * 9 + row) = 1;
    return NULL;
}

void* checkCol(void* arg) {
    // Column checkers use arr[0-9][col]
    char numbers[] = "123456789";
    char currentNum;
    int col = ((struct parameters* )arg)->col;

    for (int i = 0; i < 9; i++) {
        // i * 10 brings to row, add col to bring to col index in row
        currentNum = *(((struct parameters* )arg)->arr + col + i*10);
        if (findChar(numbers, currentNum) == NULL) {
            // Invalid col, exit thread
            *(((struct parameters* )arg)->threadArr + COLARRAY * 9 + col) = 0;
            return NULL;
        } 
    }
    *(((struct parameters* )arg)->threadArr + COLARRAY * 9 + col) = 1;
    return NULL;
}

void* checkGrid(void* arg) {
    char numbers[] = "123456789";
    char currentNum;
    int col = ((struct parameters* )arg)->col;
    int row = ((struct parameters* )arg)->row;

    for (int i = 0; i < 9; i++) {
        /**
         * col can be 0,3,6 & i%3 can be 0,1,2
         */ 
        currentNum = *(((struct parameters* )arg)->arr + (col + i%3) + ((row + i/3)*10));
        if (findChar(numbers, currentNum) == NULL) {
            // Invalid grid, exit thread
            *(((struct parameters* )arg)->threadArr + GRIDARRAY*9 + col/3 + row) = 0;
            return NULL;
        } 

    }
    *(((struct parameters* )arg)->threadArr + GRIDARRAY*9 + col/3 + row) = 1;
    return NULL;
}

char* findChar(char* string, char currentNum) {
    char* foundChar;
    int index;
    if ((foundChar = strchr(string, currentNum)) != NULL) {
        index = (int)(foundChar - string);
        memmove(&string[index], &string[index + 1], strlen(string) - index);
    }
    return foundChar;
} 