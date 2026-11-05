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

    while ((c = getchar()) != EOF && c != '\n') {
        buffer[i++] = c;

        if (i >= buffer_size) {
            buffer_size *= 2;
            char *tmp = realloc(buffer, buffer_size);

            if (!tmp) {
                fprintf(stderr, "muistin uudelleenvaraus epäonnistui");
                exit(EXIT_FAILURE);
            }

            buffer = tmp;
        }
    }

    buffer[i] = '\0';
    return buffer;
}

Parse_Result parse_input(char *input, char **tokens) {

    char *ptr = input; // Käy koko syötteen läpi
    int count = 0; // Taulukkoa varten

    while (*ptr) {

        // Ohitetaan välilyönnit
        while (isspace((unsigned char)*ptr)) {
            ptr++;
        }

        // Jos saavutettiin syötteen loppu
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
                Parse_Result val = handle_single_quotes(token, &ptr, &len);
                if (val == PARSE_INCOMPLETE) {
                    free(token);
                    tokens[count] = NULL;
                    return PARSE_INCOMPLETE;
                }

            } else if (*ptr == DOUBLE_QUOTE) {
                Parse_Result val = handle_double_quotes(token, &ptr, &len);
                if (val == PARSE_INCOMPLETE) {
                    free(token);
                    tokens[count] = NULL;
                    return PARSE_INCOMPLETE;
                }

            }  else {
                token[len++] = *ptr++;
            }
        }

        token[len] = '\0';
        tokens[count++] = token;
    }

    // Viimeinen elementti on NULL
    tokens[count] = NULL;
    return PARSE_OK;
}

Parse_Result handle_single_quotes(char *token, char **ptr, size_t *len) {
    (*ptr)++;

    while (**ptr && **ptr != '\'') {
        token[(*len)++] = **ptr;
        (*ptr)++;
    }

    if (!**ptr) {
        return PARSE_INCOMPLETE;
    }

    (*ptr)++;
    return PARSE_OK;
}

Parse_Result handle_double_quotes(char *token, char **ptr, size_t *len) {
    (*ptr)++;

    while (**ptr && **ptr != '"') {
        if (**ptr == '\\' && (*ptr)[1] && strchr("\"\\", (*ptr)[1])) {
            (*ptr)++;
        }

        token[(*len)++] = **ptr;
        (*ptr)++;
    }

    if (!**ptr) {
        return PARSE_INCOMPLETE;
    }

    (*ptr)++;
    return PARSE_OK;
}

void free_tokens(char **tokens) {
    for (size_t i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
}