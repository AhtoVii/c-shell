#include "shell.h"

char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "pwd",
    "cls",
    "echo"
};

char *builtin_str_description[] = {
    "vaihda hakemistoa",
    "tulosta sisäänrakennetut komennot",
    "poistu",
    "tulosta nykyinen hakemisto",
    "puhdista konsoli",
    "tulosta"
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
    // fork() kloonaa koko prosessin, sen jälkeen on kaksi prosessia, jotka ovat samat, mutta eroavat palautusarvossa
    // lapsessa: 0
    // vanhemmassa: lapsen PID (>0)
    // virhe: -1
    
    pid_t pid = fork();

    if (pid == 0) {
        // Lapsiprosessi
        // execvp korvaa tämän prosessin kokonaan uudella ohjelmalla
        // onnistuessaan ei koskaan palaa ja jatketaan vain virheessä.
        execvp(args[0], args);

        // Päästiin tänne, joten virhe
        fprintf(stderr, "%s: %s\n", args[0], strerror(errno));

        // _exit lopettaa lapsiprosessin heti.
        // 127 = ei löytynyt, 126 = ei oikeuksia
        _exit(errno == ENOENT ? 127 : 126);
    }

     if (pid < 0) {
        perror("forkkaus epäonnistui");
    } else {
        // VANHEMPI: odotetaan, että lapsi valmistuu.
        int status;
        // waitpid palauttaa -1 virheessä. WUNTRACED herättää myös, jos lapsi
        // pysäytetään (Ctrl+Z). Silloin WIFEXITED ja WIFSIGNALED ovat
        // molemmat epätosia, ja silmukka odottaa uudelleen.
        while (waitpid(pid, &status, WUNTRACED) != -1 &&
               !WIFEXITED(status) && !WIFSIGNALED(status));
        //      ^ ei virhettä   ^ ei vielä poistunut  ^ ei tapettu signaalilla
    }

    return 1;
}

int execute(char **args) {
    // Etsitään '>' ja katkaistaan komento sen kohdalta: echo "hello" > file.txt

    char **p = args;
    char *outfile = NULL;

    // Kasvatetaan osoitinta, kunnes '>' merkki tulee vastaan tai taulukko päättyy
    // Strcmp palauttaa 0, kun merkkijonot on samat, joten silmukka päättyy
    while (*p && strcmp(*p, ">")) {
        p++;
    }

    if (*p) {
        // Seuraava tokeni on tiedosto
        outfile = p[1];

        // Katkaistaan taulukko tähän: komento loppuu ennen '>' merkkiä
        *p = NULL;

        // Esim. echo "hello" >
        if (!outfile) { 
            fprintf(stderr, "tiedostonimi puuttuu '>' merkin jälkeen\n");
            return 1;
        }
    }

    // Jos tyhjä syöte
    if (!args[0]) {
        return 1;
    }


    int saved = -1;

    if (outfile) {
        // Yritetään avata tiedosto kirjoitukseen
        // O_WRONLY: vain kirjoitus
        // O_CREAT: luo, jos ei ole olemassa vielä
        // O_TRUNC: tyhjennä vanha sisältö
        // 0644: kenellä oikeudet
        int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror("tiedoston avaus epäonnistui");
            return 1;
        }

        // Tyhjennetään vanha puskuri
        fflush(stdout);

        saved = dup(STDOUT_FILENO);
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    // Selvitetään, onko komento sisäänrakennettu vai ulkoinen komento
    // Silmukka pysähtyy, kun nimi täsmää sisäänrakennetun komennon kanssa tai (i == num_builtints())
    int i = 0;
    while (i < num_builtins() && strcmp(args[0], builtin_str[i])) {
        i++;
    }

    fflush(stdout); // Tyhjennetään puskuri varuulta

    // Löytyikö komento? Jos löytyi kutsu funktio taulukosta esim (cd, pwd, echo), muuten aja ulkoisena ohjelmana esim (ls)
    int ret = (i < num_builtins()) ? builtin_func[i](args) : launch(args);

    if (outfile) {
        // Kirjoitetaan puskurin sisältö tiedostoon
        fflush(stdout);
        dup2(saved, STDOUT_FILENO);
        close(saved);
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

    char buffer[4096];

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