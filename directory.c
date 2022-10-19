#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#define MAXOPTION 3
#define MAXLINE 256

// chooseOption() will show the user all options
void getOption(char* option) {
    char line[MAXOPTION];

    // Show user main directory actions
    puts("Select the option(s) appropriately by entering the number:");
    puts("  Enter 1 for creating a directory");
    puts("  Enter 2 for removing directory");
    puts("  Enter 3 for printing working directory");
    puts("  Enter 4 for changing directory one level up");
    puts("  Enter 5 for reading the contents of directory");
    puts("  Enter 6 for closing the directory");
    puts("  Enter q to exit the program");
    
    fgets(line, MAXOPTION, stdin);
    sscanf(line, "%s *[^\n]", option);
}

void getName(char* name) {
    char line[MAXLINE];

    fgets(line, MAXLINE, stdin);
    sscanf(line, "%s *[^\n]", name);
}

// createDir() will create a directory with the given name
void createDir(char* name) {
    if (mkdir(name, 0777) == -1) {
        perror("Error");
    } else {
        puts("Directory is Created Successfully.");
    }
}

// removeDir() will delete a directory using the given name
void removeDir(char* name) {
    if (rmdir(name) == -1) {
        perror("Error");
    } else {
        puts("Directory is removed Successfully.");
    }
}

void printCurrentDir() {
    char cwd[MAXLINE];

    if (getcwd(cwd, MAXLINE) == NULL) {
        perror("\nError");
    }
    else {
        printf("%s\n", cwd);
    }
}

void readDir(DIR* dir) {
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }
}

/**
 * Running this file will run a short program where the user can give
 * inputs that correspond to the given actions in command line
**/
int main (void) {
    int truth = 1;
    int numChosen;
    char* option = (char *)malloc(sizeof(char) * MAXOPTION);
    DIR *dir;

    do {    
        getOption(option);

        // User entered a number
        if (isdigit(*option)) {
            char* name = (char *)malloc(sizeof(char) * MAXLINE);
            numChosen = *option - '0';

            switch(numChosen) {
                case 1:
                    // Create new dir
                    puts("Enter the Directory name you want to create:");
                    getName(name);
                    createDir(name);
                    break;
                case 2:
                    // Remove dir
                    puts("Enter the Directory name you want to remove:");
                    getName(name);
                    removeDir(name);
                    break;
                case 3:
                    // Print current dir
                    fputs("Current Working Directory is: ", stdout);
                    printCurrentDir();
                    break;
                case 4:
                    // Move up one level
                    fputs("Working Directory Before Operation: ", stdout);
                    printCurrentDir();
                    if (chdir("../") == -1) {
                        perror("Error");
                    } else {
                        puts("Directory Changed Successfully");
                        fputs("Working Directory After Operation: ", stdout);
                        printCurrentDir();
                    }
                    break;
                case 5:
                    // read contents of dir
                    // Open directory
                    if ((dir = opendir(".")) == NULL) {
                        perror("Error");
                    }
                    readDir(dir);
                    break;
                case 6:
                    // close dir
                    if (closedir(dir) == -1) {
                        perror("Error");
                    } else {
                        puts("Directory Closed Successfully");
                    }
                    break;
                default:
                    continue;
            }
            free(name);
            name = NULL;
        } 
        // Option selected was not a number, check if it was q
        else {
            if (*option == 'q') {
                break;
            }
        }
    } while(truth);

    free(option);
    option = NULL;
    return 0;
}
