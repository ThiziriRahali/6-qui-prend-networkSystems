#include "../headers/global.h"

void Jeu_Init(Jeu *jeu, Joueur *joueurs, int nbJoueurs) {
}

void Jeu_jouerTour(Jeu *jeu) {
}

int Jeu_trouverMeilleureRangee(TableJeu *table, Carte carte) {
    return 0;
}

void Jeu_prendreRangee(Joueur *joueur, Rangee *rangee) {
}

int Jeu_calculerPointsRangee(Rangee *rangee) {
    return 0;
}

int Jeu_estTermine(Jeu *jeu) {
    return 0;
}

void Jeu_afficherTableau(TableJeu *table) {
}

void Jeu_afficherScores(Jeu *jeu) {
}

Joueur* Jeu_determinerGagnant(Jeu *jeu) {
    return NULL;
}

void Rangee_Init(Rangee *rangee) {
}

void Rangee_ajouterCarte(Rangee *rangee, Carte carte) {
}

Carte Rangee_derniereCarte(Rangee *rangee) {
    Carte c = {0, 0};
    return c;
}

int Rangee_getTetesBoeuf(Rangee *rangee) {
    return 0;
}
