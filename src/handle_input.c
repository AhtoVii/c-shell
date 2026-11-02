#include "handle_input.h"

char *get_input(void) {
    int buffer_size = MAX_INPUT_BUFFER;
    char *buffer = malloc(sizeof(char) * buffer_size);

    if (!buffer) {
        fprintf(stderr, "muistin varaus epäonnistui");
        exit(EXIT_FAILURE);
    }

    int c;
    int i = 0;

    while (true) {
        c = getchar();

        if (c == EOF || c == '\n') {
            buffer[i] = '\0';
            return buffer;
        } else {
            buffer[i++] = c;
        }

        if (i >= buffer_size) {
            buffer_size *= 2;
            buffer = realloc(buffer, buffer_size);

            if (!buffer) {
                fprintf(stderr, "muistin uudelleenvaraus epäonnistui");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **parse_input(char *input) {
    int buffer_size = MAX_ARGUMENT_BUFFER;
    char **tokens = malloc(sizeof(char*) * buffer_size); // Taulukko sisältää tokenit (char*)

    // Tarkista, ettei muistinvaraus epäonnistu
    if (!tokens) {
        fprintf(stderr, "muistinvaraus epäonnistui");
        exit(EXIT_FAILURE);
    }

    char *ptr = input; // Käy koko syötteen läpi
    int count = 0; // Taulukkoa varten

    while (*ptr != '\0') {

        // Ohitetaan kaikki välilyönnit
        while (*ptr != '\0' && isspace((unsigned char)*ptr)) {
            ptr++;
        }

        // Jos äskeinen while-loop saavutti syötteen lopun
        if (*ptr == '\0') {
            break;
        }

        const char *start = ptr;
        char mode = ' ';
        bool word_has_started = 0;
        
        char buffer[1024];

        // Selvitetään tokenin pituus
        while (*ptr != '\0' && !isspace((unsigned char)*ptr)) {
            if (mode == ' ') {
                ptr++;
            } else if (mode == '\'') {

            } else if (mode == '\"') {

            }

            if (*ptr == '\'') {
                mode = '\'';

            } else if (*ptr == '\"') {
                mode = '\'';    
            }
        }

        // Lasketaan pituus
        size_t len = (size_t)(ptr - start);
        
        // Varataan tarvittaessa lisää muistia
        if (count + 1 >= buffer_size) {
            buffer_size *= 2;
            char **temp = realloc(tokens, sizeof(char*) * buffer_size);

            if (!temp) {
                fprintf(stderr, "muistin uudelleenvaraus epäonnistui");
                exit(EXIT_FAILURE);
            }

            tokens = temp;
        }

        // Luodaan tokenin muuttuja (+1 null-terminaattoria varten)
        char *token = malloc(len + 1);

        // Tarkistetaan, ettei muistinvaraus epäonnistunut
        if (!token) {
            fprintf(stderr, "muistinvaraus epäonnistui");
            exit(EXIT_FAILURE);
        }

        // Kopioidaan muuttujaan tokeni (start osoittaa ptr-pointteriin)
        memcpy(token, start, len);

        // Asetetaan null-terminaattori
        token[len] = '\0';

        // Lisätään taulukkoon tokeni ja kasvatetaan count-muuttujaa
        tokens[count++] = token;
    }

    // Viimeinen elementti on NULL
    tokens[count] = NULL;
    return tokens;
}

void free_tokens(char **tokens) {
    for (size_t i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }

    free(tokens);
}