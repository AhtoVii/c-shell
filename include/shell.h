#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int launch(char **args);
int execute(char **args);

int cmdCd(char **args);
int cmdHelp(char **args);
int cmdExit(char **args);

int numBuiltins();

#endif