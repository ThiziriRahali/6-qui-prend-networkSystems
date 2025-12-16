#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define LOG_FILE "logs/jeu.log"
#define MAX_LOG_LINE 1024

typedef struct {
    FILE *file;
    pthread_mutex_t lock;
} Logger;

Logger* Logger_Init(void);
void Logger_Close(Logger *logger);
void Logger_JoueurConnecte(Logger *logger, const char *nom, const char *ip, int port);
void Logger_PartieCommencee(Logger *logger, int nbJoueurs, const char **nomsJoueurs);
void Logger_CartePlacee(Logger *logger, int numTour, const char *nomJoueur, int numeroCarte, int rangee, int scoreAcquis);
void Logger_MancheTerminee(Logger *logger, int numManche, int *scores, int nbJoueurs);
void Logger_PartieTerminee(Logger *logger, const char *gagnant, int scoreGagnant, int *scores, int nbJoueurs, const char **nomsJoueurs);

#endif
