#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int launch(char **args);
int execute(char **args);

int cd_cmd(char **args);
int help_cmd(char **args);
int exit_cmd(char **args);
int pwd_cmd(char **args);
int cls_cmd(char **args);
int echo_cmd(char **args);

int num_builtins();

#endif