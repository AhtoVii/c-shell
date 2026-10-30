#include "handle_input.h"

char *getInput(void) {
    int buffer_size = MAX_INPUT_BUFFER;
    char *buffer = malloc(sizeof(char) * buffer_size);

    if (!buffer) {
        fprintf(stderr, "muistin varaus epäonnistui");
        exit(EXIT_FAILURE);
    }

    int c;
    int i = 0;

    while (true) {
        c = getchar();

        if (c == EOF || c == '\n') {
            buffer[i] = '\0';
            return buffer;
        } else {
            buffer[i++] = c;
        }

        if (i >= buffer_size) {
            buffer_size *= 2;
            buffer = realloc(buffer, buffer_size);

            if (!buffer) {
                fprintf(stderr, "muistin uudelleenvaraus epäonnistui");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **parseInput(char *input) {
    int buffer_size = MAX_ARGUMENT_BUFFER;
    char **tokens = malloc(sizeof(char*) * buffer_size);

    if (!tokens) {
        fprintf(stderr, "muistin varaus epäonnistui");
        exit(EXIT_FAILURE);
    }

    char *token = strtok(input, DELIMITER);

    int i = 0;
    while (token != NULL) {
        tokens[i++] = token;

        if (i >= buffer_size) {
            buffer_size *= 2;
            tokens = realloc(tokens, sizeof(char*) * buffer_size);

            if (!tokens) {
                fprintf(stderr, "muistin uudelleenvaraus epäonnistui");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, DELIMITER);
    }

    tokens[i] = NULL;

    return tokens;
}