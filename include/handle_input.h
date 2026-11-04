#ifndef HANDLE_INPUT_H
#define HANDLE_INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT_BUFFER 1024
#define MAX_ARGUMENT_BUFFER 64

#define SINGLE_QUOTE '\''
#define DOUBLE_QUOTE '"'
#define ESCAPE_CHAR '\\'

#define OVERWRITE_OUTPUT '>'
#define OVERWRITE_APPEND 'x'
#define INPUT_REDIRECTION '<'
#define HERE_DOCUMENT 'x'

typedef enum Parse_Result{
    PARSE_OK,
    PARSE_INCOMPLETE,
    PARSE_FAILED,
} Parse_Result;

// Hae syöte
char *get_input(void);

// Pilko syöte
Parse_Result parse_input(char *input, char **tokens);

Parse_Result handle_single_quotes(char *token, char **ptr, size_t *len);
Parse_Result handle_double_quotes(char *token, char **ptr, size_t *len);

// Vapauta tokenit
void free_tokens(char **tokens);

#endif