#ifndef JEU_H
#define JEU_H

#include "Carte.h"
#include "Collection.h"
#include "Joueur.h"

/**
 * ========================================
 * RÈGLES DU JEU "6 QUI PREND"
 * ========================================
 * 
 * 1. SETUP :
 *    - 2-10 joueurs
 *    - Deck de 104 cartes numérotées 1-104
 *    - Chaque carte a 1-7 "têtes de boeuf" (points négatifs)
 *    - 4 rangées sur la table
 *    
 * 2. DISTRIBUTION :
 *    - Chaque joueur reçoit 10 cartes
 *    - 4 cartes ouvertes sur la table (une par rangée)
 *    
 * 3. TOUR DE JEU :
 *    a) Tous les joueurs jouent une carte SIMULTANÉMENT (face cachée)
 *    b) Les cartes sont révélées par ordre croissant de valeur
 *    c) Chaque carte est placée à la rangée appropriée :
 *       - La carte doit être plus haute que la dernière de la rangée
 *       - Si aucune rangée ne convient : le joueur prend la rangée entière
 *       - Si plusieurs rangées conviennent : placer à la plus proche
 *    d) Si une rangée atteint 6 cartes : le joueur prend la rangée entière
 *       
 * 4. PLACEMENT D'UNE CARTE :
 *    - Chercher la rangée où valeur_carte > valeur_dernière_carte
 *    - Choisir la rangée avec la valeur MAXIMALE < valeur_carte
 *    - Si aucune : prendre la rangée la plus proche (avec moins de points)
 *    
 * 5. FIN DE TOUR :
 *    - Compter les "têtes de boeuf" des cartes prises
 *    - Ajouter au score du joueur (moins = mieux)
 *    
 * 6. FIN DE JEU :
 *    - Après 10 tours (100 cartes jouées)
 *    - Le joueur avec le MOINS de points gagne
 *    
 * ========================================
 */

#define NB_RANGEES_JEU 4
#define NB_CARTES_MAX_RANGEE 6
#define NB_CARTES_PAR_JOUEUR 10
#define NB_TOURS 10
#define DECK_TOTAL 104

// Structure d'une rangée (pile de cartes)
typedef struct {
    Carte cartes[NB_CARTES_MAX_RANGEE];  // Max 6 cartes par rangée
    int nbCartes;                        // Nombre actuel de cartes
} Rangee;

// Structure du tableau de jeu
typedef struct {
    Rangee rangees[NB_RANGEES_JEU];      // 4 rangées
    int tour;                             // Tour actuel (1-10)
} TableJeu;

// Structure de l'état du jeu
typedef struct {
    Joueur *joueurs;                     // Tableau des joueurs
    int nbJoueurs;                       // Nombre de joueurs
    TableJeu table;                      // L'état du tableau
    Collection deck;                     // Deck de cartes restantes
    int tourActuel;                      // Tour courant (1-10)
} Jeu;

/* ============ FONCTIONS DE JEU ============ */

// Initialisation du jeu
void Jeu_Init(Jeu *jeu, Joueur *joueurs, int nbJoueurs);

// Jouer un tour complet
void Jeu_jouerTour(Jeu *jeu);

// Placer une carte sur une rangée
void Jeu_placerCarte(Rangee *rangee, Carte carte);

// Trouver la meilleure rangée pour une carte
int Jeu_trouverMeilleureRangee(TableJeu *table, Carte carte);

// Récupérer une rangée (le joueur prend toutes les cartes)
void Jeu_prendreRangee(Joueur *joueur, Rangee *rangee);

// Calculer le score des têtes de boeuf
int Jeu_calculerPointsRangee(Rangee *rangee);

// Vérifier si le jeu est terminé
int Jeu_estTermine(Jeu *jeu);

// Afficher l'état du tableau
void Jeu_afficherTableau(TableJeu *table);

// Afficher les scores
void Jeu_afficherScores(Jeu *jeu);

// Déterminer le gagnant
Joueur* Jeu_determinerGagnant(Jeu *jeu);

// Réinitialiser une rangée
void Rangee_Init(Rangee *rangee);

// Ajouter une carte à une rangée
void Rangee_ajouterCarte(Rangee *rangee, Carte carte);

// Obtenir la dernière carte d'une rangée
Carte Rangee_derniereCarte(Rangee *rangee);

// Obtenir la somme des têtes de boeuf d'une rangée
int Rangee_getTetesBoeuf(Rangee *rangee);

#endif