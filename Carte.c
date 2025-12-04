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

int Carte_getTeteBoeuf(Carte *p) {
    if(p == NULL) {
        fprintf(stderr, "Erreur: pointeur Carte NULL dans Carte_getTeteBoeuf\n");
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