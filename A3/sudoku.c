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
    char* arr[10][10];
    int* threadArr[3][9];
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
        p[i * 3] = malloc(sizeof(parameters)); // Row
        p[(i * 3) + 1] = malloc(sizeof(parameters)); // Column
        p[(i * 3) + 2] = malloc(sizeof(parameters)); // Grid

        // Row checkers use i like arr[i]
        p[i * 3]->row = i;
        p[i * 3]->col = 0;
        p[i * 3]->threadArr = &threadArr;
        p[i * 3]->arr = &arr;
        pthread_create(&tid[i * 3], NULL, checkRow, (void *)p[i * 3]);

        // Column checkers use i like arr[0][i]
        p[(i * 3) + 1]->col = i;
        p[(i * 3) + 1]->row = 0;
        p[(i * 3) + 1]->threadArr = threadArr;
        p[(i * 3) + 1]->arr = arr;
        pthread_create(&tid[(i * 3) + 1], NULL, checkCol, (void *)p[(i * 3) + 1]);

        p[(i * 3) + 2]->row = gridRow;
        p[(i * 3) + 2]->col = gridCol;
        p[(i * 3) + 2]->arr = arr;
        p[(i * 3) + 2]->threadArr = threadArr;
        gridCol++;
        if (gridCol == 3) {
            gridRow++;
            gridCol = 0;
        }
        pthread_create(&tid[(i * 3) + 2], NULL, checkGrid, (void *)p[(i * 3) + 2]);

        // Wait for all threads to finish before allowing next iteration
        pthread_join(tid[i * 3], NULL);
        pthread_join(tid[(i * 3) + 1], NULL);
        pthread_join(tid[(i * 3) + 2], NULL);
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
    printf("checl row%d\n", ((struct parameters* )arg)->row);
    printf("ok lets check the arr\n%s\n", ((struct parameters *)arg)->arr);
    return NULL;
}

void* checkCol(void* arg) {
    printf("check col %d\n", ((struct parameters* )arg)->col);
    return NULL;
}

void* checkGrid(void* arg) {
    printf("grid row %d, ", ((struct parameters*)arg)->row);
    printf("col grid %d\n", ((struct parameters*)arg)->col);
    return NULL;
}