#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <netinet/in.h>
#include "GestionnaireJeu.h"

typedef struct {
    int sockfd;
    struct sockaddr_in address;
    int id; // ID du joueur (0 à MAX_JOUEURS-1)
    GestionnaireJeu *gestionnaire; // Pointeur vers l'état global du jeu
} ClientContext;

void *client_handler(void *socket_desc);

#endif
