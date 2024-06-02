#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 216
#define kropka 46

struct slowoDlaSymbolu {
    int symbol;
    unsigned char slowo[4];
    int liczbaBitow;
};

struct galaz {
    int ojciec;
    int potomek1;
    int potomek2;
};

struct zrodlo {
    char nazwaPlikuSource[24];
    int ileBajtow;
    int ileSymboli;
    char nazwaPlikuModel[24];
    char nazwaPlikuModelSort[24];
    char nazwaPlikuIleBajtow[24];
    char nazwaPlikuDrzewo[24];
    char nazwaPlikuTabelaKoduFull[24];
    char nazwaPlikuTabelaKodu[24];
    char nazwaPlikuOut[24];
};

struct opisSymbol {
    int symbol;
    int licznosc;
};



struct slowoDlaSymbolu tablicaKoduFull[512];
struct slowoDlaSymbolu tablicaKodu[256];
struct galaz drzewo[256];
struct zrodlo dokumentacja;
struct opisSymbol modelShannon[256];
struct opisSymbol modelSortShannon[256];

void zapiszTablicaKodu(int liczbaSymboli, struct slowoDlaSymbolu tablicaKodu[], char *nazwaPliku) {
    int k, i;
    unsigned char Four[4];
    FILE *wskaznikPliku = NULL;
    wskaznikPliku = fopen(nazwaPliku, "wb");

    if (wskaznikPliku == NULL) {
        printf("Nie mozna utworzyc pliku: %s \n", nazwaPliku);
        exit(EXIT_FAILURE);
    }

    printf("Tablica kodu w zapisie bajtowym do pliku %s \n", nazwaPliku);
    for (k = 0; k < liczbaSymboli; ++k) {
        for (i = 0; i < 4; ++i)
            Four[i] = tablicaKodu[k].slowo[i];
        printf(" %d %d  %d  %d %d %d \n", tablicaKodu[k].symbol, Four[0], Four[1], Four[2], Four[3], tablicaKodu[k].liczbaBitow);
        fprintf(wskaznikPliku, " %d %d  %d  %d %d %d \n", tablicaKodu[k].symbol, Four[0], Four[1], Four[2], Four[3], tablicaKodu[k].liczbaBitow);
    }
    fclose(wskaznikPliku);
}

int porownajLiczebnosci(const void *a, const void *b) {
    struct opisSymbol *pierwszy = (struct opisSymbol *)a;
    struct opisSymbol *drugi = (struct opisSymbol *)b;
    return (drugi->licznosc - pierwszy->licznosc);
}

void zrobTablicaKodu() {
    int liczbaSymboli = dokumentacja.ileSymboli;
    int symbol;
    int n, k, i;
    int indeksOjca, symbolOjca;
    int licznikDrzewa;
    int licznikKodu;
    int dlugoscCiagu = 0;
    int pozycja, ktoryBajt, ktoryBit;
    unsigned char four[4];
    unsigned char bajt, jedynka = 1;

    for (k = 0; k < 512; ++k) {
        for (i = 0; i < 4; ++i)
            tablicaKoduFull[k].slowo[i] = 0;
        tablicaKoduFull[k].symbol = 0;
        tablicaKoduFull[k].liczbaBitow = 0;
    }

    licznikDrzewa = liczbaSymboli - 2;
    licznikKodu = 0;
    tablicaKoduFull[licznikKodu].symbol = drzewo[licznikDrzewa].ojciec;

    while (licznikDrzewa + 1) {
        symbolOjca = drzewo[licznikDrzewa].ojciec;
        tablicaKoduFull[licznikKodu + 1].symbol = drzewo[licznikDrzewa].potomek1;
        tablicaKoduFull[licznikKodu + 2].symbol = drzewo[licznikDrzewa].potomek2;

        indeksOjca = -1;
        for (i = 0; i < licznikKodu + 1; ++i) {
            if ((tablicaKoduFull[i].symbol) == (symbolOjca)) {
                indeksOjca = i;
                break;
            }
        }
        if (indeksOjca == -1) {
            printf("Blad tworzenie tablicy kodu\n");
            exit(EXIT_FAILURE);
        }

        dlugoscCiagu = tablicaKoduFull[indeksOjca].liczbaBitow;

        for (i = 0; i < 4; ++i) {
            tablicaKoduFull[licznikKodu + 1].slowo[i] = tablicaKoduFull[indeksOjca].slowo[i];
            tablicaKoduFull[licznikKodu + 2].slowo[i] = tablicaKoduFull[indeksOjca].slowo[i];
        }

        pozycja = dlugoscCiagu;
        ktoryBajt = pozycja / 8;
        ktoryBit = pozycja % 8;
        jedynka = 1;
        jedynka = jedynka << (7 - ktoryBit);

        bajt = tablicaKoduFull[indeksOjca].slowo[ktoryBajt];
        bajt = bajt | jedynka;

        tablicaKoduFull[licznikKodu + 1].slowo[ktoryBajt] = bajt;
        tablicaKoduFull[licznikKodu + 1].liczbaBitow = dlugoscCiagu + 1;
        tablicaKoduFull[licznikKodu + 2].liczbaBitow = dlugoscCiagu + 1;
        licznikKodu += 2;
        licznikDrzewa--;
    }

    zapiszTablicaKodu(licznikKodu + 1, tablicaKoduFull, dokumentacja.nazwaPlikuTabelaKoduFull);

    licznikKodu = 0;
    for (k = 0; k < 256; ++k) {
        for (i = 0; i < 4; ++i)
            tablicaKodu[k].slowo[i] = 0;
        tablicaKodu[k].symbol = 0;
        tablicaKodu[k].liczbaBitow = 0;
    }

    for (k = 0; k < 2 * liczbaSymboli - 1; ++k) {
        symbol = tablicaKoduFull[k].symbol;
        if (symbol < 256) {
            tablicaKodu[licznikKodu].symbol = symbol;
            tablicaKodu[licznikKodu].liczbaBitow = tablicaKoduFull[k].liczbaBitow;
            for (i = 0; i < 4; ++i)
                tablicaKodu[licznikKodu].slowo[i] = tablicaKoduFull[k].slowo[i];
            licznikKodu++;
        }
    }

    zapiszTablicaKodu(liczbaSymboli, tablicaKodu, dokumentacja.nazwaPlikuTabelaKodu);
}

void zrobDrzewo() {
    int liczbaSymboli = dokumentacja.ileSymboli;
    int licznikSymboli = 0;
    int licznikSymboliDodanych = 0;
    int n, k;
    struct opisSymbol modelDlaDrzewa[256];

    for (k = 0; k < liczbaSymboli; k++) {
        modelDlaDrzewa[k].symbol = modelSortShannon[k].symbol;
        modelDlaDrzewa[k].licznosc = modelSortShannon[k].licznosc;
    }

    licznikSymboli = dokumentacja.ileSymboli;
    for (k = 0; k < liczbaSymboli - 1; k++) {
        drzewo[k].ojciec = 256 + k;
        drzewo[k].potomek1 = modelDlaDrzewa[liczbaSymboli - 1 - k].symbol;
        drzewo[k].potomek2 = modelDlaDrzewa[liczbaSymboli - 2 - k].symbol;
        modelDlaDrzewa[liczbaSymboli - 2 - k].symbol = 256 + k;
        modelDlaDrzewa[liczbaSymboli - 2 - k].licznosc = modelDlaDrzewa[liczbaSymboli - 1 - k].licznosc + modelDlaDrzewa[liczbaSymboli - 2 - k].licznosc;
        licznikSymboli--;
        qsort(modelDlaDrzewa, licznikSymboli, sizeof(struct opisSymbol), porownajLiczebnosci);
    }

    FILE *plikDrzewa = NULL;
    plikDrzewa = fopen(dokumentacja.nazwaPlikuDrzewo, "wb");
    if (plikDrzewa == NULL) {
        printf("Nie mozna otworzyc pliku %s\n", dokumentacja.nazwaPlikuDrzewo);
        exit(EXIT_FAILURE);
    }

    for (k = 0; k < liczbaSymboli - 1; k++) {
        fprintf(plikDrzewa, " %d %d %d \n", drzewo[k].ojciec, drzewo[k].potomek1, drzewo[k].potomek2);
    }
    fclose(plikDrzewa);
}

void zmienRozszerzenieNazwy(char *nazwaPliku, char *rozszerzenie, char *nowaNazwaPliku) {
    int i = 0;
    while ((nazwaPliku[i] != '.') && (nazwaPliku[i] != '\0')) {
        nowaNazwaPliku[i] = nazwaPliku[i];
        i++;
    }
    nowaNazwaPliku[i] = '.';
    nowaNazwaPliku[i + 1] = '\0';
    strcat(nowaNazwaPliku, rozszerzenie);
}

void wyznaczModel() {
    FILE *wejscie;
    int k, n, i;
    unsigned char buf[BUFSIZE];
    unsigned char symbol;
    int ile, licznikBajtow = 0;
    int model[256];

    for (k = 0; k < 256; ++k)
        model[k] = 0;

    if ((wejscie = fopen(dokumentacja.nazwaPlikuSource, "rb")) == NULL) {
        printf("Nie mozna otworzyc pliku: %s\n", dokumentacja.nazwaPlikuSource);
        exit(EXIT_FAILURE);
    }

    while ((ile = fread(buf, 1, BUFSIZE, wejscie)) > 0) {
        licznikBajtow += ile;
        for (k = 0; k < ile; ++k) {
            symbol = buf[k];
            model[symbol] += 1;
        }
    }
    fclose(wejscie);

    dokumentacja.ileBajtow = licznikBajtow;
    FILE *wyjscie = NULL;
    wyjscie = fopen(dokumentacja.nazwaPlikuModel, "wb");
    if (wyjscie == NULL) {
        printf("Nie mozna otworzyc pliku %s \n", dokumentacja.nazwaPlikuModel);
        exit(EXIT_FAILURE);
    }
    FILE *plikRozmiaru = NULL;
    plikRozmiaru = fopen(dokumentacja.nazwaPlikuIleBajtow, "wb");
    if (plikRozmiaru == NULL) {
        printf("Nie mozna otworzyc pliku %s \n", dokumentacja.nazwaPlikuIleBajtow);
        exit(EXIT_FAILURE);
    }

    fprintf(plikRozmiaru, "Rozmiar pliku: %d \n", licznikBajtow);
    fclose(plikRozmiaru);

    n = 0;
    for (k = 0; k < 256; ++k) {
        if (model[k]) {
            fprintf(wyjscie, " %d %d \n", k, model[k]);
            modelShannon[n].symbol = k;
            modelShannon[n].licznosc = model[k];
            n++;
        }
    }
    dokumentacja.ileSymboli = n;
    fclose(wyjscie);
}

void sortujModel() {
    FILE *wejscie, *wyjscie;
    int n, k;
    int ile;
    int symbol, licznosc;

    wejscie = fopen(dokumentacja.nazwaPlikuModel, "r");
    if (wejscie == NULL) {
        printf("Nie mozna otworzyc pliku %s \n", dokumentacja.nazwaPlikuModel);
        exit(EXIT_FAILURE);
    }
    wyjscie = fopen(dokumentacja.nazwaPlikuModelSort, "wb");
    if (wyjscie == NULL) {
        printf("Nie mozna otworzyc pliku %s \n", dokumentacja.nazwaPlikuModelSort);
        exit(EXIT_FAILURE);
    }

    n = 0;
    while (fscanf(wejscie, " %d %d \n", &symbol, &licznosc) != EOF) {
        modelShannon[n].symbol = symbol;
        modelShannon[n].licznosc = licznosc;
        n++;
    }
    qsort(modelShannon, dokumentacja.ileSymboli, sizeof(struct opisSymbol), porownajLiczebnosci);

    for (k = 0; k < dokumentacja.ileSymboli; ++k) {
        fprintf(wyjscie, " %d %d \n", modelShannon[k].symbol, modelShannon[k].licznosc);
        modelSortShannon[k].symbol = modelShannon[k].symbol;
        modelSortShannon[k].licznosc = modelShannon[k].licznosc;
    }
    fclose(wejscie);
    fclose(wyjscie);
}



void przepiszNazwe(char *nazwaPliku) {
    int i = 0;
    while (nazwaPliku[i] != '\0') {
        dokumentacja.nazwaPlikuSource[i] = nazwaPliku[i];
        i++;
    }
    dokumentacja.nazwaPlikuSource[i] = '\0';
    zmienRozszerzenieNazwy(nazwaPliku, "model", dokumentacja.nazwaPlikuModel);
    zmienRozszerzenieNazwy(nazwaPliku, "modelSort", dokumentacja.nazwaPlikuModelSort);
    zmienRozszerzenieNazwy(nazwaPliku, "ileBajtow", dokumentacja.nazwaPlikuIleBajtow);
    zmienRozszerzenieNazwy(nazwaPliku, "drzewo", dokumentacja.nazwaPlikuDrzewo);
    zmienRozszerzenieNazwy(nazwaPliku, "tabelaKoduFull", dokumentacja.nazwaPlikuTabelaKoduFull);
    zmienRozszerzenieNazwy(nazwaPliku, "tabelaKodu", dokumentacja.nazwaPlikuTabelaKodu);
    zmienRozszerzenieNazwy(nazwaPliku, "zip", dokumentacja.nazwaPlikuOut);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    przepiszNazwe(argv[1]);
    wyznaczModel();
    sortujModel();
    zrobDrzewo();
    zrobTablicaKodu();

    return 0;
}

