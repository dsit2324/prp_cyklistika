#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 150
#define INPUTFILE "cyklistika.txt"
#define OUTPUTFILE "vysledkova_listina.txt"
#define DELIMITERS ";:"

typedef struct {
    int hodiny;
    int minuty;
    float sekundy;
} CAS;

typedef struct {
    int cislo;
    char prijmeni[15];
    char jmeno[15];
    char narodnost[5];
    int rocnik;
    char klub[30];
    CAS cas;
} CYKLISTA;

CYKLISTA *readFile(CYKLISTA *cyklista, int *pocet) {
    FILE *fr = fopen(INPUTFILE, "r");
    if (fr == NULL) {
        printf("Nepodarilo se otevrit soubor %s.\n", INPUTFILE);
        return NULL;
    }
    char radek[MAX];
    int i = 0;
    int j;
    char *token;
    CYKLISTA *pomocny;

    while (fgets(radek, MAX, fr) != NULL) {
        if (i != 0) {
            pomocny = (CYKLISTA *) realloc(cyklista, sizeof(CYKLISTA) * i);
            if (pomocny == NULL) {
                printf("Neuspesna realokace.\n");
                free(cyklista);
                fclose(fr);
                return NULL;
            } else {
                cyklista = pomocny;
                token = strtok(radek, DELIMITERS);
                j = 0;
                while (token != NULL) {
                    switch (j) {
                        case 0:
                            cyklista[i - 1].cislo = atoi(token);
                            break;
                        case 1:
                            strcpy(cyklista[i - 1].prijmeni, token);
                            break;
                        case 2:
                            strcpy(cyklista[i - 1].jmeno, token);
                            break;
                        case 3:
                            strcpy(cyklista[i - 1].narodnost, token);
                            break;
                        case 4:
                            cyklista[i - 1].rocnik = atoi(token);
                            break;
                        case 5:
                            strcpy(cyklista[i - 1].klub, token);
                            break;
                        case 6:
                            cyklista[i - 1].cas.hodiny = atoi(token);
                            break;
                        case 7:
                            cyklista[i - 1].cas.minuty = atoi(token);
                            break;
                        case 8:
                            cyklista[i - 1].cas.sekundy = atof(token);
                            break;
                    }
                    j++;
                    token = strtok(NULL, DELIMITERS);
                }
            }
        }
        i++;
    }
    if (fclose(fr) == EOF) {
        printf("Nepodarilo se zavrit soubor %s.\n", INPUTFILE);
    }
    *pocet = i - 1;
    return cyklista;
}

int systemTime(int rocnik) {
    time_t t = time(NULL);
    struct tm *tim = localtime(&t);
    return (tim->tm_year + 1900) - rocnik; // Opraveno (chybělo mínus)
}

void vypisNaKonzoli(CYKLISTA *cyklista, int pocet) {
    int cze = 0;
    int svk = 0;
    int jine = 0;
    int nejstarsi = 3000;
    int index = 0;

    printf("S T A R T O V N I  L I S T I N A  -  O B R  D R A S A L\n");
    printf("----------------------------------------------------------------------------------------------------------\n");
    printf("startovni cislo|   prijmeni    |  jmeno   |narodnost|rocnik|            klub                   |   cas   |\n");
    printf("----------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < pocet; i++) {
        if (strcmp(cyklista[i].narodnost, "CZE") == 0) {
            cze++;
        } else if (strcmp(cyklista[i].narodnost, "SVK") == 0) {
            svk++;
        } else {
            jine++;
        }
        if (cyklista[i].rocnik < nejstarsi) {
            nejstarsi = cyklista[i].rocnik;
            index = i;
        }
        printf("%15d|%15s|%10s|%9s|%6d|%35s|%d:%02d:%04.1f\n", cyklista[i].cislo, cyklista[i].prijmeni,
               cyklista[i].jmeno, cyklista[i].narodnost, cyklista[i].rocnik, cyklista[i].klub,
               cyklista[i].cas.hodiny, cyklista[i].cas.minuty, cyklista[i].cas.sekundy);
    }
    printf("----------------------------------------------------------------------------------------------------------\n");
    printf("Na startu je %d zavodniku.\n", pocet);
    printf("Nejstarsim zavodnikem je %s %s narozen v roce %d. Je mu %d let.\n", cyklista[index].jmeno,
           cyklista[index].prijmeni, cyklista[index].rocnik, systemTime(cyklista[index].rocnik));
    printf("Zavodnici dle narodnosti: Cesi - %d\n", cze);
    printf("                          Slovaci - %d\n", svk);
    printf("                          Jine - %d\n", jine);
}

float prevodCasu(CAS cas) {
    return (float) cas.hodiny * 3600 + (float) cas.minuty * 60 + cas.sekundy; // Opraveno (chyběly operátory *)
}

void swap(CYKLISTA *cyklista1, CYKLISTA *cyklista2) {
    CYKLISTA temp = *cyklista1;
    *cyklista1 = *cyklista2;
    *cyklista2 = temp;
}

void bubbleSort(CYKLISTA *cyklista, int pocet) {
    for (int i = 0; i < pocet - 1; i++) {
        for (int j = 0; j < pocet - 1 - i; j++) { // Opraveno (chybělo - i)
            if (prevodCasu(cyklista[j].cas) > prevodCasu(cyklista[j + 1].cas)) {
                swap(&cyklista[j], &cyklista[j + 1]);
            }
        }
    }
}

void doSouboru(CYKLISTA *cyklista, int pocet) {
    FILE *fw = fopen(OUTPUTFILE, "w");
    if (fw == NULL) {
        printf("Nepodarilo se otevrit soubor %s.\n", OUTPUTFILE);
        return;
    }
    float ztrata; // Změněno z int na float kvůli přesnosti desetiných míst
    bubbleSort(cyklista, pocet);

    printf("Byl vytvoren soubor %s.\n", OUTPUTFILE);
    fprintf(fw, "V Y S L E D K O V A  L I S T I N A  -  O B R  D R A S A L\n");
    fprintf(fw, "----------------------------------------------------------------------------------------------------------------------------\n");
    fprintf(fw, "poradi|startovni cislo|   prijmeni    |  jmeno   |narodnost|rocnik|            klub                |   cas   |  ztrata  |\n");
    fprintf(fw, "----------------------------------------------------------------------------------------------------------------------------\n");

    // Výpis prvního místa (vítěz má ztrátu 0)
    fprintf(fw, "%6d|%15d|%15s|%10s|%9s|%6d|%32s|%d:%02d:%04.1f|\n",
            1, cyklista[0].cislo, cyklista[0].prijmeni, cyklista[0].jmeno,
            cyklista[0].narodnost, cyklista[0].rocnik, cyklista[0].klub,
            cyklista[0].cas.hodiny, cyklista[0].cas.minuty, cyklista[0].cas.sekundy);

    // Výpis ostatních závodníků se spočítanou ztrátou
    for (int i = 1; i < pocet; i++) {
        ztrata = prevodCasu(cyklista[i].cas) - prevodCasu(cyklista[0].cas);

        int ztrata_h = (int)(ztrata / 3600);
        int ztrata_m = ((int)ztrata % 3600) / 60;
        float ztrata_s = ztrata - ((float)ztrata_h * 3600 + (float)ztrata_m * 60);

        fprintf(fw, "%6d|%15d|%15s|%10s|%9s|%6d|%32s|%d:%02d:%04.1f|+%d:%02d:%04.1f\n",
                i + 1, cyklista[i].cislo, cyklista[i].prijmeni, cyklista[i].jmeno,
                cyklista[i].narodnost, cyklista[i].rocnik, cyklista[i].klub,
                cyklista[i].cas.hodiny, cyklista[i].cas.minuty, cyklista[i].cas.sekundy,
                ztrata_h, ztrata_m, ztrata_s);
    }
    if (fclose(fw) == EOF) {
        printf("Nepodarilo se zavrit soubor %s.\n", OUTPUTFILE); // Opraveno %S na %s a doplněn OUTPUTFILE
    }
}

int main(void) {
    CYKLISTA *cyklista = NULL;
    int pocet = 0;
    cyklista = readFile(cyklista, &pocet);
    vypisNaKonzoli(cyklista, pocet);
    doSouboru(cyklista, pocet);
    free(cyklista);
    return 0;
}
