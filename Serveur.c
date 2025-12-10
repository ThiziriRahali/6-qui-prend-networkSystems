// server.c : serveur de jeu très simple (TCP, IPv4)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SERVER_PORT 4242
#define BACKLOG 10       // connexions en attente
#define MAX_NAME_LEN 32

// Prototype du gestionnaire de client
void *client_handler(void *arg);

typedef struct {
    int sock;                    // socket client
    struct sockaddr_in addr;     // adresse client
} client_t;

int main(void) {
    int server_sock;
    struct sockaddr_in server_addr;

    // 1) Création du socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    // Option pour réutiliser rapidement le port
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2) Remplissage de la structure d'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // toutes les interfaces
    server_addr.sin_port = htons(SERVER_PORT);

    // 3) Bind
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_sock);
        return EXIT_FAILURE;
    }

    // 4) Listen
    if (listen(server_sock, BACKLOG) == -1) {
        perror("listen");
        close(server_sock);
        return EXIT_FAILURE;
    }

    printf("Serveur lancé sur le port %d\n", SERVER_PORT);

    // 5) Boucle principale d'acceptation
    while (1) {
        client_t *client = malloc(sizeof(client_t));
        if (!client) {
            perror("malloc");
            continue;
        }

        socklen_t addrlen = sizeof(client->addr);
        client->sock = accept(server_sock,
                              (struct sockaddr *)&client->addr,
                              &addrlen);
        if (client->sock == -1) {
            perror("accept");
            free(client);
            continue;
        }

        char ipstr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client->addr.sin_addr, ipstr, sizeof(ipstr));
        printf("Nouveau client connecté depuis %s:%d\n",
               ipstr, ntohs(client->addr.sin_port));

        // 6) Création d’un thread pour gérer ce client
        pthread_t tid;
        if (pthread_create(&tid, NULL, client_handler, client) != 0) {
            perror("pthread_create");
            close(client->sock);
            free(client);
            continue;
        }

        // Le thread s’auto-détache, pas besoin de join
        pthread_detach(tid);
    }

    close(server_sock);
    return EXIT_SUCCESS;
}
