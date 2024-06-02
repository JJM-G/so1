#include <stdio.h>
#include <stdlib.h>

#define CRCsize 4
#define buforSize 1
#define kropka 46

// Definicje funkcji
int obliczCRC(char *nazwaPlikuIn, unsigned char reszta[], unsigned char wielomian[]);
int zmienRozszerzenieNazwy(char *nazwa, char *rozszerzenie, char *nazwaNowa);
int dopiszPrzyrostekDoNazwy(char *nazwa, char *przyrostek, char *nazwaNowa);
int zapiszCRC(char *nazwaPlikuIn, unsigned char reszta[], int licznikBajtow);



// Funkcja obliczaj¹ca CRC dla pliku o podanej nazwie
int obliczCRC(char *nazwaPlikuIn, unsigned char reszta[], unsigned char wielomian[])
{
    unsigned char kolejka[CRCsize + buforSize]; // Bufor przechowuj¹cy dane z pliku
    unsigned char buforWejscia[buforSize];      // Bufor na dane wejœciowe
    unsigned char maska = 128, ostatniBit, pierwszyBit; 
    int degreeGen = 8 * CRCsize; // Stopieñ wielomianu CRC
    int licznikBitow = 0;
    int licznikBajtowIn = 0, licznikBuforu = 0;
    int n, i;

   
    for (i = 0; i < CRCsize + buforSize; i++)
        kolejka[i] = 0;    // Zerowanie ca³ej kolejki

    FILE *wskaznikPlikuIn = NULL;
    wskaznikPlikuIn = fopen(nazwaPlikuIn, "rb");
    if (wskaznikPlikuIn == NULL)
    {
        printf("Nie mozna otworzyc pliku: %s  \n", nazwaPlikuIn);
        exit(EXIT_FAILURE);
    }

    while (n = fread(buforWejscia, sizeof(unsigned char), buforSize, wskaznikPlikuIn))
    {
        licznikBajtowIn += n;
       
        for (i = 0; i < n; i++)
            kolejka[CRCsize + i] = buforWejscia[i];   //dopisanie odczytanego fragmentu pliku do kolejki

        licznikBitow = 8 * n; //n- liczba odczytanych bajtów
        while (licznikBitow)
        {
            licznikBuforu = 1 + (licznikBitow - 1) / 8;

            pierwszyBit = maska & kolejka[0];

            // Przesuwanie kolejki o 1 bit w lewo
            for (i = 0; i < CRCsize + licznikBuforu; i++)
            {
                kolejka[i] = (kolejka[i] << 1);
                ostatniBit = kolejka[i + 1] >> 7; // wyd³ubanie pierwszego bitu w nastêpnym bajcie i zapisanie go jako ostatniBit
               
                kolejka[i] = kolejka[i] | ostatniBit;  // Przejêcie bitu z nastêpnego bajtu
            }

            // Wykonanie operacji XOR
            if (pierwszyBit!= 0) // jesli pierwszy bit jest jedynka
                for (i = 0; i < CRCsize; i++)
                {
                    kolejka[i] = kolejka[i] ^ wielomian[i];
                }
            licznikBitow--;
        }
    }

    fclose(wskaznikPlikuIn);

   
    licznikBitow = degreeGen;
    while (licznikBitow > 0)
    {
        pierwszyBit = maska & kolejka[0];
        for (i = 0; i < CRCsize; i++)
        {
            kolejka[i] = (kolejka[i] << 1);
            ostatniBit = kolejka[i + 1] >> 7;
            kolejka[i] = kolejka[i] | ostatniBit;
        }
        
        if (pierwszyBit!= 0) 
            for (i = 0; i < CRCsize; i++)
            {
                kolejka[i] = kolejka[i] ^ wielomian[i];
            }
        licznikBitow--;
    }

    for (i = 0; i < CRCsize; i++)
        reszta[i] = kolejka[i];

    return licznikBajtowIn;
}

// Funkcja zmieniaj¹ca rozszerzenie pliku
int zmienRozszerzenieNazwy(char *nazwa, char *rozszerzenie, char *nazwaNowa)
{
    char *w_nazwy;
    int i, n;

    //czyszczenie pamieci
    for (n = 0; n < 24; ++n)
        nazwaNowa[n] = 0;
    n = 0;
    w_nazwy = nazwa;
    nazwaNowa[n] = *w_nazwy;
    while ((nazwa[n] != kropka) && (nazwa[n] != '\0'))
    {
        n++;
        w_nazwy++;
        nazwaNowa[n] = *w_nazwy;
    }
    n++;
    i = 0;
    nazwaNowa[n] = rozszerzenie[i];
    while (rozszerzenie[i] != '\0')
    {
        n++;
        i++;
        nazwaNowa[n] = rozszerzenie[i];
    }

    return 0;
}

// Funkcja dopisuj¹ca przyrostek do nazwy pliku
int dopiszPrzyrostekDoNazwy(char *nazwa, char *przyrostek, char *nazwaNowa)
{
    int i = 0;
    int foundDot = 0; // Flaga wskazuj¹ca, czy znaleziono kropkê

    // Szukanie ostatniej kropki w nazwie pliku
    while (nazwa[i] != '\0')
    {
        if (nazwa[i] == '.')
            foundDot = i;
        i++;
    }

    // Kopiowanie oryginalnej nazwy pliku do nowej nazwy (bez poprzedniego rozszerzenia)
    for (i = 0; i < foundDot; i++)
    {
        nazwaNowa[i] = nazwa[i];
    }

    // Dodanie nowego rozszerzenia do nowej nazwy
    while (*przyrostek != '\0')
    {
        nazwaNowa[i++] = *przyrostek;
        przyrostek++;
    }

    // Zakoñczenie nowej nazwy znakiem NULL
    nazwaNowa[i] = '\0';

    return 0;
}

// Funkcja zapisuj¹ca wynik CRC do pliku
int zapiszCRC(char *nazwaPlikuIn, unsigned char reszta[], int licznikBajtow)
{
    char nazwaNowa[24]; // Maksymalna d³ugoœæ nazwy pliku
    int i;

    // Tworzenie nowej nazwy pliku z rozszerzeniem .CRC
    dopiszPrzyrostekDoNazwy(nazwaPlikuIn, ".CRC", nazwaNowa);

    // Zapisywanie wyniku CRC do pliku
    FILE *wskaznikPlikuOut = NULL;
    wskaznikPlikuOut = fopen(nazwaNowa, "wb");
    if (wskaznikPlikuOut == NULL)
    {
        printf("Nie mozna utworzyc pliku: %s\n", nazwaNowa);
        return 1;
    }

    // Zapisywanie wyniku CRC do pliku
    for (i = 0; i < CRCsize; i++)
    {
        fwrite(&reszta[i], sizeof(unsigned char), 1, wskaznikPlikuOut);
    }

    fclose(wskaznikPlikuOut);

    printf("Wynik CRC zapisano do pliku: %s\n", nazwaNowa);

    return 0;
}

// G³ówna funkcja programu
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Uzycie: %s <nazwa_pliku>\n", argv[0]);
        return 1;
    }

    unsigned char CRC[4];
    unsigned char wielomian[4] = {0, 0, 0x00, 0x02};
    int i;
    int licznikBajtow = 0;

    // Wyznaczanie CRC
    licznikBajtow = obliczCRC(argv[1], CRC, wielomian);

    // Zapisywanie wyniku CRC do pliku
    zapiszCRC(argv[1], CRC, licznikBajtow);

    return 0;
}


