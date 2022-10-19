#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main (int argc, char *argv[]) {
    // Check if user gave any args
    if (argc <= 1) {
        puts("Insufficient parameters passed");
        return 0;
    }

    int in_desc, out_desc, rd;
    char outFN[255];
    char inFN[255];
    char *c = malloc(100*sizeof(char)); 
    outFN[0] = '\0';
    inFN[0] = '\0';
    // Prepend file names with "./"
    strcat(inFN, "./");
    strcat(inFN, argv[1]);
    strcat(outFN, "./");
    strcat(outFN, argv[2]);

    // Open input and output files
    in_desc = open(inFN, O_RDONLY, 0777);
    if (in_desc == -1) {
        perror("Open input");
        exit(1);
    }

    out_desc = open(outFN, O_CREAT|O_RDWR, 0777);
    if (out_desc == -1) {
        perror("Open output");
        exit(1);
    }

    do {
        if ((rd = read(in_desc, c, 10)) != 0) {
            write(out_desc, c, strlen(c));
            // Overwrite c buffer so last write doesn't repeat chars
            for (int i = 0; i < 10; i++) {
                c[i] = '\0';
            }
        }
    } while (rd != 0);

    printf("The contents of file %s "
    "have successfully copied into the %s file\n", argv[1], argv[2]);

    close(in_desc);
    close(out_desc);
    return 0;
}