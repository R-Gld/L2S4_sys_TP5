#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 128

int main(int argc, char **argv) {
    int exit_code = EXIT_SUCCESS;

    FILE** streams = calloc(argc, sizeof(FILE*));
    streams[0] = stdout;
    for (int i = 1; i < argc; ++i) {
        streams[i] = fopen(argv[i], "w");
        if(streams[i] == NULL) {
            perror("fopen");
            fprintf(stderr, "Can't open %s file: %s\n", argv[i], strerror(errno));
        }
    }

    char* buf = calloc(BUFFER_SIZE, sizeof(char));

    while(!feof(stdin)) {
        size_t nb = fread(buf, sizeof(char), BUFFER_SIZE, stdin);
        if(ferror(stdin)) {
            perror("fread");
            exit_code = 1;
            break;
        }

        for (int i = 0; i < argc; ++i) {
            FILE* stream = streams[i];
            if(stream != NULL) {
                fwrite(buf, sizeof(char), nb, stream);
            }
        }
    }

    free(buf);
    for (int i = 1; i < argc; ++i) {
        fclose(streams[i]);
    }
    free(streams);
    return exit_code;
}