#include "shell.h"

char *builtin_str[] = {
    "cd",   // Vaihda hakemistoa
    "help", // Näytä sisäänrakennetut komennot
    "exit", // Poistu ohjelmasta
    "pwd",  // Tulosta nykyinen hakemisto 
    "cls",  // Puhdista konsoli
    "echo"  // Tulosta viesti
};

// Sisäänrakennettujen komentojen kuvaukset
char *builtin_str_description[] = {
    "vaihda hakemistoa",
    "tulosta sisäänrakennetut komennot",
    "poistu",
    "tulosta nykyinen hakemisto",
    "puhdista konsoli",
    "tulosta"
};

// Funktiopointterit sisäänrakennettuihin komentoihin
int (*builtin_func[]) (char**) = {
    &cd_cmd,
    &help_cmd,
    &exit_cmd,
    &pwd_cmd,
    &cls_cmd,
    &echo_cmd
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

int execute(char **args) {
    // Tarkistetaan onko komennossa uudelleenohjausoperaattori ('>' tai '>>')

    char **p = args;
    char *outfile = NULL;
    char *cmd = NULL;

    // Kasvatetaan osoitinta, kunnes jokin uudelleenohjausmerkki tulee vastaan tai taulukko päättyy
    // Strcmp palauttaa 0, kun merkkijonot on samat, joten silmukka päättyy
    while (*p) {
        if (!(strcmp(*p, OVERWRITE))) {
            cmd = OVERWRITE;
            break;
        } else if (!(strcmp(*p, APPEND))) {
            cmd = APPEND;
            break;
        } else if (!(strcmp(*p, REDIRECT_INPUT))) {
            cmd = REDIRECT_INPUT;
            break;
        } else if (!(strcmp(*p, HERE_DOCUMENT))) {
            cmd = HERE_DOCUMENT;
            break;
        } else {

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
    int input_or_output = 0;

    if (outfile) {
        // Yritetään avata tiedosto kirjoitukseen
        // O_WRONLY: vain kirjoitus
        // O_CREAT: luo, jos ei ole olemassa vielä
        // O_TRUNC: tyhjennä vanha sisältö
        // O_APPEND: lisää tiedoston loppuun
        // 0644: kenellä oikeudet

        // Ylikirjoitetaanko vai lisätäänkö tiedostoon?
        int o_flags = strcmp(cmd, OVERWRITE) == 0 ? O_TRUNC : O_APPEND;

        if (!strcmp(cmd, OVERWRITE)) {
            o_flags = O_WRONLY | O_CREAT | O_TRUNC;
            input_or_output = 1;
        } else if (!strcmp(cmd, APPEND)) {
            o_flags = O_WRONLY | O_CREAT | O_APPEND;
            input_or_output = 1;
        } else if(!strcmp(cmd, REDIRECT_INPUT)) {
            o_flags = O_RDONLY;
        } else if (!strcmp(cmd, HERE_DOCUMENT)) {
            o_flags = 0;
        }

        int fd = open(outfile, o_flags, 0644);

        if (fd < 0) {
            perror("tiedoston avaus epäonnistui");
            *p = redirect_token;
            return 1;
        }

        if (input_or_output) {
            // Tyhjennetään stdout-puskuri
            fflush(stdout);

            // Tallennetaan alkuperäinen stdout
            saved = dup(STDOUT_FILENO);

            // Uudelleenohjataan stdout tiedostoon
            dup2(fd, STDOUT_FILENO);
        } else {

            fflush(stdin);

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

    if (input_or_output) {
        fflush(stdout); // Tyhjennetään puskuri varuulta
    } else {
        fflush(stdin);
    }

    // Löytyikö komento? Jos löytyi kutsu funktio taulukosta esim (cd, pwd, echo), muuten aja ulkoisena ohjelmana esim (ls)
    int ret = (i < num_builtins()) ? builtin_func[i](args) : launch(args);

    if (outfile) {
        if (input_or_output) {
            // Palautetaa stdout alkuperäiseksi
            fflush(stdout);
            dup2(saved, STDOUT_FILENO);
            close(saved);
        } else {
            fflush(stdin);
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
        printf("[%s]:\t[%s]\n", builtin_str[i], builtin_str_description[i]);
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