#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MAX_LINE 1024
#define MAX_ARGS 100

// Parsi komento
void parseCommand(char *line, char **args) {
    // Erota tokenit toisistaan välilyönnin, tabin ja uuden rivin perusteella
    char *token = strtok(line, " \t\n");

    size_t i = 0;

    // Käy kaikki tokenit läpi
    while (token != NULL) {

        // Lisätään tokeni argumenttilistaan
        args[i++] = token;

        token = strtok(NULL, " \n");
    }

    // Viimeinen argumentti NULL execvp-funktion vuoksi
    args[i] = NULL;
}

void executeCommand(char **args) {
    pid_t pid, wpid;
    int status;

    if ((pid = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("exec");
        }
        exit(EXIT_FAILURE);
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

int main(void) {
    char line[MAX_LINE];
    char *args[MAX_ARGS];

    while (true) {
        printf("$ ");

        if (!fgets(line, sizeof(line), stdin)) {
            break;
        }

        parseCommand(line, args);

        if (args[0] == NULL) {
            continue;
        }

        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                fprintf(stderr, "cd: argumentti puuttuu\n");
            } else if (chdir(args[1]) != 0) {
                perror("cd");
            }

            continue;
        }

        executeCommand(args);
    }

    return 0;
}