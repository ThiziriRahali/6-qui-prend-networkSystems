#ifndef SERVEUR6_H
#define SERVEUR6_H


#define MAX_CLIENTS 10
#define TAILLE_BUFFER 2048
#define TAILLE_NOM_JOUEUR 256

int initialiser_serveur(const char *adresse_ip, int port);

int accepter_connexion(int serveur_socket);

char* recevoir_nom_client(int client_socket);

#endif 