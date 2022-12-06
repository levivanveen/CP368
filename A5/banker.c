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
#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct customer {
    int* alloc;
    int* need;
    sem_t runThread; 
} customer;

typedef struct threadInfo {
    customer *current;
    customer *next;
    int* available;
    int id;
    int resourceCount;
} threadInfo;

const int MAXINPUT = 25;

int** readFile(char *fileName, int resourceCount, int* processCount);
void copyArr(int** copyThis, int** intoThis, int rows, int cols);
int** createTwoDimArr(int rows, int cols);
void printTwoDimArr(int** twoDimArr, int rows, int cols, char* name);
void printArr(int* arr, int cols);
void printStartingStats(int* available, int** maxArr, int r, int c);
char* getInput();
void printAvailable(int* available, int c, int newLine);
void runBankersAlgo(int **maxArr, int **needArr, int **allocArr, int *available, int r, int c);
int checkReq(char* input, int* available, int** needArr, int r, int c);
int safeReq(int available, int need, int request);
int safetyAlgo(int* available, int processCount, int resourceCount, int** allocArr, int** needArr, char* input);
int releaseResources(int* available, int processCount, int resourceCount, int** allocArr, int** needArr, char* input);
int findSequence(int* sequence, int* available, int processCount, int resourceCount, int** needArr, int** allocArr);
void freeAllocated(int* available, int* needArr, int* allocArr, int resourceCount);
void* runCustomer(void* arg);

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
    free(allocArr);
    allocArr = NULL;
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
/**
 * r is number of rows in 2d arrays, each row being a process
 * c is number of columns in a process, each column being a resource
 */ 
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
            if (safetyAlgo(available, r, c, allocArr, needArr, input))
                printf("State is safe, and request is satisfied\n");
            else
                printf("request is denied\n");
        }
        else if (strcmp(token, "Run") ==0) {
            int finish[r];
            for (int i = 0; i < r; i++) {
                finish[i] = -1;
            }
            if (findSequence(finish, available, r, c, needArr, allocArr) == 0)
                printf("request is denied\n");
            printf("Safe Sequence is: ");
            printArr(finish, r);

            //Initiallize an array of customers, and set semaphores
            customer customers[r];
            threadInfo *thread[r];
            pthread_t tid[r];

            for (int i = 0; i < r; i++) {
                if (!i)
                    sem_init(&customers[i].runThread, 0, 1);
                else
                    sem_init(&customers[i].runThread, 0, 0);
            }
            for (int i = 0; i < r; i++) {
                thread[i] = malloc(sizeof(threadInfo));

                customers[i].alloc = allocArr[finish[i]];
                customers[i].need = needArr[finish[i]];
                thread[i]->available = available;
                thread[i]->current = &customers[i];
                thread[i]->id = finish[i];
                thread[i]->resourceCount = c;

                if (i == r - 1) {
                    thread[i]->next = NULL;
                } else {
                    thread[i]->next = &customers[i + 1];
                }

                pthread_create(&tid[i], NULL, runCustomer, (void *)thread[i]);
            }
            for (int i = 0; i < r; i++) {
                pthread_join(tid[i], NULL);
                free(thread[i]);
                thread[i] = NULL;
            }
        }
        else if (strcmp(token, "RL") ==0) {
            if (releaseResources(available, r, c, allocArr, needArr, input))
                printf("State is safe, and request is satisfied\n");
            else
                printf("request is denied\n");
        }
        else if (strcmp(token, "Exit") ==0) {
            printf("Exiting the program\n");
            return;
        }
        else {
            printf("Invalid input, use one of RQ, RL, Status, Run, Exit\n");
        }
        free(input);
        input = NULL;
    }
}

void* runCustomer(void* arg) {
    int id = ((threadInfo *)arg)->id;
    int resourceCount = ((threadInfo *)arg)->resourceCount;
    sem_wait(&(((threadInfo *)arg)->current->runThread));
    printf("--> Customer/Thread %d\n", id);
    printf("    Allocated resources: ");
    printArr(((threadInfo *)arg)->current->alloc, resourceCount);
    printf("    Needed: ");
    printArr(((threadInfo *)arg)->current->need, resourceCount);
    printf("    Available: ");
    printArr(((threadInfo *)arg)->available, resourceCount);
    puts("    Thread has started\n    Thread has finished");
    //Run thread
    puts("    Thread is releasing resources");
    // Free allocated resources
    freeAllocated(((threadInfo *)arg)->available, ((threadInfo *)arg)->current->need, ((threadInfo *)arg)->current->alloc, resourceCount);
    printf("    New Available: ");
    printArr(((threadInfo *)arg)->available, resourceCount);

    // Let the next thread run
    if (((threadInfo *)arg)->next != NULL)
        sem_post(&(((threadInfo *)arg)->next->runThread));
    return NULL;
}

int checkReq(char* input, int* available, int** needArr, int r, int c) {
    char* delimit = " \n";
    char* token = strtok(input, delimit);
    int processId;
    // Skip over the first, as it is the commmand
    if (token != NULL) token = strtok(NULL, delimit);
    else {
        printf("Improper usage of command, ");
        return 0;
    }
    // Now get process id 
    if (token != NULL) {
        processId = atoi(token);
        token = strtok(NULL, delimit);
    } else {
        printf("Improper usage of command, ");
        return 0;
    }
    
    // If process id is greater than row count, dont use it
    if (processId >= r || processId < 0) {
        printf("Customer doesn't exist, ");
        return 0;
    }

    // Check if each resources can be allocated
    for (int i = 0; i < c; i++) {
        if (token == NULL) {
            printf("Invalid usage of command, ");
            return 0;
        }
        if (isdigit((int)*token)) {
            if (safeReq(available[i], needArr[processId][i], atoi(token)) == 0) {
                printf("State is unsafe, ");
                return 0;
            }
        } else {
            printf("Invalid usage of command, ");
            return 0;
        }
        token = strtok(NULL, delimit);
    }
    if (token != NULL) {
        printf("Only request for resources that exist\n");
    }
    // The input and request are valid
    return 1;
}

int safeReq(int available, int need, int request) {
    if ((request > need) || (request > available)) {
        return 0;
    }
    return 1;
}

int safetyAlgo(int* available, int processCount, int resourceCount, int** allocArr, int** needArr, char* input) {
    // first check if safe req
    char* newInput = (char *) malloc( strlen(input) + 1 ); 
    strcpy(newInput, input);
    int validReq = checkReq(newInput, available, needArr, processCount, resourceCount);
    free(newInput);
    newInput = NULL;
    if (!validReq) {
        return 0;
    }

    // Create new arrays to check for sequence with request
    int** newAlloc = createTwoDimArr(processCount, resourceCount);
    int** newNeed = createTwoDimArr(processCount, resourceCount);
    int newAvailable[resourceCount];
    copyArr(needArr, newNeed, processCount, resourceCount);
    copyArr(allocArr, newAlloc, processCount, resourceCount);
    
    char* delimit = " \n";
    char* token = strtok(input, delimit);
    //skip over command
    token = strtok(NULL, delimit);
    // Get processID
    int processId = atoi(token);
    token = strtok(NULL, delimit);

    for (int i = 0; i < resourceCount; i++) {
        newNeed[processId][i] = newNeed[processId][i] - atoi(token);
        newAlloc[processId][i] = newAlloc[processId][i] + atoi(token);
        newAvailable[i] = available[i] - atoi(token);
        token = strtok(NULL, delimit);
    }

    int finish[processCount];
    for (int i = 0; i < processCount; i++) {
        finish[i] = -1;
    }
    int validSequence = findSequence(finish, available, processCount, resourceCount, newNeed, newAlloc);

    if (!validSequence) {
        return 0;
    }
    copyArr(newNeed, needArr, processCount, resourceCount);
    copyArr(newAlloc, allocArr, processCount, resourceCount);

    for (int i = 0; i < resourceCount; i++) {
        available[i] = newAvailable[i];
    }

    free(newAlloc);
    free(newNeed);
    newNeed = NULL;
    newAlloc = NULL;
    return 1;
}

int findSequence(int* sequence, int* available, int processCount, int resourceCount, int** needArr, int** allocArr) {
    int work[resourceCount];
    for (int i = 0; i < resourceCount; i++) {
        work[i] = available[i];
    }
    int found = 0;
    // Keep looping until all processes have found a way to get resources
    while (found < processCount) {
        int foundSwitch = 0;
        // Loop through processes to see if they can run
        for (int i = 0; i < processCount; i++) {
            // Move to next process if already recieved resources
            int inSequence = 0;
            for (int j = 0; j < processCount; j++) {
                if (i == sequence[j])
                    inSequence = 1;
            }
            if (inSequence)
                continue;

            // Now check if enough work to run the process
            int validSwitch = 1;
            for (int j = 0; j < resourceCount; j++) {
                if (work[j] < needArr[i][j]) {
                    validSwitch = 0;
                    break;
                }
            }
            // If enough work to run process, add it to sequence and increase work
            if (validSwitch) {
                for (int j = 0; j < resourceCount; j++) {
                    work[j] += allocArr[i][j];
                }
                sequence[found] = i;
                foundSwitch = 1;
                found++;
                break;
            }
        }
        if (!foundSwitch) {
            // Can't find any possible sequence
            printf("Can't find any sequence, ");
            return 0;
        }
    }
    return 1;
}

int releaseResources(int* available, int processCount, int resourceCount, int** allocArr, int** needArr, char* input) {
    char* delimit = " \n";
    char* token = strtok(input, delimit);
    int processId;
    // Skip over the first, as it is the commmand
    if (token != NULL) token = strtok(NULL, delimit);
    else {
        printf("Improper usage of command, ");
        return 0;
    }
    // Now get process id 
    if (token != NULL) {
        processId = atoi(token);
        token = strtok(NULL, delimit);
    } else {
        printf("Improper usage of command, ");
        return 0;
    }
    
    // If process id is greater than row count, dont use it
    if (processId >= processCount || processId < 0) {
        printf("Customer doesn't exist, ");
        return 0;
    }

    int newAvailable[resourceCount];
    int** newAlloc = createTwoDimArr(processCount, resourceCount);
    int** newNeed = createTwoDimArr(processCount, resourceCount);

    copyArr(allocArr, newAlloc, processCount, resourceCount);
    copyArr(needArr, newNeed, processCount, resourceCount);

    int validRelease = 1;

    for (int i = 0; i < resourceCount; i++) {
        if (token == NULL) {
            printf("Invalid usage of command, ");
            return 0;
        }
        if (isdigit((int)*token)) {
            if ((newAlloc[processId][i] - atoi(token)) >= 0) {
                newAlloc[processId][i] = newAlloc[processId][i] - atoi(token);
                newNeed[processId][i] = newNeed[processId][i] + atoi(token);
                newAvailable[i] = available[i] + atoi(token);
            } else {
                printf("State is unsafe, ");
                validRelease = 0;
                break;
            }
        } else {
            printf("Invalid usage of command, ");
            validRelease = 0;
            break;
        }
        token = strtok(NULL, delimit);
    }

    if(validRelease) {
        copyArr(newAlloc, allocArr, processCount, resourceCount);
        copyArr(newNeed, needArr, processCount, resourceCount);
        for (int i = 0; i < resourceCount; i++) {
           available[i] = newAvailable[i];
        }
        if (token != NULL) {
            printf("Only request for resources that exist\n");
        }
    }
    
    free(newAlloc);
    free(newNeed);
    newNeed = NULL;
    newAlloc = NULL;
    return validRelease;
}

void freeAllocated(int* available, int* needArr, int* allocArr, int resourceCount) {
    for (int i = 0; i < resourceCount; i++) {
        available[i] = available[i] + allocArr[i];
        needArr[i] = needArr[i] + allocArr[i];
        allocArr[i] = 0;
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

void printArr(int* arr, int cols) {
    for (int i = 0; i < cols; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}
