#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

#define READ_END 0
#define WRITE_END 1
#define BUFFER_SIZE 1024

void read_sample(int fd[2], char* inFN);
void split_line(char* arr, int fd[2], char* filename);
//void child_exec(char)
void writeOuput(char* command, char* output);

int main(int argc, char *argv[]) {
    // Confirm user gave args
    if (argc <= 1) {
        puts("Insufficient parameters passed");
        return 1;
    }
    
    int fd[2], pipe_length;
    char inFN[255];
    char file_contents[BUFFER_SIZE];
    char tok[2] = "\n";
    char* token;
    inFN[0] = '\0';
    pid_t pid;

    // Prepend file name from argv
    strcat(inFN, "./");
    strcat(inFN, argv[1]);

    // Create the pipe
    if (pipe(fd) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    // Create child process to read file
    if ((pid = fork()) < 0) {
        // Error occurred
        fprintf(stderr,"Fork Failed");
        return 1;
    }

    if (pid == 0) {
        // Child process
        read_sample(fd, inFN);
        exit(1);
    }

    close(fd[WRITE_END]);
    // Check if anything returned
    if ((pipe_length = read(fd[READ_END], file_contents, BUFFER_SIZE)) <= 0)
        // Nothing returned from pipe, exit program
        return 1;
    file_contents[pipe_length] = 0; // Terminate the string
    int arr_len = 0;

    // Create array of size 1
    char** arr = (char**)malloc(sizeof(char*));

    // Split up file contents into by line
    token = strtok(file_contents, tok);
    while (token != NULL) {
        // Create enough room at each index for the line
        arr[arr_len] = (char*) malloc (sizeof(char) * strlen(token));
        strcpy(arr[arr_len++], token);
        token = strtok(NULL, tok);
        if (token != NULL)
            arr = realloc(arr, sizeof(char*) * arr_len + 1);
    }

    // Now execute all the functions
    char* filename = malloc(sizeof(char));

    for (int i = 0; i < arr_len; i++) {
        // Create the pipe
        if (pipe(fd) == -1) {
            fprintf(stderr, "Pipe Failed");
            return 1;
        }
        split_line(arr[i], fd, filename);
    }
    
    int children_left = arr_len;
    while (children_left > 0) {
        wait(NULL);
        --children_left;
    }

    for (int i = 0; i < arr_len; i++) {
        free(arr[i]);
        arr[i] = NULL;
    }
    free(arr);
    arr = NULL;
    return 0;
}

void read_sample(int fd[2], char* inFN) {
    close(fd[READ_END]);

    char* c = malloc(BUFFER_SIZE * sizeof(char));
    char file_contents[BUFFER_SIZE];
    int in_desc, rd;

    in_desc = open(inFN, O_RDONLY, 0777);
    if (in_desc == -1) {
        fprintf(stderr, "File Open Failed");
        return;
    }

    do {
        if ((rd = read(in_desc, c, BUFFER_SIZE)) != 0)
            strncpy(file_contents, c, BUFFER_SIZE);
    } while (rd != 0);

    free(c);
    close(in_desc);

    // Write to parent
    write(fd[WRITE_END], file_contents, strlen(file_contents) + 1);

}

void split_line(char* arr, int fd[2], char* filename) {
    pid_t pid;
    char* token;
    int flag = 0, file_desc, pipe_length;
    char line_arr[10][200];
    char line[50];
    char cmd_output[BUFFER_SIZE];;
    int line_len = 0;

    strcpy(line, arr);
    token = strtok(line, " ");
    // Split line up by space
    while (token != NULL) {
        if (flag) {
            filename = realloc(filename, sizeof(char) * strlen(token));
            strcpy(filename, token);
        }
        else if (*token == '>')
            // if redirect command found, set flag and skip to next param
            flag = 1;
        else
            strcpy(line_arr[line_len++], token);
        token = strtok(NULL, " ");
    }


    char *ptr_array[line_len + 1];
    for ( int i = 0; i < line_len; i++ )
        ptr_array[i] = line_arr[i];
    ptr_array[line_len] = NULL;

    if((pid = fork()) < 0) {
        // Error occurred
        fprintf(stderr,"Fork Failed");
        return;
    }
    if (pid == 0) {
        if (flag) {
            // Redirect output
            file_desc = open(filename, O_APPEND | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
            if (file_desc == -1) {
                fprintf(stderr, "File Open Failed");
                exit(1);
            }
            dup2(file_desc, 1);
            close(file_desc);
        } else {
            // Don't redirect output
            close(STDOUT_FILENO);
            dup(fd[WRITE_END]);
            close(fd[READ_END]);
        }
        execvp(ptr_array[0], ptr_array);
        exit(1);
    }

    // Parent prints to output.txt
    close(fd[WRITE_END]);
    if (!flag) {
        close(STDIN_FILENO);
        dup(fd[READ_END]);
        if ((pipe_length = read(fd[READ_END], cmd_output, BUFFER_SIZE)) >= 0) {
            cmd_output[pipe_length] = 0; // Terminate the string
            writeOuput(arr, cmd_output);
        }
    }
}

void writeOuput(char* command, char* output)
{
    FILE *fp;
    fp = fopen("output.txt", "a");
	fprintf(fp, "The output of: %s : is\n", command);
	fprintf(fp, ">>>>>>>>>>>>>>>\n%s<<<<<<<<<<<<<<<\n", output);
    fclose(fp);
}