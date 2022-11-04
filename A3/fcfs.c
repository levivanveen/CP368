#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct threadInfo{
    int p_id;
    int arr_time;
    int burst_time;
    int waiting_time;
    int turn_around_time;
} threadInfo;

threadInfo* readThreads(char* in_file, int *thread_arr_size);
void calcTurnaround(threadInfo* thread_arr, int thread_arr_size);
void calcWait(threadInfo* thread_arr, int thread_arr_size);

int main(int argc, char* argv[]) {
    // Confirm user gave args
    if (argc <= 1) {
        puts("Insufficient parameters passed");
        return 1;
    }

    char in_file[255];
    in_file[0] = '\0';

    // Prepend file name from argv
    strcat(in_file, "./");
    strcat(in_file, argv[1]);

    int thread_arr_size = 0;
    threadInfo* thread_arr = readThreads(in_file, &thread_arr_size);

    calcTurnaround(thread_arr, thread_arr_size);

    // Need to 
    for (int i = 0; i < thread_arr_size; i++) {
        printf("\n%d %d %d\n", thread_arr[i].p_id, thread_arr[i].arr_time, thread_arr[i].burst_time);
    }

    // Clean up allocated stuff and return
    free(thread_arr);
    thread_arr = NULL;
    return 0;
}

void calcTurnaround(threadInfo* thread_arr, int thread_arr_size) {
    /*  To calculate the turnaround time
        Do a while loop and for each thread and check when it is done
     */
    int finished = 0;
    int time = 0;
    int wait_queue[thread_arr_size];
    int counter = 0;

    while (!finished) {
        printf("wow");
        // First, we check if any new threads have arrived
        for (int i = 0; i < thread_arr_size; i++) {
            if (thread_arr[i].arr_time == time) {
                wait_queue[i] = thread_arr[i].burst_time;
                counter++;
            }
        }
        if (counter == 6) {
            printf("Done\n");
            finished = 1;
        }
    }
    for (int i = 0; i < 6; i++) {
        printf("ok %d\n", wait_queue[i]);
    }
}

threadInfo* readThreads(char* in_file, int *thread_arr_size) {
    const int buffer_size = 100;
    int in_desc, rd, col = 1, thread_index = 1, num = 0;
    char* token;
    char delimit[] = " \n,";
    char *c = malloc(buffer_size*sizeof(char));

    // Create array of threadInfo's, initialze at size one
    threadInfo *thread_arr = malloc(sizeof(threadInfo) * 1);

    if ((in_desc = open(in_file, O_RDONLY, 0777)) < 0) {
        perror("File open error");
        return NULL;
    }
    while ((rd = read(in_desc, c, buffer_size)) != 0) {
        // Add items to array
        token = strtok(c, delimit);
        
        while (token != NULL) {
            int i = 0;
            while (token[i] != '\0') {
                if(((token[i] - 48) >= 0) && (token[i] - 48 <= 9))
                    num = (num*10) + (token[i] - 48);
                i++;
            }
            if (col == 4)
                col = 1;
            switch(col) {
                case 1:
                    // p_id
                    thread_arr[thread_index - 1].p_id = num;
                    col++;
                    break;
                case 2:
                    // arr_time
                    thread_arr[thread_index - 1].arr_time = num;
                    col++;
                    break;
                case 3:
                    // burst_time
                    thread_arr[thread_index - 1].burst_time = num;
                    col++;

                    // Create room for one more threadInfo in the array
                    thread_arr = realloc(thread_arr, sizeof(threadInfo) * (thread_index + 1));
                    thread_index++;
            }
            num = 0;
            token = strtok(NULL, delimit);
        }
        // Overwrite c buffer so last write doesn't repeat chars
        for (int i = 0; i < buffer_size; i++) {
            c[i] = '\0';
        }
    }
    free(c);
    c = NULL;
    *thread_arr_size = thread_index - 1;
    return thread_arr;
}