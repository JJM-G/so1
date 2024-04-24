#include <stdio.h>
#include <string.h>

#define BUFSIZE 1024

typedef struct 
{
    char nazwaPliku[256];
    unsigned long liczbaBajtow;
} Dokumentacja;

typedef struct 
{
    unsigned char symbol;
    int liczba;
} SymbolLiczba;

void inicjalizujModelShannon(int modelShannon[256]) 
{
    int i;
    for (i = 0; i < 256; ++i) {
        modelShannon[i] = 0;
    }
}

void zliczSymbole(const char *nazwaPliku, int modelShannon[256], Dokumentacja *dok) 
{
    FILE *plik = fopen(nazwaPliku, "rb");
    if (!plik) {
        perror("Nie mo¿na otworzyæ pliku wejœciowego");
        return;
    }

    unsigned char bufor[BUFSIZE];
    size_t przeczytaneBajty;

    dok->liczbaBajtow = 0;
    while ((przeczytaneBajty = fread(bufor, 1, BUFSIZE, plik)) > 0) 
	{
        size_t i;
        for (i = 0; i < przeczytaneBajty; ++i)
		{
            modelShannon[bufor[i]]++;
        }
        dok->liczbaBajtow += przeczytaneBajty;
    }

    fclose(plik);
}

void zmienRozszerzeniePliku(const char *staryPlik, char *nowyPlik, const char *noweRozszerzenie) 
{
    strcpy(nowyPlik, staryPlik); 
    char *kropka = strrchr(nowyPlik, '.'); 
    if (kropka != NULL)
	{
        *kropka = '\0'; 
    }
    strcat(nowyPlik, noweRozszerzenie);
}

void zapiszLiczbeBajtow(const char *nazwaPliku, unsigned long liczbaBajtow) 
{
    char nazwaPlikuZBajtami[256];
    strcpy(nazwaPlikuZBajtami, nazwaPliku); 
    char *kropka = strrchr(nazwaPlikuZBajtami, '.'); 
    if (kropka != NULL) 
	{
        *kropka = '\0'; 
    }
    strcat(nazwaPlikuZBajtami, ".IleBajtow"); 

    FILE *plik = fopen(nazwaPlikuZBajtami, "w");
    if (!plik) 
	{
        perror("Nie mo¿na otworzyæ pliku do zapisu liczby bajtów");
        return;
    }

    fprintf(plik, "%lu", liczbaBajtow);
    fclose(plik);
}



int porownaj(const void *a, const void *b) 
{
    SymbolLiczba *item1 = (SymbolLiczba *)a;
    SymbolLiczba *item2 = (SymbolLiczba *)b;
    return item2->liczba - item1->liczba;
}

void zapiszModelSort(const char *nazwaPliku, SymbolLiczba modelShannon[], int rozmiar) 
{
    char nazwaPlikuModelSort[256];
    zmienRozszerzeniePliku(nazwaPliku, nazwaPlikuModelSort, ".modelSort");

    FILE *plik = fopen(nazwaPlikuModelSort, "w");
    if (!plik) {
        perror("Nie mo¿na otworzyæ pliku do zapisu posortowanego modelu");
        return;
    }


int i = 0;
    for ( i; i < rozmiar; ++i) 
	{
        if (modelShannon[i].liczba != 0) 
		{
            fprintf(plik, "%d %d\n", modelShannon[i].symbol, modelShannon[i].liczba);
        }
    }

    fclose(plik);
}

void zapiszModel(const char *nazwaPliku, int modelShannon[256]) 
{
    SymbolLiczba pary[256];
    int rozmiar = 0;

    FILE *plikModel = fopen(nazwaPliku, "w");
    if (!plikModel) 
	{
        perror("Nie mo¿na otworzyæ pliku do zapisu modelu");
        return;
    }


int i = 0;
    for (i;  i < 256; ++i) 
	{
        if (modelShannon[i] != 0) 
		{
            fprintf(plikModel, "%d %d\n", i, modelShannon[i]);
            pary[rozmiar].symbol = (unsigned char)i;
            pary[rozmiar].liczba = modelShannon[i];
            rozmiar++;
        }
    }

    fclose(plikModel);
    qsort(pary, rozmiar, sizeof(SymbolLiczba), porownaj);
    zapiszModelSort(nazwaPliku, pary, rozmiar);
}



int main(int argc, char *argv[])
{
    if (argc != 2) {
        return 1;
    }

    int modelShannon[256];
    inicjalizujModelShannon(modelShannon);

    Dokumentacja dok;
    strcpy(dok.nazwaPliku, argv[1]);
    zliczSymbole(argv[1], modelShannon, &dok);

    zapiszLiczbeBajtow(dok.nazwaPliku, dok.liczbaBajtow);
    char nazwaPlikuModel[256];
    zmienRozszerzeniePliku(argv[1], nazwaPlikuModel, ".model");
    zapiszModel(nazwaPlikuModel, modelShannon);

    return 0;
}

