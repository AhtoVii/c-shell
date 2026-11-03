#ifndef HANDLE_INPUT_H
#define HANDLE_INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT_BUFFER 1024
#define MAX_ARGUMENT_BUFFER 64

#define SINGLE_QUOTE ('\'')
#define DOUBLE_QUOTE ('"')
#define ESCAPE_CHAR ('\\')

#define OVERWRITE_OUTPUT ('>')
#define OVERWRITE_APPEND ('>>')
#define INPUT_REDIRECTION ('<')
#define HERE_DOCUMENT ('<<')

// Hae syöte
char *get_input(void);

// Pilko syöte
int parse_input(char *input, char **tokens);

int handle_single_quotes(char *token, char **ptr, size_t *len);
int handle_double_quotes(char *token, char **ptr, size_t *len);
void handle_output_overwrite(char *token, char **ptr, size_t *len);
void handle_output_append(char *token, char **ptr, size_t *len);
void handle_input_redirection(char *token, char **ptr, size_t *len);
void handle_here_document(char *token, char **ptr, size_t *len);

// Vapauta tokenit
void free_tokens(char **tokens);

#endif