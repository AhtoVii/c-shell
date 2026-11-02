#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <pwd.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

#include "handle_input.h"
#include "shell.h"

char *get_current_directory(void);
char *get_host_name(void);
char *get_user(void);

int main(void) {

    int status = 1;
    char *user = get_user();
    char *host_name = get_host_name();
    

    while (status) {
        char *current_dir = get_current_directory();

        printf("%s@%s:%s$ ", user, host_name, current_dir);
        fflush(stdout);

        char *line = get_input();
        char **args = parse_input(line);
        status = execute(args);

        free(current_dir);
        free(line);
        free_tokens(args);

    }

    free(user);
    free(host_name);
    return EXIT_SUCCESS;
}

char *get_current_directory(void) {
    char *buffer = malloc(1024);
    getcwd(buffer, 1024);
    return buffer;
}

char *get_host_name(void) {
    char *host_name = malloc(HOST_NAME_MAX + 1);
    
    if (host_name == NULL) {
        return NULL;
    }
    
    if (gethostname(host_name, HOST_NAME_MAX + 1) == -1) {
        free(host_name);
        return NULL;
    }

    host_name[HOST_NAME_MAX] = '\0';
    return host_name;
}

char *get_user(void) {
    struct passwd *pw = getpwuid(getuid());
    char *user = pw ? pw->pw_name : "user";
    return user;
}