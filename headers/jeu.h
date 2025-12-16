#ifndef JEU_H
#define JEU_H

#include "structures.h"

void Jeu_Init(Jeu *jeu, Joueur *joueurs, int nbJoueurs);
void Jeu_jouerTour(Jeu *jeu);
int Jeu_trouverMeilleureRangee(TableJeu *table, Carte carte);
void Jeu_prendreRangee(Joueur *joueur, Rangee *rangee);
int Jeu_calculerPointsRangee(Rangee *rangee);
int Jeu_estTermine(Jeu *jeu);
void Jeu_afficherTableau(TableJeu *table);
void Jeu_afficherScores(Jeu *jeu);
Joueur* Jeu_determinerGagnant(Jeu *jeu);

void Rangee_Init(Rangee *rangee);
void Rangee_ajouterCarte(Rangee *rangee, Carte carte);
Carte Rangee_derniereCarte(Rangee *rangee);
int Rangee_getTetesBoeuf(Rangee *rangee);

#endif
