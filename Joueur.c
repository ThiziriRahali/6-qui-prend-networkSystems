#include <stdio.h>
#include <string.h>
#include "Joueur.h"
#include "Collection.h"

void Joueur_Init(Joueur *p, int x, char* y) {
    if (p == NULL || y == NULL) return;

    strncpy(p->ip, "0.0.0.0", MAX_CHARS - 1);
    p->ip[MAX_CHARS - 1] = '\0';

    strncpy(p->nom, y, MAX_CHARS - 1);
    p->nom[MAX_CHARS - 1] = '\0';

    p->score = x;
}

void Joueur_Method(Joueur *p, int dx, char* dy) {
    if (p == NULL) return;

    p->score += dx;

    if (dy != NULL) {
        strncpy(p->nom, dy, MAX_CHARS - 1);
        p->nom[MAX_CHARS - 1] = '\0';
    }
}

/* Affiche les cartes du joueur */
void Joueur_AfficherCartes(Joueur *p) {
    if (p == NULL) return;

    char *s = Collection_toString(&p->jeuCartes);
    if (s == NULL) return;

    printf("Cartes de %s :\n%s", p->nom, s);
    free(s);
}

void Joueur_retirerCarte(Joueur *p, int i) {
    if (p == NULL) return;

    Collection_retirerCarte(&p->jeuCartes, i);
}

void Joueur_updateScore(Joueur *p, Collection *c) {
    if (p == NULL || c == NULL) return;

    int delta = Collection_getScore(c);  
    p->score += delta;
}
