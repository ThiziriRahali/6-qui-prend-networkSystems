// client.c : client pour le jeu 6 qui prend

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <adresse_ip_serveur> <port_serveur> <nom_joueur>\n", argv[0]);
        fprintf(stderr, "Exemple: %s 127.0.0.1 4242 Alice\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *server_ip = argv[1];
    int server_port = atoi(argv[2]);
    const char *nom_joueur = argv[3];

    if (server_port <= 0 || server_port > 65535) {
        fprintf(stderr, "Port invalide: %d\n", server_port);
        return EXIT_FAILURE;
    }

    if (strlen(nom_joueur) == 0 || strlen(nom_joueur) > 31) {
        fprintf(stderr, "Nom de joueur invalide (1-31 caractères)\n");
        return EXIT_FAILURE;
    }

    int sock;
    struct sockaddr_in serv_addr;
    char buffer[1024];

    // Création du socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);

    // Conversion de l'adresse IP
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        return EXIT_FAILURE;
    }

    // Connexion au serveur
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("connect");
        close(sock);
        return EXIT_FAILURE;
    }

    printf("🎮 Connecté au serveur %s:%d\n", server_ip, server_port);
    printf("Envoi du nom de joueur: %s\n", nom_joueur);

    // Envoyer le nom du joueur
    ssize_t sent = send(sock, nom_joueur, strlen(nom_joueur), 0);
    if (sent == -1) {
        perror("send");
        close(sock);
        return EXIT_FAILURE;
    }

    printf("En attente du démarrage de la partie...\n\n");

    // Boucle de réception des messages du serveur
    while (1) {
        ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            printf("Déconnexion du serveur.\n");
            break;
        }

        buffer[n] = '\0';
        printf("%s", buffer);
        fflush(stdout);
    }

    close(sock);
    printf("\nFin de la connexion.\n");
    return EXIT_SUCCESS;
}
