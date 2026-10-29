#include "handleInput.h"

char *getInput(void) {
    int bufferSize = MAX_INPUT_BUFFER;
    char *buffer = malloc(sizeof(char) * bufferSize);

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

        if (i >= bufferSize) {
            bufferSize *= 2;
            buffer = realloc(buffer, bufferSize);

            if (!buffer) {
                fprintf(stderr, "muistin uudelleenvaraus epäonnistui");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **parseInput(char *input) {
    int bufferSize = MAX_ARGUMENT_BUFFER;
    char **tokens = malloc(sizeof(char*) * bufferSize);

    if (!tokens) {
        fprintf(stderr, "muistin varaus epäonnistui");
        exit(EXIT_FAILURE);
    }

    char *token = strtok(input, DELIMITER);

    int i = 0;
    while (token != NULL) {
        tokens[i++] = token;

        if (i >= bufferSize) {
            bufferSize *= 2;
            tokens = realloc(tokens, sizeof(char*) * bufferSize);

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