#ifndef GESTIONNAIREJEU_H
#define GESTIONNAIREJEU_H

#include "structures.h"

void GestionnaireJeu_Init(GestionnaireJeu *p, char *ip, int port);
void GestionnaireJeu_AjouteJoueur(GestionnaireJeu *p, Joueur *j);
int GestionnaireJeu_poserCarte(GestionnaireJeu *p, int colonne, Carte *c);

#endif
