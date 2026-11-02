#ifndef HANDLE_INPUT_H
#define HANDLE_INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT_BUFFER 1024
#define MAX_ARGUMENT_BUFFER 64
#define DELIMITER " \t\r\n"


// Hae syöte
char *get_input(void);

// Pilko syöte
char **parse_input(char *input);

// Vapauta tokenit
void free_tokens(char **tokens);

#endif