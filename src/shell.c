#include "shell.h"

char *builtinStr[] = {"cd", "help", "exit"};

int (*builtinFunc[]) (char**) = {
    &cmdCd,
    &cmdHelp,
    &cmdExit
};

int numBuiltins() {
    return sizeof(builtinStr) / sizeof(char*);
}

int launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();

    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("lapsiprosessi epäonnistui");
        }

        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Forkkaus epäonnistui");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!(WIFEXITED(status)) && !(WIFSIGNALED(status)));
    }

    return 1;
}

int execute(char **args) {
    if (args[0] == NULL) {
        return 1;
    }

    for (int i = 0; i < numBuiltins(); i++) {
        if (strcmp(args[0], builtinStr[i]) == 0) {
            return (*builtinFunc[i])(args);
        }
    }

    return launch(args);
}

int cmdCd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "odotettiin argumenttia komennolle \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("hakemiston vaihto epäonnistui");
        }
    }

    return 1;
}
int cmdHelp(char **args) {
    (void)args;

    printf("Sisäänrakennetut komennot:\n");
    for (int i = 0; i < numBuiltins(); i++) {
        printf("\t%s\n", builtinStr[i]);
    }

    return 1;
}

int cmdExit(char **args) {
    (void)args;
    return 0;
}