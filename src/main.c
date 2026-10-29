#include <stdio.h>
#include <stdbool.h>

#include "handleInput.h"
#include "shell.h"

int main(void) {

    int status = 1;

    while (status) {
        printf("$ ");
        char *line = getInput();
        char **args = parseInput(line);
        status = execute(args);

        free(line);
        free(args);
    }

    return EXIT_SUCCESS;
}