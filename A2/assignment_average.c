#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

void fill_arr(char* c, int arr[6][10], int rd);

int main(int argc, char *argv[]) {
    // Check if user gave any args
    if (argc <= 1) {
        puts("Insufficient parameters passed");
        return 1;
    }

    int in_desc, rd;
    char inFN[255];
    char *c = malloc(200*sizeof(char));

    inFN[0] = '\0';
    
    // Prepend file names
    strcat(inFN, "./");
    strcat(inFN, argv[1]);

    // Open input file
    in_desc = open(inFN, O_RDONLY, 0777);
    if (in_desc == -1) {
        fprintf(stderr, "File Open Failed");
        return 1;
    }

    int arr[6][10];

    // Read file and fill 2d array
    if ((rd = read(in_desc, c, 200)) != 0) {
        fill_arr(c, arr, rd);
        free(c);
        c = NULL;
    }
    else {
        return 1;
    }

    // Creating GTA processes
    pid_t gta_pid[3];
    int gta_children = 3;
    int ta_children = 2;
    int chapter[2][10];
    int assignment[10];
    int sum = 0;
    float avg = 0;

    for (int i = 0; i < gta_children; i++) {
        gta_pid[i] = fork();
        if (gta_pid[i] < 0) {
        // Error occurred
            fprintf(stderr,"Fork Failed");
            return 1;
        }
        if (gta_pid[i] == 0) {
            // Gta process takes care of one chapter -> 2 assignments
            memcpy(chapter[0], arr[2*i], sizeof(arr[2*i]));
            memcpy(chapter[1], arr[2*i + 1], sizeof(arr[2*i + 1]));

            // Now that chapter contains the 2 assignments, TA deals with each assignment
            pid_t ta_pid[2];

            // Wait for each child to print before starting next question to get correct output
            for (int j = 0; j < ta_children; j++) {
                ta_pid[j] = fork();
                if (ta_pid[j] < 0) {
                    // Error occurred
                    fprintf(stderr,"Fork Failed");
                    return 1;
                }
                if (ta_pid[j] == 0) {
                    memcpy(assignment, chapter[j], sizeof(chapter[j]));
                    for (int student = 0; student < 10; student++) {
                        sum += assignment[student];
                    }
                    avg = (float)sum / 10;
                    printf("Assignment %d - Average = %.6f\n", i*2+j+1, avg);
                    exit(0);
                }
                wait(NULL);
            }
            exit(0);
        }
        wait(NULL);
    }

    close(in_desc);

    return 0;
}

// fill_arr fills the 2d array with values from input txt file
void fill_arr(char* c, int arr[6][10], int rd) {
    int num = -1;
    int i = 0;
    for (int k = 0; k < 10; k++) {
        int flag = 0;
        int j = 0;
        while (flag == 0) {
            if (c[i] == '\n' || i == rd) {
                flag = 1;
            }
            // If theres a space, get buffered number
            if ((c[i] == ' ' || c[i] == '\n' || i == rd) && num != -1) {
                arr[j++][k] = num;
                num = -1;
            }
            // If not space, add number to buffer
            else if (c[i] != ' ' && c[i] != '\n') {

                if (num == -1) {
                    num = 0;
                }
                if(((c[i] -48) >=0) && (c[i]-48 <=9))
                    num = (num*10) + (c[i] - 48);
            }
            i++;
        }
    }

}