#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "template.h"
#include "Carte.h"

/* “méthodes” */
void Carte_Init(Carte *p, int num, int tete) {
    if(p == NULL) {
        fprintf(stderr, "Erreur: pointeur Carte NULL dans Carte_Init\n");
        exit(EXIT_FAILURE);
    }
    if(num < 1 || num > 104) {
        fprintf(stderr, "Erreur: valeurNum invalide dans Carte_Init\n");
        exit(EXIT_FAILURE);
    }
    if(tete < 1 || tete > 7) {
        fprintf(stderr, "Erreur: teteBoeuf invalide dans Carte_Init\n");
        exit(EXIT_FAILURE);
    }
    p->valeurNum = num;  // de 1 à 104 //
    p->teteBoeuf = tete;  // de 1 à 7 //
}

int Carte_getValeurNum(Carte *p) {
    if(p == NULL) {
        fprintf(stderr, "Erreur: pointeur Carte NULL dans Carte_getValeurNum\n");
        exit(EXIT_FAILURE);
    }
    return p->valeurNum;
}

int Carte_getValeurTete(Carte *p) {
    if(p == NULL) {
        fprintf(stderr, "Erreur: pointeur Carte NULL dans Carte_getValeurTete\n");
        exit(EXIT_FAILURE);
    }
    return p->teteBoeuf;
}

void Carte_InitNum(Carte *p, int num) {
    if(p == NULL) {
        fprintf(stderr, "Erreur: pointeur Carte NULL dans Carte_InitNum\n");
        exit(EXIT_FAILURE);
    }
    if(num < 1 || num > 104) {
        fprintf(stderr, "Erreur: valeurNum invalide dans Carte_InitNum\n");
        exit(EXIT_FAILURE);
    }
    p->valeurNum = num;
    if(num == 55) {
        p->teteBoeuf = 7;
    } else if(num % 10 == 0) {
        p->teteBoeuf = 3;
    } else if(num % 5 == 0) {
        p->teteBoeuf = 2;
    } else if(num % 11 == 0) {
        p->teteBoeuf = 5;
    } else {
        p->teteBoeuf = 1;
    }
}

char* Carte_toString(Carte *p) {
    if (p == NULL) {
        fprintf(stderr, "Erreur: pointeur Carte NULL dans Carte_toString\n");
        exit(EXIT_FAILURE);
    }

    size_t size = 128;
    char *buffer = malloc(size);
    if (buffer == NULL) {
        fprintf(stderr, "Erreur: échec de l'allocation mémoire dans Carte_toString\n");
        exit(EXIT_FAILURE);
    }

    // Convertir la valeur numérique en string
    char valeur_str[10];
    snprintf(valeur_str, sizeof valeur_str, "%d", Carte_getValeurNum(p));

    // Générer les étoiles
    int nb_tetes = Carte_getValeurTete(p);
    char etoiles[10] = {0};
    for (int i = 0; i < nb_tetes && i < 9; i++) {
        etoiles[i] = '*';
    }
    etoiles[nb_tetes > 9 ? 9 : nb_tetes] = '\0';

    // Centrer la valeur numérique
    int len_valeur = strlen(valeur_str);
    int padding_left_val = (8 - len_valeur) / 2;
    int padding_right_val = 8 - len_valeur - padding_left_val;

    char valeur_centree[20];  // Buffer plus grand
    snprintf(valeur_centree, sizeof valeur_centree,
        "%*s%s%*s",
        padding_left_val, "",
        valeur_str,
        padding_right_val, "");

    // Centrer les étoiles
    int len_etoiles = strlen(etoiles);
    int padding_left_et = (8 - len_etoiles) / 2;
    int padding_right_et = 8 - len_etoiles - padding_left_et;

    char etoiles_centrees[20];  // Buffer plus grand
    snprintf(etoiles_centrees, sizeof etoiles_centrees,
        "%*s%s%*s",
        padding_left_et, "",
        etoiles,
        padding_right_et, "");

    snprintf(buffer, size,
        "------------\n"
        "|          |\n"
        "| %s |\n"
        "|          |\n"
        "| %s |\n"
        "|          |\n"
        "------------",
        valeur_centree,
        etoiles_centrees);

    return buffer;
}

char* Carte_toStringCache(Carte *p){
    if (p == NULL) {
        fprintf(stderr, "Erreur: pointeur Carte NULL dans Carte_toString\n");
        exit(EXIT_FAILURE);
    }

    size_t size = 128;
    char *buffer = malloc(size);
    if (buffer == NULL) {
        fprintf(stderr, "Erreur: échec de l'allocation mémoire dans Carte_toString\n");
        exit(EXIT_FAILURE);
    }
    
    snprintf(buffer, size,
        "------------\n"
        "|          |\n"
        "|    %s    |\n"
        "|          |\n"
        "|    %s    |\n"
        "|          |\n"
        "------------",
        "??",
        "??");
    
    return buffer;
}