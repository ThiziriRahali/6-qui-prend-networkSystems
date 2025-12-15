// Serveur.c : serveur de jeu 6 qui prend

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "Joueur.h"  // ← Doit être AVANT jeu.h pour avoir MAX_CHARS
#include "jeu.h"
#include "logging.h"

#define BACKLOG 10
#define MAX_NAME_LEN 32
#define MIN_JOUEURS 2
#define MAX_JOUEURS 10

// Prototype du gestionnaire de client
void *client_handler(void *arg);
void *lancer_partie(void *arg);

typedef struct {
    int sock;
    struct sockaddr_in addr;
    char nom[MAX_NAME_LEN];
} client_t;

// Variables partagées (protégées par mutex)
client_t *clients_connectes[MAX_JOUEURS];
int nb_clients = 0;
int partie_en_cours = 0;
pthread_mutex_t mutex_clients = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_min_joueurs = PTHREAD_COND_INITIALIZER;
Logger *g_logger = NULL;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <adresse_ip> <port>\n", argv[0]);
        fprintf(stderr, "Exemple: %s 127.0.0.1 4242\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *ip_str = argv[1];
    int port = atoi(argv[2]);

    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Port invalide: %d\n", port);
        return EXIT_FAILURE;
    }

    // Initialiser le logger
    g_logger = Logger_Init();
    if (!g_logger) {
        fprintf(stderr, "Impossible d'initialiser le logger\n");
        return EXIT_FAILURE;
    }

    int server_sock;
    struct sockaddr_in server_addr;

    // 1) Création du socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("socket");
        Logger_Close(g_logger);
        return EXIT_FAILURE;
    }

    // Option pour réutiliser rapidement le port
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2) Remplissage de la structure d'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    // Conversion de l'adresse IP
    if (inet_pton(AF_INET, ip_str, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(server_sock);
        Logger_Close(g_logger);
        return EXIT_FAILURE;
    }

    // 3) Bind
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_sock);
        Logger_Close(g_logger);
        return EXIT_FAILURE;
    }

    // 4) Listen
    if (listen(server_sock, BACKLOG) == -1) {
        perror("listen");
        close(server_sock);
        Logger_Close(g_logger);
        return EXIT_FAILURE;
    }

    printf("🎮 Serveur lancé sur %s:%d\n", ip_str, port);
    printf("En attente de %d joueurs minimum...\n", MIN_JOUEURS);

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
        
        // Recevoir le nom du joueur
        ssize_t n = recv(client->sock, client->nom, MAX_NAME_LEN - 1, 0);
        if (n <= 0) {
            fprintf(stderr, "Erreur lors de la réception du nom\n");
            close(client->sock);
            free(client);
            continue;
        }
        client->nom[n] = '\0';
        // Supprimer le newline si présent
        if (client->nom[strlen(client->nom) - 1] == '\n') {
            client->nom[strlen(client->nom) - 1] = '\0';
        }
        
        printf("✅ Nouveau joueur connecté: %s depuis %s:%d\n",
               client->nom, ipstr, ntohs(client->addr.sin_port));

        // Logger la connexion
        Logger_JoueurConnecte(g_logger, client->nom, ipstr, ntohs(client->addr.sin_port));

        // Ajouter le client à la liste
        pthread_mutex_lock(&mutex_clients);
        
        if (nb_clients < MAX_JOUEURS && !partie_en_cours) {
            clients_connectes[nb_clients] = client;
            nb_clients++;
            
            printf("Joueurs connectés: %d/%d\n", nb_clients, MIN_JOUEURS);
            
            // Si on a assez de joueurs, lancer la partie
            if (nb_clients >= MIN_JOUEURS && !partie_en_cours) {
                partie_en_cours = 1;
                printf("\n🎮 LANCEMENT DE LA PARTIE avec %d joueurs!\n\n", nb_clients);
                
                // Logger le début de la partie
                const char *noms[MAX_JOUEURS];
                for (int i = 0; i < nb_clients; i++) {
                    noms[i] = clients_connectes[i]->nom;
                }
                Logger_PartieCommencee(g_logger, nb_clients, noms);
                
                // Créer un thread pour gérer la partie
                pthread_t tid_partie;
                pthread_create(&tid_partie, NULL, lancer_partie, NULL);
                pthread_detach(tid_partie);
            }
        } else {
            // Partie déjà en cours ou trop de joueurs
            char *msg = "Partie déjà en cours ou serveur plein\n";
            send(client->sock, msg, strlen(msg), 0);
            close(client->sock);
            free(client);
        }
        
        pthread_mutex_unlock(&mutex_clients);
    }

    close(server_sock);
    Logger_Close(g_logger);
    return EXIT_SUCCESS;
}

void *lancer_partie(void *arg) {
    (void)arg;
    
    pthread_mutex_lock(&mutex_clients);
    
    // Créer les joueurs
    Joueur *joueurs = malloc(nb_clients * sizeof(Joueur));
    for (int i = 0; i < nb_clients; i++) {
        Joueur_Init(&joueurs[i], 0, clients_connectes[i]->nom);
        snprintf(joueurs[i].ip, MAX_CHARS, "%d", clients_connectes[i]->sock);
    }
    
    // Initialiser le jeu
    Jeu jeu;
    Jeu_Init(&jeu, joueurs, nb_clients);
    
    // Envoyer message de début à tous les joueurs
    for (int i = 0; i < nb_clients; i++) {
        char msg[256];
        snprintf(msg, sizeof(msg), "La partie commence avec %d joueurs!\n", nb_clients);
        send(clients_connectes[i]->sock, msg, strlen(msg), 0);
    }
    
    pthread_mutex_unlock(&mutex_clients);
    
    // Jouer les tours
    while (!Jeu_estTermine(&jeu)) {
        Jeu_jouerTour(&jeu);
        Jeu_afficherTableau(&jeu.table);
        Jeu_afficherScores(&jeu);
        sleep(2); // Pause entre les tours
    }
    
    // Annoncer le gagnant
    Joueur *gagnant = Jeu_determinerGagnant(&jeu);
    printf("\n🏆 Gagnant: %s avec %d points!\n", gagnant->nom, gagnant->score);
    
    // Logger la fin de la partie
    if (g_logger) {
        const char *noms[MAX_JOUEURS];
        int scores[MAX_JOUEURS];
        for (int i = 0; i < nb_clients; i++) {
            noms[i] = joueurs[i].nom;
            scores[i] = joueurs[i].score;
        }
        Logger_PartieTerminee(g_logger, gagnant->nom, gagnant->score, scores, nb_clients, noms);
    }
    
    // Envoyer résultats aux clients
    pthread_mutex_lock(&mutex_clients);
    for (int i = 0; i < nb_clients; i++) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Partie terminée! Gagnant: %s (%d points)\n", 
                 gagnant->nom, gagnant->score);
        send(clients_connectes[i]->sock, msg, strlen(msg), 0);
        close(clients_connectes[i]->sock);
        free(clients_connectes[i]);
    }
    
    nb_clients = 0;
    partie_en_cours = 0;
    pthread_mutex_unlock(&mutex_clients);
    
    free(joueurs);
    
    printf("\nPartie terminée. En attente de nouveaux joueurs...\n");
    return NULL;
}
