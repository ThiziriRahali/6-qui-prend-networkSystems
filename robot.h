#ifndef ROBOT_H
#define ROBOT_H

#include "Carte.h"
#include "Collection.h"

#define MAX_NAME_LEN 32
#define MAX_HAND_SIZE 10
#define ROBOT_TIMEOUT 30

/**
 * Structure pour un joueur robot
 */
typedef struct {
    int socket;              // Socket de connexion au serveur
    char nom[MAX_NAME_LEN];  // Nom du robot
    Carte main[MAX_HAND_SIZE];  // Les cartes en main du robot
    int nb_cartes;           // Nombre de cartes actuelles
    int score;               // Score actuel du robot
} Robot;

/**
 * Structure pour l'etat du jeu recu du serveur
 */
typedef struct {
    Carte rangees[4][6];     // Les 4 rangees de jeu (max 6 cartes par rangee)
    int nb_cartes_rangee[4]; // Nombre de cartes dans chaque rangee
    int scores[10];          // Scores des joueurs
    int nb_joueurs;          // Nombre total de joueurs
} EtatJeu;

/* ============ FONCTIONS DU ROBOT ============ */

/**
 * Initialiser le robot
 */
Robot* Robot_Init(const char *nom, int socket);

/**
 * Envoyer le nom du robot au serveur
 */
int Robot_EnvoyerNom(Robot *robot);

/**
 * Recevoir les cartes initiales du serveur
 */
int Robot_RecevoirMain(Robot *robot, Carte *cartes, int nb_cartes);

/**
 * Choisir une carte a jouer
 * Retourne l'indice de la carte selectionnee
 */
int Robot_ChoisirCarte(Robot *robot);

/**
 * Strategie simple : jouer une carte aleatoire
 */
int Robot_Strategie_Aleatoire(Robot *robot);

/**
 * Strategie 2 : jouer la plus petite carte
 */
int Robot_Strategie_PlusPetite(Robot *robot);

/**
 * Strategie avancee : analyser les rangees et minimiser les pertes
 */
int Robot_Strategie_Intelligente(Robot *robot, EtatJeu *etat);

/**
 * Envoyer la carte choisie au serveur
 */
int Robot_EnvoyerCarte(Robot *robot, int indice_carte);

/**
 * Recevoir l'etat du jeu du serveur
 */
int Robot_RecevoirEtatJeu(Robot *robot, EtatJeu *etat);

/**
 * Retirer une carte de la main du robot
 */
void Robot_RetirerCarte(Robot *robot, int indice);

/**
 * Afficher la main du robot (debug)
 */
void Robot_AfficherMain(Robot *robot);

/**
 * Afficher l'etat du jeu (debug)
 */
void Robot_AfficherEtat(EtatJeu *etat);

/**
 * Liberer les ressources du robot
 */
void Robot_Destroy(Robot *robot);

#endif
