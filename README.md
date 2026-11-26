C-Shell

C-kielellä toteutettu komentotulkki (shell), joka mahdollistaa komentojen suorittamisen Unix-ympäristössä. Projektin tavoitteena on toteuttaa komentotulkin keskeinen toimintalogiikka ilman valmiiden komentotulkkien hyödyntämistä.

Ominaisuudet

Interaktiivinen komentorivi: Käyttäjä voi syöttää komentoja komentotulkkiin.

Syötteen käsittely: Komentorivin lukeminen ja komentojen argumenttien jäsentäminen.

Prosessien luonti: Uusien prosessien käynnistäminen fork()- ja execvp()-funktioilla.

Prosessien hallinta: Lapsiprosessien suorituksen odottaminen waitpid()-funktiolla.

Dynaaminen muistinhallinta: Muistin varaaminen ja vapauttaminen komentojen käsittelyn yhteydessä.

Käytetyt teknologiat

C (C11)

POSIX-järjestelmärajapinnat

Clang

GNU Make

Vaatimukset

Projektin kääntäminen ja suorittaminen edellyttää:

Linux- tai muuta POSIX-yhteensopivaa ympäristöä

Clang-kääntäjää

GNU Makea

Asennus ja suorittaminen

Kloonaa repositorio:

git clone https://github.com/AhtoVii/c-shell.git
cd c-shell

Käännä projekti:

make

Käynnistä komentotulkki:

./shell

Poista käännöksessä syntyneet tiedostot:

make clean

Projektin rakenne

c-shell/
├── include/       # Otsikkotiedostot
├── src/           # Lähdekoodit
├── history.txt    # Projektin historia
├── steps.txt      # Kehitysvaiheiden muistiinpanot
└── makefile       # Käännösasetukset

Tavoite

Projektin tarkoituksena on syventää ymmärrystä C-ohjelmoinnista, käyttöjärjestelmien toiminnasta, prosessien hallinnasta sekä dynaamisesta muistinhallinnasta.

Projekti toimii myös käytännön harjoituksena Unix-järjestelmien prosessirajapintojen ja komentotulkkien toimintaperiaatteiden opiskeluun.
