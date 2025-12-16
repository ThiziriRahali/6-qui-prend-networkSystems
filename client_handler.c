// client_handler.c : gestion d'un client (un thread par client)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// Même struct que dans server.c
typedef struct {
    int sock;
    struct sockaddr_in addr;
} client_t;

// Fonction de gestion de client
void *client_handler(void *arg) {
    client_t *client = (client_t *)arg;
    char buffer[1024];
    ssize_t n;

    // Exemple de "protocole" très simple :
    // - le client envoie des messages texte
    // - le serveur renvoie une réponse de type "OK: <message>"
    // A toi ensuite de remplacer ça par la logique de ton jeu.

    while (1) {
        n = recv(client->sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            // 0 = déconnexion propre, -1 = erreur
            break;
        }

        buffer[n] = '\0';
        printf("Reçu du client %d : %s\n", client->sock, buffer);

        // Ici tu peux parser le message pour tes commandes de jeu,
        // mettre à jour l'état du serveur, broadcast aux autres, etc.

        // Pour l'instant, on renvoie juste une confirmation.
        char reply[1200];
        snprintf(reply, sizeof(reply), "OK: %s", buffer);

        if (send(client->sock, reply, strlen(reply), 0) == -1) {
            perror("send");
            break;
        }
    }

    printf("Client %d déconnecté\n", client->sock);
    close(client->sock);
    free(client);
    return NULL;
}
