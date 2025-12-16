#include "../headers/global.h"

Logger *g_logger = NULL;
client_t *clients_connectes[MAX_JOUEURS];
int nb_clients = 0;
int partie_en_cours = 0;

void send_error(const char* message) {
    if (g_logger) {
        fprintf(g_logger->file, "[ERROR] %s\n", message);
        fflush(g_logger->file);
    }
    fprintf(stderr, "[ERROR] %s\n", message);
}

void send_info(const char* message) {
    if (g_logger) {
        fprintf(g_logger->file, "[INFO] %s\n", message);
        fflush(g_logger->file);
    }
    fprintf(stdout, "[INFO] %s\n", message);
}
