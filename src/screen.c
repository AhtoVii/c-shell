#include "screen.h"

void run(void) {
    int status = 1;

    char *host_name = get_host_name();

    while (status) {
        print_prompt(false, host_name);

        char *line = get_input();
        if (!line) {
            putchar('\n');
            break; // CTRL-D lopettaa shellin
        }
        char **args = malloc(MAX_ARGUMENT_BUFFER * sizeof(char *));

        Parse_Result parsed_input_value = parse_input(line, args); 

        while (parsed_input_value == PARSE_INCOMPLETE) {
            print_prompt(true, NULL);

            get_missing_input(&line);

            free_tokens(args);

            args[0] = NULL;
            parsed_input_value = parse_input(line, args);
        }

        if (parsed_input_value == PARSE_OK) {
            char *heredoc_body = NULL;
            int mode;

            for (int i = 0; args[i] != NULL; i++) {
                char *op = get_redirection_operator(args[i], &mode);
                if (!op) {
                    continue;
                }

                if (!strcmp(op, HERE_DOCUMENT) && args[i + 1]) {
                    heredoc_body = read_heredoc(args[i + 1]);
                }

                break;
            }

            status = execute(args, heredoc_body);
            free(heredoc_body);
        }

        free(line);
        free_tokens(args);
        
        free(args);
    }

    free(host_name);
}

void get_missing_input(char **line) {
    char *missing_input = get_input();

    if (missing_input == NULL) {
        fprintf(stderr, "\nodottamaton tiedoston loppu\n");
        return;
    }

    size_t line_len = strlen(*line);
    size_t extra_len = strlen(missing_input);

    char *tmp = realloc(*line, line_len + 1 + extra_len + 1);

    if (tmp == NULL) {
        fprintf(stderr, "muistin uudelleenvaraus epäonnistui\n");
        free(missing_input);
        return;
    }

    *line = tmp;

    (*line)[line_len] = '\n';
    (*line)[line_len + 1] = '\0';
    strcat(*line, missing_input);
    free(missing_input);
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

void shorten_path(const char *cwd, char *out, size_t size) {
    const char *home = getenv("HOME");

    if (home && *home) {
        size_t len = strlen(home);

        if (strncmp(cwd, home, len) == 0 && (cwd[len] == '/' || cwd[len] == '\0')) {
            snprintf(out, size, "~%s", cwd + len);
            return;
        }
    }

    snprintf(out, size, "%s", cwd);
}

void print_prompt(bool wait_for_input, char *host_name) {
    if (wait_for_input) {
            printf("> ");
    } else {
        char *user = get_user();
        char cwd[PATH_MAX];
        char current_dir[PATH_MAX + 2];

        getcwd(cwd, sizeof cwd);
        shorten_path(cwd, current_dir, sizeof current_dir);
        printf(GREEN "%s@%s" RESET ":" CYAN "%s" RESET "$ ", user, host_name, current_dir);
    }

    fflush(stdout);
}