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

// Jäsentämisen tulos
typedef enum {
    PARSE_OK,
    PARSE_INCOMPLETE,
    PARSE_FAILED,
} Parse_Result;

// Hae syöte
char *get_input(void);

// Pilko syöte
Parse_Result parse_input(char *input, char **tokens);

// Toteuta heittomerkkien sisällä tapahtuva logiikka
Parse_Result handle_single_quotes(char *token, char **ptr, size_t *len);

// Toteuta lainausmerkkien sisällä tapahtuva logiikka
Parse_Result handle_double_quotes(char *token, char **ptr, size_t *len);

// Lue heredoc monirivinen syöte, kunnes delimiter tulee vastaan
char *read_heredoc(const char *delim);

// Vapauta tokenit, mutta älä itse taulukkoa
void free_tokens(char **tokens);

#endif