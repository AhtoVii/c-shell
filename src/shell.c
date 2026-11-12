#include "shell.h"

static char *builtin_str[] = {
    "cd",       // Vaihda hakemistoa
    "help",     // Näytä sisäänrakennetut komennot
    "exit",     // Poistu ohjelmasta
    "pwd",      // Tulosta nykyinen hakemisto 
    "cls",      // Puhdista konsoli
    "echo",     // Tulosta viesti
    "history",  // Tulosta komentohistoria
};

// Sisäänrakennettujen komentojen kuvaukset
static char *builtin_str_description[] = {
    "vaihda hakemistoa",
    "tulosta sisäänrakennetut komennot",
    "poistu",
    "tulosta nykyinen hakemisto",
    "puhdista konsoli",
    "tulosta syöte",
    "tulosta komentohistoria"
};

// Funktiopointterit sisäänrakennettuihin komentoihin
static int (*builtin_func[]) (char**) = {
    &cd_cmd,
    &help_cmd,
    &exit_cmd,
    &pwd_cmd,
    &cls_cmd,
    &echo_cmd,
    &history_cmd,
};

// Palauta sisäänrakennettujen komentojen määrä
int num_builtins() {
    return sizeof(builtin_str) / sizeof(char*);
}

int launch(char **args) {    
    pid_t pid = fork(); // Luo lapsiprosessin

    if (pid == 0) {
        // Lapsiprosessi: suoritetaan ulkoinen ohjelma
        execvp(args[0], args);

        // Jos execvp epäonnistuu tulostetaan virhe ja poistutaan
        fprintf(stderr, "%s: %s\n", args[0], strerror(errno));

        _exit(errno == ENOENT ? 127 : 126); // 127 = ei löytynyt 126 = ei oikeuksia
    }

     if (pid < 0) {
        // virhe forkin aikana
        perror("forkkaus epäonnistui");
    } else {
        // Vanhempi prosessi: odotetaan lapsiprosessin päättymistä
        int status;
        while (waitpid(pid, &status, WUNTRACED) != -1 &&
               !WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1; // Palautetaan 1 jatkamista varten
}

int set_o_flags(char *cmd) {
    // O_WRONLY: vain kirjoitus
    // O_CREAT: luo, jos ei ole olemassa vielä
    // O_TRUNC: tyhjennä vanha sisältö
    // O_APPEND: lisää tiedoston loppuun

    if (!strcmp(cmd, OVERWRITE)) {
        return O_WRONLY | O_CREAT | O_TRUNC;
    } else if (!strcmp(cmd, APPEND)) {
        return O_WRONLY | O_CREAT | O_APPEND;
    } else if(!strcmp(cmd, REDIRECT_INPUT)) {
        return O_RDONLY;
    } else if (!strcmp(cmd, HERE_DOCUMENT)) {
        return 0;
    } else {
        return 0;
    }
}

char *get_redirection_operator(char *p, int *mode) {
    // Strcmp palauttaa 0, kun merkkijonot on samat
    if (!(strcmp(p, OVERWRITE))) {
        *mode = 1;
        return OVERWRITE;
    } else if (!(strcmp(p, APPEND))) {
        *mode = 1;
        return APPEND;
    } else if (!(strcmp(p, REDIRECT_INPUT))) {
        *mode = 0;
        return REDIRECT_INPUT;
    } else if (!(strcmp(p, HERE_DOCUMENT))) {
        *mode = 0;
        return HERE_DOCUMENT;
    }

    *mode = -1;
    return NULL;
}

static int body_to_fd(const char *body) {
    FILE *tmp = tmpfile();

    if (!tmp) {
        perror("tmpfile");
        return -1;
    }

    fputs(body ? body : "", tmp);
    fflush(tmp);
    rewind(tmp);

    int fd = dup(fileno(tmp));
    fclose(tmp);
    return fd;
}

// Tallennetaan komentohistoriaan
static int append_command_history(char **args) {
    
    FILE *fptr;
    fptr = fopen("history.txt", "a");

    if (!fptr) {
        fprintf(stderr, "tiedostonavaus epäonnistui\n");
        return 1;
    } 
    
    char **tmp = args;
    while (*tmp) {
        fputs(*tmp, fptr);

        // Lisätään välilyönti
        if (*(tmp + 1)) {
            fputs(" ", fptr);
        }
        tmp++;
    }

    fputs("\n", fptr);
    fclose(fptr);

    return 1;
}

int execute(char **args, char *heredoc_body) {
    char **p = args;
    char *outfile = NULL;
    char *cmd = NULL;
    int mode = -1;

    // Kasvatetaan osoitinta, kunnes jokin uudelleenohjausmerkki tulee vastaan tai taulukko päättyy
    while (*p) {
        cmd = get_redirection_operator(*p, &mode);

        if (cmd) {
            break;
        }

        p++;
    }

    char *redirect_token = NULL;

    if (*p) {
        // Jos löytyy uudelleenohjaus, tallennetaan tiedoston nimi,
        // joka on operaattorin oikealla puolella
        outfile = p[1];

        // Jos uudelleenohjaus tapahtuu, niin säilytetään koko taulukko, jotta ei tule muistivuotoa
        redirect_token = *p;

        // Katkaistaan taulukko tähän: komento loppuu ennen merkkiä
        *p = NULL;

        if (!outfile) { 
            // Jos tiedostonimi puuttuu tulostetaan virhe
            fprintf(stderr, "tiedostonimi puuttuu '%s' merkin jälkeen\n", cmd);
            *p = redirect_token;
            return 1;
        }
    }

    // Jos tyhjä syöte
    if (!args[0]) {
        if (redirect_token) {
            *p = redirect_token;
        }

        return 1;
    }

    int saved = -1;

    if (outfile) {
        int fd;

        if (!strcmp(cmd, HERE_DOCUMENT)) {
            fd = body_to_fd(heredoc_body);
        } else {
            // Yritetään avata tiedosto kirjoitukseen
            // 0644: kenellä oikeudet
            fd = open(outfile, set_o_flags(cmd), 0644);
        }

        if (fd < 0) {
            perror("tiedoston avaus epäonnistui");
            *p = redirect_token;
            return 1;
        }

        if (mode) {
            // Tyhjennetään stdout-puskuri
            fflush(stdout);

            // Tallennetaan alkuperäinen stdout
            saved = dup(STDOUT_FILENO);

            // Uudelleenohjataan stdout tiedostoon
            dup2(fd, STDOUT_FILENO);
        } else {

            saved = dup(STDIN_FILENO);

            dup2(fd, STDIN_FILENO);
        }

        close(fd);
    }

    // Selvitetään, onko komento sisäänrakennettu vai ulkoinen komento
    // Silmukka pysähtyy, kun nimi täsmää sisäänrakennetun komennon kanssa tai (i == num_builtints())
    int i = 0;
    while (i < num_builtins() && strcmp(args[0], builtin_str[i])) {
        i++;
    }

    if (mode) {
        fflush(stdout); // Tyhjennetään puskuri varuulta
    }

    // Komennot tallennetaan aina komentohistoriaan
    append_command_history(args);

    // Löytyikö komento? Jos löytyi kutsu funktio taulukosta esim (cd, pwd, echo), muuten aja ulkoisena ohjelmana esim (ls)
    int ret = (i < num_builtins()) ? builtin_func[i](args) : launch(args);

    if (outfile) {
        if (mode) {
            // Palautetaa stdout alkuperäiseksi
            fflush(stdout);
            dup2(saved, STDOUT_FILENO);
            close(saved);
        } else {
            dup2(saved, STDIN_FILENO);
            close(saved);
        }
    }

    if (redirect_token) {
        *p = redirect_token;
    }

    return ret; // 1 = jatka 0 = lopeta
}

int cd_cmd(char **args) {
    if (args[1] == NULL) {
        // cd ilman argumentteja vie kotihakemistoon
        if (chdir(getenv("HOME"))) {
            fprintf(stderr, "kotihakemistoon siirtyminen epäonnistui\n");
        }
        //fprintf(stderr, "odotettiin argumenttia komennolle \"cd\"\n");
    } else {
        if (chdir(args[1])) {
            fprintf(stderr, "%s: %s: hakemistoa ei löytynyt\n", args[0], args[1]);
        }
    }

    return 1;
}
int help_cmd(char **args) {
    (void)args;

    printf("Sisäänrakennetut komennot:\n");

    for (int i = 0; i < num_builtins(); i++) {
        printf("%-8s:%5s%s\n", builtin_str[i], " ", builtin_str_description[i]);
    }

    return 1;
}

int exit_cmd(char **args) {
    (void)args;
    return 0;
}

int pwd_cmd(char **args) {
    (void)args;

    char buffer[PATH_MAX];

    if (getcwd(buffer, sizeof(buffer)) != NULL) {
        printf("%s\n", buffer);
    } else {
        perror("getcwd() error");
        return 1;
    }

    return 1;
}

int cls_cmd(char **args) {
    (void)args;

    fflush(stdout);
    printf("\033[H\033[2J\033[3J"); // ANSI-escape koodi konsolin tyhjentämiseen
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

int history_cmd(char **args) {
    (void)args;

    FILE *fptr;
    fptr = fopen("history.txt", "r");

    if (!fptr) {
        fprintf(stderr, "tiedostonavaus epäonnistui\n");
        return 1;
    }

    char line[1024];
    size_t index = 1;

    while (fgets(line, sizeof(line), fptr)) {
        printf("%zu %s", index++, line);
    }

    fclose(fptr);
    return 1;
}