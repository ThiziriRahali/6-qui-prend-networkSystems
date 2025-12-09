#include "GestionnaireClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int initialiser_socket(const char *adresse_ip, int port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Erreur lors de la création du socket");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(adresse_ip);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Erreur de connexion au serveur");
        close(sockfd);
        return -1;
    }
    
    printf("Connecté au serveur %s:%d\n", adresse_ip, port);
    return sockfd;
}

void definir_nom_joueur(Joueur *joueur, int joueur_id, const char *nom)
{
    snprintf(joueur->ip, MAX_CHARS, "%d", joueur_id);  // ✅ Convertir l'int en string
    strncpy(joueur->nom, nom, MAX_CHARS - 1);
    joueur->nom[MAX_CHARS - 1] = '\0';
    printf("Nom du joueur %d défini : %s\n", joueur_id, joueur->nom);
}
void envoyer_nom_joueur(int sockfd, const char *nom_joueur)
{
    if (send(sockfd, nom_joueur, strlen(nom_joueur) + 1, 0) < 0)
    {
        perror("Erreur lors de l'envoi du nom");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Nom du joueur envoyé au serveur : %s\n", nom_joueur);
}