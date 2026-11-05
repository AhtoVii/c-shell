#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <pwd.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "handle_input.h"
#include "shell.h"

#define RESET "\033[0m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"

#define MAX_ARGUMENT_BUFFER 64

char *get_host_name(void);
char *get_user(void);
void shorten_path(const char *cwd, char *out, size_t size);
void print_prompt(bool wait_for_input, char *host_name);
void get_missing_input(char **line);
void run(void);