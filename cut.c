#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 256

void printUsage(char* exec);

int main(int argc, char **argv) {

    if(argc < 3 || argc > 4) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    char delim = argv[1][0];
    if(strlen(argv[1]) > 1) {
        fprintf(stderr, "The DELIM should be a 1 octet char.\n");
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    int field_number = atoi(argv[2]);
    if(field_number < 1) {
        fprintf(stderr, "The FIELD_NUMBER should be an int greater than 1.\n");
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    FILE* input = (argc == 4) ? fopen(argv[3], "r") : stdin;
    if(input == NULL) {
        fprintf(stderr, "Error, cannot open file %s.\n", argv[3]);
        return EXIT_FAILURE;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while((read = getline(&line, &len, input)) != -1) {
        int current_field = 1;
        char *tk = strtok(line, &delim);
        while(tk != NULL) {
            if(current_field == field_number) {
                printf("%s\n", tk);
                break;
            }
            tk = strtok(NULL, &delim);
            current_field++;
        }
    }
    free(line);

    if(input != stdin) {
        fclose(input);
    }

    return EXIT_SUCCESS;
}

void printUsage(char* exec) {
    fprintf(stderr, "Usage: %s DELIM FIELD_NUMBER [FILE]\n", exec);
}
