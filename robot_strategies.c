// robot_strategies.c : strategies de jeu pour le robot

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>  // FIX: Pour INT_MAX
#include <unistd.h>
#include <sys/socket.h>
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
    int valeur_min = robot->main[0].valeurNum;

    for (int i = 1; i < robot->nb_cartes; i++) {
        if (robot->main[i].valeurNum < valeur_min) {
            valeur_min = robot->main[i].valeurNum;
            indice_min = i;
        }
    }

    printf("[ROBOT] Strategie PLUS PETITE: Jouer carte #%d (valeur %d)\n", 
           indice_min, valeur_min);
    return indice_min;
}

/**
 * Strategie avancee : analyser les rangees et minimiser les pertes
 */
int Robot_Strategie_Intelligente(Robot *robot, EtatJeu *etat) {
    if (!robot || !etat || robot->nb_cartes <= 0) return -1;

    int meilleur_indice = 0;
    int meilleur_cout = INT_MAX;  // FIX: INT_MAX maintenant défini

    // Pour chaque carte dans la main
    for (int i = 0; i < robot->nb_cartes; i++) {
        int valeur_carte = robot->main[i].valeurNum;
        int cout = 0;
        int rangee_cible = -1;
        int max_derniere_valeur = -1;

        // Trouver la meilleure rangee pour cette carte
        for (int r = 0; r < 4; r++) {
            int nb_cartes_rangee = etat->nb_cartes_rangee[r];

            if (nb_cartes_rangee == 0) continue;

            // FIX: Acces correct a la derniere carte
            Carte derniere_carte = etat->rangees[r][nb_cartes_rangee - 1];
            int derniere_valeur = derniere_carte.valeurNum;

            if (valeur_carte > derniere_valeur) {
                if (derniere_valeur > max_derniere_valeur) {
                    max_derniere_valeur = derniere_valeur;
                    rangee_cible = r;

                    // Si rangee pleine (5 cartes), on devra la prendre
                    if (nb_cartes_rangee >= 5) {
                        cout = 0;
                        for (int c = 0; c < nb_cartes_rangee; c++) {
                            cout += etat->rangees[r][c].teteBoeuf;
                        }
                    } else {
                        cout = 0;
                    }
                }
            }
        }

        // Si aucune rangee ne convient (carte trop petite)
        if (rangee_cible == -1) {
            int min_points = INT_MAX;
            for (int r = 0; r < 4; r++) {
                int points_rangee = 0;
                for (int c = 0; c < etat->nb_cartes_rangee[r]; c++) {
                    points_rangee += etat->rangees[r][c].teteBoeuf;
                }
                if (points_rangee < min_points) {
                    min_points = points_rangee;
                    rangee_cible = r;
                }
            }
            cout = min_points;
        }

        if (cout < meilleur_cout) {
            meilleur_cout = cout;
            meilleur_indice = i;
        }
    }

    printf("[ROBOT] Strategie INTELLIGENTE: Jouer carte #%d (cout estime: %d pts)\n", 
           meilleur_indice, meilleur_cout);
    return meilleur_indice;
}

/**
 * Choisir une carte a jouer
 */
int Robot_ChoisirCarte(Robot *robot) {
    if (!robot || robot->nb_cartes <= 0) return -1;
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
    snprintf(buffer, sizeof(buffer), "%d", carte.valeurNum);

    ssize_t sent = send(robot->socket, buffer, strlen(buffer), 0);
    if (sent == -1) {
        perror("send carte");
        return -1;
    }

    printf("[ROBOT] Carte jouee: #%d (valeur %d)\n", indice_carte, carte.valeurNum);
    Robot_RetirerCarte(robot, indice_carte);
    return 0;
}

/**
 * Recevoir l'etat du jeu du serveur
 */
int Robot_RecevoirEtatJeu(Robot *robot, EtatJeu *etat) {
    if (!robot || !etat) return -1;

    // TODO: Parser le message du serveur et remplir etat
    printf("[ROBOT] Etat du jeu recu\n");
    return 0;
}

/**
 * Retirer une carte de la main du robot
 */
void Robot_RetirerCarte(Robot *robot, int indice) {
    if (!robot || indice < 0 || indice >= robot->nb_cartes) return;

    for (int i = indice; i < robot->nb_cartes - 1; i++) {
        robot->main[i] = robot->main[i + 1];
    }
    robot->nb_cartes--;
}

/**
 * Afficher la main du robot
 */
void Robot_AfficherMain(Robot *robot) {
    if (!robot) return;

    printf("[ROBOT] Main de %s (%d cartes): ", robot->nom, robot->nb_cartes);
    for (int i = 0; i < robot->nb_cartes; i++) {
        printf("%d ", robot->main[i].valeurNum);
    }
    printf("\n");
}

/**
 * Afficher l'etat du jeu
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
