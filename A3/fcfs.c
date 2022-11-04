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
    int completion_time;
} threadInfo;

threadInfo* readThreads(char* in_file, int *thread_arr_size);
float calcTurnaround(threadInfo* thread_arr, int thread_arr_size);
float calcWait(threadInfo* thread_arr, int thread_arr_size);
void printInfo(threadInfo* thread_arr, int thread_arr_size, float turn_average, float wait_average);
int threadCompare(const void *v1, const void *v2);

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

    qsort(thread_arr, thread_arr_size, sizeof(threadInfo), threadCompare);

    float turn_average = calcTurnaround(thread_arr, thread_arr_size);
    float wait_average = calcWait(thread_arr, thread_arr_size);

    printInfo(thread_arr, thread_arr_size, turn_average, wait_average);

    // Clean up allocated stuff and return
    free(thread_arr);
    thread_arr = NULL;
    return 0;
}

void printInfo(threadInfo* thread_arr, int thread_arr_size, float turn_average, float wait_average) {
    printf("Thread ID       Arrival Time    Burst Time  Completion Time Turn-Around Time    Waiting Time\n");
    for (int i = 0; i < thread_arr_size; i++) {
        printf("%-24d%-16d%-12d%-16d%-12d%d\n", thread_arr[i].p_id, thread_arr[i].arr_time, thread_arr[i].burst_time, thread_arr[i].completion_time, thread_arr[i].turn_around_time, thread_arr[i].waiting_time);
    }
    printf("The average waiting time: %.2f\n", wait_average);
    printf("The average turn-around time: %.2f\n", turn_average);
}

float calcWait(threadInfo* thread_arr, int thread_arr_size) {
    int total_wait = 0;
    for (int i = 0; i < thread_arr_size; i++) {
        thread_arr[i].waiting_time = thread_arr[i].turn_around_time - thread_arr[i].burst_time;
        total_wait += thread_arr[i].waiting_time;
    }
    return total_wait / (float) thread_arr_size;
}

float calcTurnaround(threadInfo* thread_arr, int thread_arr_size) {
    const int burst_time = 0;
    const int work_time = 1;
    int finished = 0, worked = 0, time = 0, queue_tail = -1, queue_head = 0, turn_around_total = 0;
    int wait_queue[thread_arr_size][2];

    while (!finished) {
        // First, we check if any new threads have arrived, only if not all are in queue
        if (queue_tail != thread_arr_size) {
            for (int i = 0; i < thread_arr_size; i++) {
                if (thread_arr[i].arr_time == time) {
                    wait_queue[i][burst_time] = thread_arr[i].burst_time;
                    wait_queue[i][work_time] = 0;
                    queue_tail++;
                    break;
                }
            }
        }
        // Now we move through the queue array to find what thread to do work on
        worked = 0;
        while (!worked) {
            if (queue_head > queue_tail) {
                finished = 1;
                break;
            }
            if (wait_queue[queue_head][burst_time] > wait_queue[queue_head][work_time]) {
                // Still work to do on thread, increment work counter
                wait_queue[queue_head][work_time]++;
                worked = 1;
            } else {
                // work is done on thread
                turn_around_total += time - thread_arr[queue_head].arr_time;
                thread_arr[queue_head].turn_around_time = time - thread_arr[queue_head].arr_time;
                thread_arr[queue_head].completion_time = time;
                queue_head++;
            }
        }
        time++;
    }
    return turn_around_total / (float)thread_arr_size;
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

int threadCompare(const void *v1, const void *v2) {
    const threadInfo *p1 = (threadInfo *)v1;
    const threadInfo *p2 = (threadInfo *)v2;
    if (p1->arr_time < p2->arr_time)
        return -1;
    else if (p1->arr_time > p2->arr_time)
        return +1;
    else
        return 0;
}