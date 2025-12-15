// robot.c : joueur robot pour le jeu 6 qui prend

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <adresse_ip_serveur> <port_serveur> <nom_robot>\n", argv[0]);
        fprintf(stderr, "Exemple: %s 127.0.0.1 4242 Robot1\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *server_ip = argv[1];
    int server_port = atoi(argv[2]);
    const char *nom_robot = argv[3];

    if (server_port <= 0 || server_port > 65535) {
        fprintf(stderr, "Port invalide: %d\n", server_port);
        return EXIT_FAILURE;
    }

    if (strlen(nom_robot) == 0 || strlen(nom_robot) > 31) {
        fprintf(stderr, "Nom du robot invalide (1-31 caract\u00e8res)\n");
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

    printf("\ud83e\udd16 [ROBOT] %s connect\u00e9 au serveur %s:%d\n", nom_robot, server_ip, server_port);

    // Envoyer le nom du robot
    ssize_t sent = send(sock, nom_robot, strlen(nom_robot), 0);
    if (sent == -1) {
        perror("send");
        close(sock);
        return EXIT_FAILURE;
    }

    printf("\ud83e\udd16 [ROBOT] Nom envoy\u00e9: %s\n", nom_robot);
    printf("\ud83e\udd16 [ROBOT] En attente de messages du serveur...\n\n");

    // Boucle de r\u00e9ception des messages du serveur
    while (1) {
        ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            printf("\ud83e\udd16 [ROBOT] D\u00e9connexion du serveur.\n");
            break;
        }

        buffer[n] = '\0';
        printf("\ud83e\udd16 [ROBOT] Message re\u00e7u: %s", buffer);
        fflush(stdout);

        // TODO: Ici on ajoutera la logique du robot
        // - Parser le message
        // - D\u00e9terminer quelle carte jouer
        // - Envoyer la d\u00e9cision au serveur
    }

    close(sock);
    printf("\ud83e\udd16 [ROBOT] Fin de la connexion.\n");
    return EXIT_SUCCESS;
}
