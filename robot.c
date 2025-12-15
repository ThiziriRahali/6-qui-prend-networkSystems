// robot.c : joueur robot pour le jeu 6 qui prend

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include "robot.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <adresse_ip_serveur> <port_serveur> <nom_robot>\n", argv[0]);
        fprintf(stderr, "Exemple: %s 127.0.0.1 4242 Robot1\n", argv[0]);
        return EXIT_FAILURE;
    }

    srand(time(NULL));  // Initialiser le generateur aleatoire

    const char *server_ip = argv[1];
    int server_port = atoi(argv[2]);
    const char *nom_robot = argv[3];

    if (server_port <= 0 || server_port > 65535) {
        fprintf(stderr, "Port invalide: %d\n", server_port);
        return EXIT_FAILURE;
    }

    if (strlen(nom_robot) == 0 || strlen(nom_robot) > 31) {
        fprintf(stderr, "Nom du robot invalide (1-31 caracteres)\n");
        return EXIT_FAILURE;
    }

    int sock;
    struct sockaddr_in serv_addr;
    char buffer[1024];

    // Creation du socket
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

    printf("[ROBOT] %s connecte au serveur %s:%d\n", nom_robot, server_ip, server_port);

    // Initialiser le robot
    Robot *robot = Robot_Init(nom_robot, sock);
    if (!robot) {
        fprintf(stderr, "Erreur lors de l'initialisation du robot\n");
        close(sock);
        return EXIT_FAILURE;
    }

    // Envoyer le nom du robot
    if (Robot_EnvoyerNom(robot) == -1) {
        fprintf(stderr, "Erreur lors de l'envoi du nom\n");
        Robot_Destroy(robot);
        close(sock);
        return EXIT_FAILURE;
    }

    printf("[ROBOT] En attente de messages du serveur...\n\n");

    // Boucle principale du robot
    int partie_en_cours = 1;
    while (partie_en_cours) {
        // Recevoir les messages du serveur
        ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            printf("[ROBOT] Deconnexion du serveur.\n");
            break;
        }

        buffer[n] = '\0';
        printf("[ROBOT] Message du serveur: %s", buffer);
        fflush(stdout);

        // TODO: Parser le message et determiner les actions
        // - Si c'est "La partie commence..." : initialiser la logique du jeu
        // - Si c'est les cartes initiales : appeler Robot_RecevoirMain
        // - Si c'est le tour du robot : appeler Robot_ChoisirCarte() puis Robot_EnvoyerCarte()
        // - Si c'est la fin de partie : terminer

        // Pour l'instant, juste afficher et continuer
    }

    Robot_Destroy(robot);
    close(sock);
    printf("[ROBOT] Fin de la connexion.\n");
    return EXIT_SUCCESS;
}
