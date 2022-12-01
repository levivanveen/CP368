/**
-------------------------------------------------
Authors:    Levi Van Veen, Vandan Alpesh Thacker
ID:         200852490, 203220140
Email:      vanv2490@mylaurier.ca
Version     25-10-2022
-------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int MAXINPUT = 25;

int** readFile(char *fileName, int resourceCount, int* processCount);
void copyArr(int** copyThis, int** intoThis, int rows, int cols);
int** createTwoDimArr(int rows, int cols);
void printTwoDimArr(int** twoDimArr, int rows, int cols, char* name);
void printStartingStats(int* available, int** maxArr, int r, int c);
char* getInput();
void printAvailable(int* available, int c, int newLine);
void runBankersAlgo(int **maxArr, int **needArr, int **allocArr, int *available, int r, int c);

int main(int argc, char* argv[]) {
    // Confirm user gave args
    if (argc <= 4) {
        puts("Insufficient parameters passed");
        return 1;
    }
    int processCount = 0, resourceCount = argc  - 1;
    char *fileName = "./sample_in_banker.txt";

    // Read file and get max array
    int** maxArr = readFile(fileName, resourceCount, &processCount);
    if (!processCount) {
        printf("Error, no processes in file");
        return 1;
    }

    // initialize all other arrays
    int available[resourceCount];
    for (int i = 1; i < argc; i++)
        available[i - 1] = atoi(argv[i]);
    int** allocArr = createTwoDimArr(processCount, resourceCount);
    int** needArr = createTwoDimArr(processCount, resourceCount);

    copyArr(maxArr, needArr, processCount, resourceCount);

    // Now we have all arrays initiallized, we can run the bankers algo
    printStartingStats(available, maxArr, processCount, resourceCount);
    runBankersAlgo(maxArr, needArr, allocArr, available, processCount, resourceCount);

    free(maxArr);
    maxArr = NULL;
    free(needArr);
    needArr = NULL;
    /* free(allocArr);
    allocArr = NULL; */
}

// Reads the file of processes and returns the number of processes in file
int** readFile(char *fileName, int resourceCount, int* processCount) {
    int **maxArr = malloc(sizeof(int*) * 1);
    char* delimit = ", \n\r";
    FILE *in = fopen(fileName, "r");

    if (!in) {
        printf("Error opening input file\n");
        return NULL;
    }
    while (!feof(in)) {
        char* token;
        char line[20];

        // Go through file line by line
        if (fgets(line, 20, in) != NULL) {
            int resourceLine[resourceCount];
            token = strtok(line, delimit);

            int i = 0;
            while (token != NULL) {
                resourceLine[i] = atoi(token);
                token = strtok(NULL, delimit);
                i++;
            }
            (*processCount)++;
            // maxArr is initiallized with size one, only realloc if greater than 1
            if (*processCount != 1) {
                maxArr = realloc(maxArr, *processCount * sizeof(int*));
            }
            maxArr[*processCount - 1] = malloc(resourceCount * sizeof(int));
            for (int j = 0; j < resourceCount; j++) {
                maxArr[*processCount - 1][j] = resourceLine[j];
            }
        }
    }
    return maxArr;
}

void runBankersAlgo(int **maxArr, int **needArr, int **allocArr, int *available, int r, int c) {
    while (1) {
        char* input = getInput();
        char inputCopy[MAXINPUT];
        strcpy(inputCopy, input);

        char* delimit = " \n";
        char* token = strtok(inputCopy, delimit);
        // Check user input
        if (strcmp(token, "Status") == 0) {
            printAvailable(available, c, 1);
            printTwoDimArr(maxArr, r, c, "Maximum Resources");
            printTwoDimArr(allocArr, r, c, "Allocated Resources");
            printTwoDimArr(needArr, r, c, "Need Resources");
        }
        else if (strcmp(token, "RQ") == 0) {
            printf("token is RQ\n");
        }
        else if (strcmp(token, "Run") ==0) {
            printf("token is Run\n");
        }
        else if (strcmp(token, "RL") ==0) {
            printf("token is RL\n");
        }
        else if (strcmp(token, "Exit") ==0) {
            printf("User wants to exit\n");
            return;
        }
        else {
            printf("Invalid input, use one of RQ, RL, Status, Run, Exit\n");
        }
    }
}

char* getInput() {
    char line[MAXINPUT];
    printf("Enter Commmand: ");
    if (fgets(line, MAXINPUT, stdin) ==0) {
        puts("EOF Occurred");
        return NULL;
    }
    char* cpy = malloc(sizeof(char) * strlen(line));
    strcpy(cpy, line);
    return cpy;
}

void copyArr(int** copyThis, int** intoThis, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            intoThis[i][j] = copyThis[i][j];
        }
    }
    return;
}

void printStartingStats(int* available, int** maxArr, int r, int c) {
    printf("Number of Customers: %d\n", r);
    printf("Currently ");
    printAvailable(available, c, 0);
    printTwoDimArr(maxArr, r, c, "Maximum resources from file");
}

void printAvailable(int* available, int c, int newLine) {
    printf("Available Resources: ");
    if (newLine) printf("\n");
    for (int i = 0; i < c; i++) {
        printf("%d", available[i]);
        if (c-1 != i) printf(" ");
        else printf("\n");
    }
}

int** createTwoDimArr(int rows, int cols) {
    int **c = (int**)malloc(sizeof(int*)*rows);
    for(int i=0; i < rows; i++)
        *(c+i) = (int*)malloc(sizeof(int)*cols);
    return c;
}

void printTwoDimArr(int** twoDimArr, int rows, int cols, char* name) {
    printf("%s:\n", name);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", twoDimArr[i][j]);
        }
        printf("\n");
    }
    return;
}
