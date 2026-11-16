#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <pwd.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "handle_input.h"
#include "shell.h"

#define RESET "\033[0m"
#define GREEN "\033[32m"
#define RED "\033[31m"

#define MAX_ARGUMENT_BUFFER 64

// Palauta tietokoneen käyttäjän nimi
char *get_host_name(void);

// Hae käyttäjä
char *get_user(void);

// Lyhennä kotihakemisto
void shorten_path(const char *cwd, char *out, size_t size);

// Tulosta syöte, joka näkyy joka rivillä
void print_prompt(bool wait_for_input, char *host_name);

// Haetaan loput syötteestä, jos heitto tai lainausmerkkiä ei suljettu
void get_missing_input(char **line);

// Aja shell
void run(void);