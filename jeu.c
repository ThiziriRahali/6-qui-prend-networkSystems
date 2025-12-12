#include "jeu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>  // ← Ajoute cette ligne

/* ============ INITIALISATION ============ */

void Rangee_Init(Rangee *rangee) {
    if (rangee == NULL) return;
    rangee->nbCartes = 0;
    memset(rangee->cartes, 0, sizeof(rangee->cartes));
}

void Jeu_Init(Jeu *jeu, Joueur *joueurs, int nbJoueurs) {
    if (jeu == NULL || joueurs == NULL || nbJoueurs <= 0) return;
    
    jeu->joueurs = joueurs;
    jeu->nbJoueurs = nbJoueurs;
    jeu->tourActuel = 0;
    
    // Initialiser les 4 rangées
    for (int i = 0; i < NB_RANGEES_JEU; i++) {
        Rangee_Init(&jeu->table.rangees[i]);
    }
    
    printf("Jeu initialisé avec %d joueurs\n", nbJoueurs);
}

/* ============ RANGÉES ============ */

void Rangee_ajouterCarte(Rangee *rangee, Carte carte) {
    if (rangee == NULL || rangee->nbCartes >= NB_CARTES_MAX_RANGEE) return;
    
    rangee->cartes[rangee->nbCartes] = carte;
    rangee->nbCartes++;
}

Carte Rangee_derniereCarte(Rangee *rangee) {
    if (rangee == NULL || rangee->nbCartes == 0) {
        Carte carte_nulle = {0, 0};
        return carte_nulle;
    }
    return rangee->cartes[rangee->nbCartes - 1];
}

int Rangee_getTetesBoeuf(Rangee *rangee) {
    if (rangee == NULL) return 0;
    
    int total = 0;
    for (int i = 0; i < rangee->nbCartes; i++) {
        total += rangee->cartes[i].teteBoeuf;
    }
    return total;
}

/* ============ PLACEMENT DE CARTES ============ */

int Jeu_trouverMeilleureRangee(TableJeu *table, Carte carte) {
    if (table == NULL) return -1;
    
    int meilleure_rangee = -1;
    int max_valeur = -1;
    int min_tetes = INT_MAX;
    
    for (int i = 0; i < NB_RANGEES_JEU; i++) {
        Rangee *rangee = &table->rangees[i];
        
        // Si la rangée est pleine, passer
        if (rangee->nbCartes >= NB_CARTES_MAX_RANGEE) continue;
        
        // Si la carte est plus grande que la dernière carte de la rangée
        Carte derniere = Rangee_derniereCarte(rangee);
        if (carte.valeurNum > derniere.valeurNum) {
            // Prendre la rangée avec la plus grande valeur finale < carte
            if (derniere.valeurNum > max_valeur) {
                max_valeur = derniere.valeurNum;
                meilleure_rangee = i;
                min_tetes = Rangee_getTetesBoeuf(rangee);
            }
            // En cas d'égalité, prendre celle avec le moins de points
            else if (derniere.valeurNum == max_valeur) {
                int tetes_actuelles = Rangee_getTetesBoeuf(rangee);
                if (tetes_actuelles < min_tetes) {
                    min_tetes = tetes_actuelles;
                    meilleure_rangee = i;
                }
            }
        }
    }
    
    // Si aucune rangée ne convient, prendre celle avec le moins de points
    if (meilleure_rangee == -1) {
        min_tetes = INT_MAX;
        for (int i = 0; i < NB_RANGEES_JEU; i++) {
            int tetes = Rangee_getTetesBoeuf(&table->rangees[i]);
            if (tetes < min_tetes) {
                min_tetes = tetes;
                meilleure_rangee = i;
            }
        }
    }
    
    return meilleure_rangee;
}

void Jeu_placerCarte(Rangee *rangee, Carte carte) {
    if (rangee == NULL) return;
    Rangee_ajouterCarte(rangee, carte);
}

/* ============ PRISE DE RANGÉE ============ */

void Jeu_prendreRangee(Joueur *joueur, Rangee *rangee) {
    if (joueur == NULL || rangee == NULL) return;
    
    int points = Rangee_getTetesBoeuf(rangee);
    
    printf("Joueur %s prend une rangée et gagne %d points\n", joueur->nom, points);
    
    // Ajouter les points au joueur
    joueur->score += points;
    
    // Vider la rangée
    Rangee_Init(rangee);
}

int Jeu_calculerPointsRangee(Rangee *rangee) {
    if (rangee == NULL) return 0;
    return Rangee_getTetesBoeuf(rangee);
}

/* ============ DÉROULEMENT DU JEU ============ */

void Jeu_jouerTour(Jeu *jeu) {
    if (jeu == NULL || jeu->tourActuel >= NB_TOURS) return;
    
    jeu->tourActuel++;
    printf("\n========== TOUR %d ==========\n", jeu->tourActuel);
    
    // TODO: Implémenter la logique complète du tour
    // 1. Récupérer les cartes jouées par chaque joueur
    // 2. Trier les cartes par valeur
    // 3. Pour chaque carte, la placer sur une rangée
    // 4. Gérer les cas où la rangée est pleine
    // 5. Calculer les scores
    
    printf("Tour %d joué\n", jeu->tourActuel);
}

int Jeu_estTermine(Jeu *jeu) {
    if (jeu == NULL) return 0;
    return jeu->tourActuel >= NB_TOURS;
}

/* ============ AFFICHAGE ============ */

void Jeu_afficherTableau(TableJeu *table) {
    if (table == NULL) return;
    
    printf("\n=== TABLEAU DE JEU ===\n");
    for (int i = 0; i < NB_RANGEES_JEU; i++) {
        Rangee *rangee = &table->rangees[i];
        printf("Rangée %d (%d cartes, %d pts) : ", 
               i + 1, rangee->nbCartes, Rangee_getTetesBoeuf(rangee));
        
        for (int j = 0; j < rangee->nbCartes; j++) {
            printf("[%d] ", rangee->cartes[j].valeurNum);
        }
        printf("\n");
    }
}

void Jeu_afficherScores(Jeu *jeu) {
    if (jeu == NULL) return;
    
    printf("\n=== SCORES ACTUELS ===\n");
    for (int i = 0; i < jeu->nbJoueurs; i++) {
        printf("%s : %d points\n", jeu->joueurs[i].nom, jeu->joueurs[i].score);
    }
}

Joueur* Jeu_determinerGagnant(Jeu *jeu) {
    if (jeu == NULL || jeu->nbJoueurs <= 0) return NULL;
    
    Joueur *gagnant = &jeu->joueurs[0];
    
    for (int i = 1; i < jeu->nbJoueurs; i++) {
        if (jeu->joueurs[i].score < gagnant->score) {
            gagnant = &jeu->joueurs[i];
        }
    }
    
    return gagnant;
}