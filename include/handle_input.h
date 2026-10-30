#ifndef HANDLE_INPUT_H
#define HANDLE_INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_INPUT_BUFFER 1024
#define MAX_ARGUMENT_BUFFER 64
#define DELIMITER " \t\r\n"


// Hae syöte
char *getInput(void);

// Pilko syöte
char **parseInput(char *input);

#endif