#include "../logs/logging.h"
#include <time.h>

Logger* Logger_Init(void) {
    Logger *logger = malloc(sizeof(Logger));
    if (!logger) return NULL;
    
    logger->file = fopen(LOG_FILE, "a");
    if (!logger->file) {
        free(logger);
        return NULL;
    }
    
    pthread_mutex_init(&logger->lock, NULL);
    return logger;
}

void Logger_Close(Logger *logger) {
    if (!logger) return;
    if (logger->file) fclose(logger->file);
    pthread_mutex_destroy(&logger->lock);
    free(logger);
}

void Logger_JoueurConnecte(Logger *logger, const char *nom, const char *ip, int port) {
    if (!logger || !logger->file) return;
    pthread_mutex_lock(&logger->lock);
    fprintf(logger->file, "[JOUEUR CONNECTE] %s from %s:%d\n", nom, ip, port);
    fflush(logger->file);
    pthread_mutex_unlock(&logger->lock);
}

void Logger_PartieCommencee(Logger *logger, int nbJoueurs, const char **nomsJoueurs) {
    if (!logger || !logger->file) return;
    pthread_mutex_lock(&logger->lock);
    fprintf(logger->file, "[PARTIE COMMENCEE] %d joueurs:\n", nbJoueurs);
    for (int i = 0; i < nbJoueurs; i++) {
        fprintf(logger->file, "  - %s\n", nomsJoueurs[i]);
    }
    fflush(logger->file);
    pthread_mutex_unlock(&logger->lock);
}

void Logger_CartePlacee(Logger *logger, int numTour, const char *nomJoueur, int numeroCarte, int rangee, int scoreAcquis) {
    if (!logger || !logger->file) return;
    pthread_mutex_lock(&logger->lock);
    fprintf(logger->file, "[TOUR %d] %s joue %d sur rangee %d (score: %d)\n", numTour, nomJoueur, numeroCarte, rangee, scoreAcquis);
    fflush(logger->file);
    pthread_mutex_unlock(&logger->lock);
}

void Logger_MancheTerminee(Logger *logger, int numManche, int *scores, int nbJoueurs) {
    if (!logger || !logger->file) return;
    pthread_mutex_lock(&logger->lock);
    fprintf(logger->file, "[MANCHE %d TERMINEE] Scores: ", numManche);
    for (int i = 0; i < nbJoueurs; i++) {
        fprintf(logger->file, "%d ", scores[i]);
    }
    fprintf(logger->file, "\n");
    fflush(logger->file);
    pthread_mutex_unlock(&logger->lock);
}

void Logger_PartieTerminee(Logger *logger, const char *gagnant, int scoreGagnant, int *scores, int nbJoueurs, const char **nomsJoueurs) {
    if (!logger || !logger->file) return;
    pthread_mutex_lock(&logger->lock);
    fprintf(logger->file, "[PARTIE TERMINEE] Gagnant: %s (%d pts)\n", gagnant, scoreGagnant);
    for (int i = 0; i < nbJoueurs; i++) {
        fprintf(logger->file, "  - %s: %d\n", nomsJoueurs[i], scores[i]);
    }
    fflush(logger->file);
    pthread_mutex_unlock(&logger->lock);
}
