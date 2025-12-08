#ifndef GESTIONNAIREJEU
#define GESTIONNAIREJEU

#include "Joueur.h"

#define MAX_SIZE 128

typedef struct {
    char ip[MAX_SIZE];
    int port;
    Joueur listeJoueurs[10];
    Collection *carteNonDistribuees;
    Collection *carteDejaDistribuees;
    Collection colonnes[4];
} GestionnaireJeu;

/* “méthodes” */
void GestionnaireJeu_Init(GestionnaireJeu *p, char *ip, int port);
void GestionnaireJeu_AjouteJoueur(GestionnaireJeu *p, Joueur *j);

void GestionnaireJeu_poserCarte(GestionnaireJeu *p, int colonne, Carte *c);

#endif
