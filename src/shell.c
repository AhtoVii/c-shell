#include "shell.h"

char *builtin_str[] =
 {
    "cd",
    "help",
    "exit",
    "pwd",
    "cls",
    "echo"
};

int (*builtin_func[]) (char**) = {
    &cd_cmd,
    &help_cmd,
    &exit_cmd,
    &pwd_cmd,
    &cls_cmd,
    &echo_cmd
};

int num_builtins() {
    return sizeof(builtin_str) / sizeof(char*);
}

int launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();

    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            fprintf(stderr, "%s: komentoa ei löytynyt\n", args[0]);
        } else if (errno == ENOENT) {
            fprintf(stderr, "%s: ei suoritusoikeutta\n", args[0]);
        } else {
            fprintf(stderr, "%s: %s\n", args[0], strerror(errno));
        }

        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Forkkaus epäonnistui");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
            if (!wpid) {
                perror("waitpid");
                break;
            }
            
        } while (!(WIFEXITED(status)) && !(WIFSIGNALED(status)));
    }

    return 1;
}

int execute(char **args) {
    if (args[0] == NULL) {
        return 1;
    }

    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return launch(args);
}

int cd_cmd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "odotettiin argumenttia komennolle \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            fprintf(stderr, "%s: %s: hakemistoa ei löytynyt\n", args[0], args[1]);
        }
    }

    return 1;
}
int help_cmd(char **args) {
    (void)args;

    printf("Sisäänrakennetut komennot:\n");

    for (int i = 0; i < num_builtins(); i++) {
        printf("*    [%s]\n", builtin_str[i]);
    }

    return 1;
}

int exit_cmd(char **args) {
    (void)args;
    return 0;
}

int pwd_cmd(char **args) {
    (void)args;

    size_t buffer_size = 1024;
    char buffer[buffer_size];

    if (getcwd(buffer, buffer_size) != NULL) {
        printf("%s\n", buffer);
    } else {
        perror("getcwd() error");
        return 0;
    }

    return 1;
}

int cls_cmd(char **args) {
    (void)args;

    fflush(stdout);
    printf("\033[H\033[2J\033[3J");
    fflush(stdout);
    return 1;
}

int echo_cmd(char **args) {
    for (size_t i = 1; args[i] != NULL; i++) {
        // Erotetaan välilyönneillä, mutta ei lisätä välilyöntiä loppuun
        printf("%s%s", args[i], args[i + 1] ? " " : "");
    }

    printf("\n");

    return 1;
}