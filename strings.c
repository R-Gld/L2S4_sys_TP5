#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MEMSIZE 128

int main(int argc, char **argv) {
    if (argc < 2 || argc == 3 || argc > 4) {
        fprintf(stderr, "Usage: %s [-n N] FILE\n", argv[0]);
        return EXIT_FAILURE;
    }

    long n_long = 4; // Default value for n
    char *fileName = argv[1];
    char *endptr; // For strtol error checking

    // Adjust parameters if -n option is used
    if (argc == 4) {
        if (strcmp(argv[1], "-n") != 0) {
            fprintf(stderr, "Invalid option. Usage: %s [-n N] FILE\n", argv[0]);
            return EXIT_FAILURE;
        }
        n_long = strtol(argv[2], &endptr, 10);
        // Check for non-numeric characters or no conversion performed
        if (*endptr != '\0' || endptr == argv[2]) {
            fprintf(stderr, "N must be a numeric value.\n");
            return EXIT_FAILURE;
        }
        fileName = argv[3];
    }

    int n = (int) n_long;

    FILE *stream = fopen(fileName, "r");
    if (stream == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    char *memory = (char *)calloc(MEMSIZE, sizeof(char));
    if (memory == NULL) {
        perror("Failed to allocate memory");
        fclose(stream);
        return EXIT_FAILURE;
    }

    int length = 0;
    char vl;

    while (fread(&vl, sizeof(char), 1, stream) == 1) {
        if (isprint(vl) || vl == '\t') {
            if (length < MEMSIZE - 1) {
                memory[length++] = vl;
            }
        } else {
            if (length >= n) {
                memory[length] = '\0'; // Null-terminate before printing
                printf("%s\n", memory);
            }
            length = 0;
        }
    }

    if (ferror(stream)) {
        perror("Error reading from file");
        fclose(stream);
        free(memory);
        return EXIT_FAILURE;
    }

    // Flush remaining content if it meets the length requirement
    if (length >= n) {
        memory[length] = '\0';
        printf("%s\n", memory);
    }

    fclose(stream);
    free(memory);

    return EXIT_SUCCESS;
}
