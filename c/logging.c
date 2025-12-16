#include "../headers/global.h"


Logger* Logger_Init(void) {
    Logger *logger = malloc(sizeof(Logger));
    if (!logger) {
        send_error("malloc Logger");
        return NULL;
    }

    logger->file = fopen(LOG_FILE, "a");
    if (!logger->file) {
        send_error("fopen");
        free(logger);
        return NULL;
    }

    pthread_mutex_init(&logger->lock, NULL);

    time_t now = time(NULL);
    fprintf(logger->file, "\n=== NOUVEAU JEU - %s", ctime(&now));
    fflush(logger->file);

    return logger;
}

void Logger_Close(Logger *logger) {
    if (!logger) return;

    pthread_mutex_lock(&logger->lock);
    if (logger->file) {
        fprintf(logger->file, "\n=== FIN DU JEU ===\n");
        fflush(logger->file);
        fclose(logger->file);
    }
    pthread_mutex_unlock(&logger->lock);

    pthread_mutex_destroy(&logger->lock);
    free(logger);
}

static void _write_log(Logger *logger, const char *format, ...) {
    if (!logger || !logger->file) return;

    pthread_mutex_lock(&logger->lock);

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", tm_info);

    fprintf(logger->file, "[%s] ", timestamp);

    va_list args;
    va_start(args, format);
    vfprintf(logger->file, format, args);
    va_end(args);

    fprintf(logger->file, "\n");
    fflush(logger->file);

    pthread_mutex_unlock(&logger->lock);
}

void Logger_JoueurConnecte(Logger *logger, const char *nom, const char *ip, int port) {
    if (!logger) return;
    _write_log(logger, "[CONNEXION] Joueur '%s' depuis %s:%d", nom, ip, port);
}

void Logger_PartieCommencee(Logger *logger, int nbJoueurs, const char **nomsJoueurs) {
    if (!logger) return;

    pthread_mutex_lock(&logger->lock);
    fprintf(logger->file, "\n--- DÉBUT DE PARTIE ---\n");
    fprintf(logger->file, "[PARTIE] Lancement avec %d joueurs: ", nbJoueurs);
    for (int i = 0; i < nbJoueurs; i++) {
        fprintf(logger->file, "%s", nomsJoueurs[i]);
        if (i < nbJoueurs - 1) fprintf(logger->file, ", ");
    }
    fprintf(logger->file, "\n");
    fflush(logger->file);
    pthread_mutex_unlock(&logger->lock);
}

void Logger_CartePlacee(Logger *logger, int numTour, const char *nomJoueur, int numeroCarte, int rangee, int scoreAcquis) {
    if (!logger) return;

    char buffer[MAX_LOG_LINE];
    snprintf(buffer, sizeof(buffer),
             "[TOUR %d] %s joue carte #%d -> Rangée %d | Points acqu: %d",
             numTour, nomJoueur, numeroCarte, rangee, scoreAcquis);
    _write_log(logger, "%s", buffer);
}

void Logger_MancheTerminee(Logger *logger, int numManche, int *scores, int nbJoueurs) {
    if (!logger) return;

    pthread_mutex_lock(&logger->lock);
    fprintf(logger->file, "\n[MANCHE %d TERMINÉE] Scores actuels:\n", numManche);
    for (int i = 0; i < nbJoueurs; i++) {
        fprintf(logger->file, "  Joueur %d: %d points\n", i + 1, scores[i]);
    }
    fprintf(logger->file, "\n");
    fflush(logger->file);
    pthread_mutex_unlock(&logger->lock);
}

void Logger_PartieTerminee(Logger *logger, const char *gagnant, int scoreGagnant,
                          int *scores, int nbJoueurs, const char **nomsJoueurs) {
    if (!logger) return;

    pthread_mutex_lock(&logger->lock);
    fprintf(logger->file, "\n--- FIN DE PARTIE ---\n");
    fprintf(logger->file, "[GAGNANT] %s avec %d points!\n\n", gagnant, scoreGagnant);
    fprintf(logger->file, "Classement final:\n");
    for (int i = 0; i < nbJoueurs; i++) {
        fprintf(logger->file, "  %d. %s: %d points\n", i + 1, nomsJoueurs[i], scores[i]);
    }
    fprintf(logger->file, "\n");
    fflush(logger->file);
    pthread_mutex_unlock(&logger->lock);
}
