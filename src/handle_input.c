#include "handle_input.h"

char *get_input(void) {
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

int parse_input(char *input, char **tokens) {

    char *ptr = input; // Käy koko syötteen läpi
    int count = 0; // Taulukkoa varten

    while (*ptr) {
        while (isspace((unsigned char)*ptr)) {
            ptr++;
        }

        if (!*ptr) {
            break;
        }

        char *token = malloc(strlen(ptr) + 1);
        size_t len = 0;

        // Tarkistetaan, ettei muistinvaraus epäonnistunut
        if (!token) {
            fprintf(stderr, "muistinvaraus epäonnistui\n");
            exit(EXIT_FAILURE);
        }

        while (*ptr && !isspace((unsigned char)*ptr)) {

            if (*ptr == SINGLE_QUOTE) {
                int val = handle_single_quotes(token, &ptr, &len);
                if (val != 0) {
                    free(token);
                    tokens[count] = NULL;
                    return -1;
                }

            } else if (*ptr == DOUBLE_QUOTE) {
                int val = handle_double_quotes(token, &ptr, &len);
                if (val != 0) {
                    free(token);
                    tokens[count] = NULL;
                    return -1;
                }

            } else if (*ptr == ESCAPE_CHAR) {
                ptr++;
                token[len++] = *ptr++;
            } /* else if (*ptr == OVERWRITE_OUTPUT) {
                handle_output_overwrite(token, &ptr, &len);
                
            } else if (*ptr == OVERWRITE_APPEND) {
                handle_output_append(token, &ptr, &len);

            } else if (*ptr == INPUT_REDIRECTION) {
                handle_input_redirection(token, &ptr, &len);

            } else if (*ptr == HERE_DOCUMENT) {
                handle_here_document(token, &ptr, &len);

            } */ else {
                token[len++] = *ptr++;
            }
        }

        token[len] = '\0';
        tokens[count++] = token;
    }

    // Viimeinen elementti on NULL
    tokens[count] = NULL;
    return 0;
}

int handle_single_quotes(char *token, char **ptr, size_t *len) {
    (*ptr)++;

    while (**ptr && **ptr != '\'') {
        token[(*len)++] = **ptr;
        (*ptr)++;
    }

    if (!**ptr) {
        // FAIL
        /*fprintf(stderr, "heittomerkkejä ei suljettu\n");
        exit(EXIT_FAILURE);*/
        return -1;
    }

    (*ptr)++;
    return 0;
}

int handle_double_quotes(char *token, char **ptr, size_t *len) {
    (*ptr)++;

    while (**ptr && **ptr != '"') {
        if (**ptr == '\\' && (*ptr)[1] && strchr("\"\\", (*ptr)[1])) {
            (*ptr)++;
        }

        token[(*len)++] = **ptr;
        (*ptr)++;
    }

    if (!**ptr) {
        // FAIL
        /* fprintf(stderr, "lainausmerkkejä ei suljettu\n");
        exit(EXIT_FAILURE); */
        return -1;
    }

    (*ptr)++;
    return 0;
}

/* void handle_output_overwrite(char *token, char **ptr, size_t *len) {

}

void handle_output_append(char *token, char **ptr, size_t *len) {

}

void handle_input_redirection(char *token, char **ptr, size_t *len) {

}

void handle_here_document(char *token, char **ptr, size_t *len) {

} */

void free_tokens(char **tokens) {
    for (size_t i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
}