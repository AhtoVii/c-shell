#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <fcntl.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define OVERWRITE ">"
#define APPEND ">>"
#define REDIRECT_INPUT "<"
#define HERE_DOCUMENT "<<"

// Käynnistä prosessi
int launch(char **args);

// Aja prosessi
int execute(char **args);

// Change directory komento
int cd_cmd(char **args);

// Help komento
int help_cmd(char **args);

// Exit komento
int exit_cmd(char **args);

// Print working directory komento
int pwd_cmd(char **args);

// Clear screen komento
int cls_cmd(char **args);

// Echo komento
int echo_cmd(char **args);

// Laskee sisäänrakennettujen funktioiden määrän
int num_builtins();

#endif