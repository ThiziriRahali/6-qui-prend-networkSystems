#include "Serveur.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>


#define TAILLE_BUFFER 2048

int initialiser_serveur(const char *adresse_ip, int port)
{
    int serveur_socket;
    struct sockaddr_in adresse_serveur;

    serveur_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (serveur_socket == -1)
    {
        perror("Erreur création socket");
        exit(EXIT_FAILURE);
    }

    adresse_serveur.sin_family = AF_INET;
    adresse_serveur.sin_port = htons(port);
    adresse_serveur.sin_addr.s_addr = inet_addr(adresse_ip);

    if (bind(serveur_socket, (struct sockaddr *)&adresse_serveur, sizeof(adresse_serveur)) == -1)
    {
        perror("Erreur bind");
        close(serveur_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(serveur_socket, MAX_CLIENTS) == -1)
    {
        perror("Erreur listen");
        close(serveur_socket);
        exit(EXIT_FAILURE);
    }

    return serveur_socket;
}

int accepter_connexion(int serveur_socket)
{
    struct sockaddr_in adresse_client;
    socklen_t taille = sizeof(adresse_client);

    int client_socket = accept(serveur_socket, (struct sockaddr *)&adresse_client, &taille);
    if (client_socket < 0)
    {
        perror("Erreur accept");
        exit(EXIT_FAILURE);
    }

    printf("Connexion acceptée\n");
    return client_socket;
}

char* recevoir_nom_client(int client_socket)
{
    char buffer[TAILLE_NOM_JOUEUR];
    ssize_t recu = recv(client_socket, buffer, TAILLE_NOM_JOUEUR - 1, 0);

    if (recu > 0)
    {
        buffer[recu] = '\0';
        printf("Client connecté: %s\n", buffer);

        char *nom = malloc((recu + 1) * sizeof(char));
        if (nom == NULL)
        {
            perror("Erreur allocation nom");
            exit(EXIT_FAILURE);
        }
        strcpy(nom, buffer);
        return nom;
    }
    else if (recu == 0)
    {
        printf("Client fermé avant envoi du nom\n");
    }
    else
    {
        perror("Erreur réception nom");
    }
    return NULL;
}
