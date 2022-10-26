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
    //char* arr[10][10];
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
    //, threadArr[3][9];
    int gridRow = 0, gridCol = 0;
    inFile[0] = '\0';

    // Prepend file name from argv
    strcat(inFile, "./");
    strcat(inFile, argv[1]);

    readSudoku(inFile, arr);

    // 1 thread per column, 1 thread per line, 1 thread per 3x3 -> 27 threads
    pthread_t tid[27];
    // Idea, create array of 27 parameters, then free them all at the end
    

    for (int i = 0; i < 9; i++) {
        parameters *row = (parameters *)malloc(sizeof(parameters));
        parameters *col = (parameters *)malloc(sizeof(parameters));
        parameters *grid = (parameters *)malloc(sizeof(parameters));

        // Row checkers use i like arr[i]
        row->row = i;
        row->col = 0;
        //row->arr = arr;
        pthread_create(&tid[i * 3], NULL, checkRow, (void *)row);

        // Column checkers use i like arr[0][i]
        col->col = i;
        col->row = 0;
        //col->arr = arr;
        pthread_create(&tid[(i * 3) + 1], NULL, checkCol, (void *)col);

        grid->row = gridRow;
        grid->col = gridCol;
        gridCol++;
        //grid->arr = arr;
        if (gridCol == 3) {
            gridRow++;
            gridCol = 0;
        }
        pthread_create(&tid[(i * 3) + 2], NULL, checkGrid, (void *)grid);

        free(row);
        row = NULL;
        free(col);
        col = NULL;
        free(grid);
        grid = NULL;
    }
    for (int i = 0; i < 27; i++) {
        pthread_join(tid[i], NULL);
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
    printf("ok in check row%d\n", ((struct parameters* )arg)->row);
    return NULL;
}

void* checkCol(void* arg) {
    printf("ok in check %d col\n", ((struct parameters* )arg)->col);
    return NULL;
}

void* checkGrid(void* arg) {
    printf("%d grid row", ((struct parameters*)arg)->row);
    printf("%d grid col\n", ((struct parameters*)arg)->col);
    return NULL;
}