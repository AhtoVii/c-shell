#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdbool.h>

#include "handle_input.h"
#include "shell.h"

#define HOST_NAME_MAX 256

int main(void) {

    int status = 1;
    char host_name[HOST_NAME_MAX + 1];
    gethostname(host_name, sizeof(host_name));

    while (status) {
        printf("%s@%s:%s$ ", getenv("USER"), host_name, getenv("PWD"));
        char *line = getInput();
        char **args = parseInput(line);
        status = execute(args);

        free(line);
        free(args);
    }

    return EXIT_SUCCESS;
}