#ifndef JOUEUR
#define JOUEUR

#include "Collection.h"

#define MAX_CHARS 256

typedef struct {
    char ip[MAX_CHARS];
    char nom[MAX_CHARS];
    Collection jeuCartes;
    int score;
    int socket;           // Socket pour communication réseau (-1 pour bot)
    int is_bot;           // 1 si c'est un bot, 0 si humain
    int id;               // ID du joueur (index)
} Joueur;

/* "éthodes" */
void Joueur_Init(Joueur *p, int x, char* y);
void Joueur_Method(Joueur *p, int dx, char* dy);

void Joueur_AfficherCartes(Joueur *p);

void Joueur_retirerCarte(Joueur *p, int i);

void Joueur_updateScore(Joueur *p, Collection *c);

#endif
