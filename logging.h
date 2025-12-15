#ifndef LOGGING_H
#define LOGGING_H

#include <time.h>
#include "Joueur.h"
#include "jeu.h"

#define LOG_FILE "jeu.log"
#define MAX_LOG_LINE 1024

/**
 * Structure pour gérer les logs du jeu
 */
typedef struct {
    FILE *file;
    pthread_mutex_t lock;  // Pour sécuriser l'accès concurrent
} Logger;

/* Initialiser le logger */
Logger* Logger_Init(void);

/* Fermer le logger */
void Logger_Close(Logger *logger);

/* Log : Enregistrement d'un joueur */
void Logger_JoueurConnecte(Logger *logger, const char *nom, const char *ip, int port);

/* Log : Démarrage de la partie */
void Logger_PartieCommencee(Logger *logger, int nbJoueurs, const char **nomsJoueurs);

/* Log : Placement d'une carte */
void Logger_CartePlacee(Logger *logger, int numTour, const char *nomJoueur, 
                       int numeroCarte, int rangee, int scoreAcquis);

/* Log : Fin d'une manche */
void Logger_MancheTerminee(Logger *logger, int numManche, int *scores, int nbJoueurs);

/* Log : Fin de partie */
void Logger_PartieTerminee(Logger *logger, const char *gagnant, int scoreGagnant, 
                          int *scores, int nbJoueurs, const char **nomsJoueurs);

#endif
