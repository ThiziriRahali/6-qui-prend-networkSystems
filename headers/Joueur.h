#ifndef JOUEUR_H
#define JOUEUR_H

#include "structures.h"

void Joueur_Init(Joueur *p, int x, char* y);
void Joueur_Method(Joueur *p, int dx, char* dy);
void Joueur_AfficherCartes(Joueur *p);
void Joueur_retirerCarte(Joueur *p, int i);
void Joueur_updateScore(Joueur *p, Collection *c);

#endif
