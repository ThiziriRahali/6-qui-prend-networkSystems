#ifndef JOUEUR
#define JOUEUR

#include "Collection.h"

#define MAX_CHARS 256

typedef struct {
    char ip[MAX_CHARS];
    char nom[MAX_CHARS];
    Collection jeuCartes;
    int score;
} Joueur;

/* “méthodes” */
void Joueur_Init(Joueur *p, int x, char* y);
void Joueur_Method(Joueur *p, int dx, char* dy);

void Joueur_AfficherCartes(Joueur *p);

void Joueur_retirerCarte(Joueur *p, int i);

void Joueur_updateScore(Joueur *p, Collection *c);

#endif
