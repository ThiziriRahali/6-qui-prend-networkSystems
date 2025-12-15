// robot_strategies.c : strategies de jeu pour le robot

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "robot.h"

/**
 * Initialiser le robot
 */
Robot* Robot_Init(const char *nom, int socket) {
    Robot *robot = malloc(sizeof(Robot));
    if (!robot) {
        perror("malloc Robot");
        return NULL;
    }

    robot->socket = socket;
    strncpy(robot->nom, nom, MAX_NAME_LEN - 1);
    robot->nom[MAX_NAME_LEN - 1] = '\0';
    robot->nb_cartes = 0;
    robot->score = 0;
    memset(robot->main, 0, sizeof(robot->main));

    return robot;
}

/**
 * Envoyer le nom du robot au serveur
 */
int Robot_EnvoyerNom(Robot *robot) {
    if (!robot || robot->socket == -1) return -1;

    ssize_t sent = send(robot->socket, robot->nom, strlen(robot->nom), 0);
    if (sent == -1) {
        perror("send nom");
        return -1;
    }

    printf("[ROBOT] Nom envoye: %s\n", robot->nom);
    return 0;
}

/**
 * Recevoir les cartes initiales du serveur
 * Format simple: les cartes sont separees par des espaces ou newline
 */
int Robot_RecevoirMain(Robot *robot, Carte *cartes, int nb_cartes) {
    if (!robot || !cartes || nb_cartes <= 0) return -1;

    robot->nb_cartes = nb_cartes;
    memcpy(robot->main, cartes, nb_cartes * sizeof(Carte));

    printf("[ROBOT] Main recue: %d cartes\n", nb_cartes);
    Robot_AfficherMain(robot);

    return 0;
}

/**
 * Strategie simple : jouer une carte aleatoire
 */
int Robot_Strategie_Aleatoire(Robot *robot) {
    if (!robot || robot->nb_cartes <= 0) return -1;

    int indice = rand() % robot->nb_cartes;
    printf("[ROBOT] Strategie ALEATOIRE: Carte choisie a l'indice %d\n", indice);
    return indice;
}

/**
 * Strategie 2 : jouer la plus petite carte
 */
int Robot_Strategie_PlusPetite(Robot *robot) {
    if (!robot || robot->nb_cartes <= 0) return -1;

    int indice_min = 0;
    int valeur_min = robot->main[0].valeur;

    // Trouver la carte avec la plus petite valeur
    for (int i = 1; i < robot->nb_cartes; i++) {
        if (robot->main[i].valeur < valeur_min) {
            valeur_min = robot->main[i].valeur;
            indice_min = i;
        }
    }

    printf("[ROBOT] Strategie PLUS PETITE: Jouer carte #%d (valeur %d)\n", 
           indice_min, valeur_min);
    return indice_min;
}

/**
 * Strategie avancee : analyser les rangees et minimiser les pertes
 * (A IMPLEMENTER PLUS TARD)
 */
int Robot_Strategie_Intelligente(Robot *robot, EtatJeu *etat) {
    if (!robot || !etat || robot->nb_cartes <= 0) return -1;

    // Pour l'instant, on utilise la strategie simple
    printf("[ROBOT] Strategie INTELLIGENTE: Non implementee, utilisation de PlusPetite\n");
    return Robot_Strategie_PlusPetite(robot);
}

/**
 * Choisir une carte a jouer (wrapper - appelle la strategie)
 */
int Robot_ChoisirCarte(Robot *robot) {
    if (!robot || robot->nb_cartes <= 0) return -1;

    // Pour l'instant, utiliser la strategie "plus petite carte"
    return Robot_Strategie_PlusPetite(robot);
}

/**
 * Envoyer la carte choisie au serveur
 */
int Robot_EnvoyerCarte(Robot *robot, int indice_carte) {
    if (!robot || indice_carte < 0 || indice_carte >= robot->nb_cartes) {
        fprintf(stderr, "[ROBOT] Indice de carte invalide\n");
        return -1;
    }

    Carte carte = robot->main[indice_carte];
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%d", carte.valeur);

    ssize_t sent = send(robot->socket, buffer, strlen(buffer), 0);
    if (sent == -1) {
        perror("send carte");
        return -1;
    }

    printf("[ROBOT] Carte jouee: #%d (valeur %d)\n", indice_carte, carte.valeur);
    Robot_RetirerCarte(robot, indice_carte);
    return 0;
}

/**
 * Recevoir l'etat du jeu du serveur
 * (Format a definir avec le serveur - pour l'instant, juste afficher)
 */
int Robot_RecevoirEtatJeu(Robot *robot, EtatJeu *etat) {
    if (!robot || !etat) return -1;

    // TODO: Parser le message du serveur et remplir etat
    // Pour l'instant, juste un placeholder
    printf("[ROBOT] Etat du jeu recu\n");
    return 0;
}

/**
 * Retirer une carte de la main du robot
 */
void Robot_RetirerCarte(Robot *robot, int indice) {
    if (!robot || indice < 0 || indice >= robot->nb_cartes) return;

    // Shift les cartes apres l'indice
    for (int i = indice; i < robot->nb_cartes - 1; i++) {
        robot->main[i] = robot->main[i + 1];
    }
    robot->nb_cartes--;
}

/**
 * Afficher la main du robot (debug)
 */
void Robot_AfficherMain(Robot *robot) {
    if (!robot) return;

    printf("[ROBOT] Main de %s (%d cartes): ", robot->nom, robot->nb_cartes);
    for (int i = 0; i < robot->nb_cartes; i++) {
        printf("%d ", robot->main[i].valeur);
    }
    printf("\n");
}

/**
 * Afficher l'etat du jeu (debug)
 */
void Robot_AfficherEtat(EtatJeu *etat) {
    if (!etat) return;

    printf("[ROBOT] Etat du jeu:\n");
    printf("  Rangees: ");
    for (int i = 0; i < 4; i++) {
        printf("[R%d: %d cartes] ", i + 1, etat->nb_cartes_rangee[i]);
    }
    printf("\n");

    printf("  Scores: ");
    for (int i = 0; i < etat->nb_joueurs; i++) {
        printf("%d ", etat->scores[i]);
    }
    printf("\n");
}

/**
 * Liberer les ressources du robot
 */
void Robot_Destroy(Robot *robot) {
    if (!robot) return;
    free(robot);
}
