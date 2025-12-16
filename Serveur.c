
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include "Joueur.h"
#include "jeu.h"
#include "logging.h"
#include "server_communication.h"

#define BACKLOG 10
#define MAX_NAME_LEN 32
#define MIN_JOUEURS 2
#define MAX_JOUEURS 10
#define TIMEOUT_TIMER 30

void *client_handler(void *arg);
void *lancer_partie(void *arg);
void *timer_thread(void *arg);

typedef struct {
    int sock;
    struct sockaddr_in addr;
    char nom[MAX_NAME_LEN];
    int is_bot;
} client_t;

client_t *clients_connectes[MAX_JOUEURS];
int nb_clients = 0;
int nb_joueurs_max = 0;
int partie_en_cours = 0;
int timer_active = 0;
time_t timer_start = 0;
pthread_mutex_t mutex_clients = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_min_joueurs = PTHREAD_COND_INITIALIZER;
Logger *g_logger = NULL;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <adresse_ip> <port> <nb_joueurs_max>\n", argv[0]);
        fprintf(stderr, "Exemple: %s 127.0.0.1 4242 4\n", argv[0]);
        fprintf(stderr, "Nombre de joueurs max entre %d et %d\n", MIN_JOUEURS, MAX_JOUEURS);
        return EXIT_FAILURE;
    }

    const char *ip_str = argv[1];
    int port = atoi(argv[2]);
    int max_joueurs = atoi(argv[3]);

    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Port invalide: %d\n", port);
        return EXIT_FAILURE;
    }

    if (max_joueurs < MIN_JOUEURS || max_joueurs > MAX_JOUEURS) {
        fprintf(stderr, "Nombre de joueurs invalide: %d (doit etre entre %d et %d)\n", 
                max_joueurs, MIN_JOUEURS, MAX_JOUEURS);
        return EXIT_FAILURE;
    }

    nb_joueurs_max = max_joueurs;

    g_logger = Logger_Init();
    if (!g_logger) {
        fprintf(stderr, "Impossible d'initialiser le logger\n");
        return EXIT_FAILURE;
    }

    int server_sock;
    struct sockaddr_in server_addr;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("socket");
        Logger_Close(g_logger);
        return EXIT_FAILURE;
    }

    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip_str, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(server_sock);
        Logger_Close(g_logger);
        return EXIT_FAILURE;
    }

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_sock);
        Logger_Close(g_logger);
        return EXIT_FAILURE;
    }

    if (listen(server_sock, BACKLOG) == -1) {
        perror("listen");
        close(server_sock);
        Logger_Close(g_logger);
        return EXIT_FAILURE;
    }

    printf("Serveur lance sur %s:%d\n", ip_str, port);
    printf("Maximum de joueurs: %d\n", nb_joueurs_max);
    printf("En attente de joueurs...\n");
    printf("Une fois le premier joueur connecte, un timer de %d secondes demarre.\n", TIMEOUT_TIMER);
    printf("La partie lancera avec les joueurs connectes + des bots (si necessaire)\n\n");

    while (1) {
        client_t *client = malloc(sizeof(client_t));
        if (!client) {
            perror("malloc");
            continue;
        }

        memset(client, 0, sizeof(client_t));
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
        
        ssize_t n = recv(client->sock, client->nom, MAX_NAME_LEN - 1, 0);
        if (n <= 0) {
            fprintf(stderr, "Erreur lors de la reception du nom\n");
            close(client->sock);
            free(client);
            continue;
        }
        client->nom[n] = '\0';
        if (client->nom[strlen(client->nom) - 1] == '\n') {
            client->nom[strlen(client->nom) - 1] = '\0';
        }
        client->is_bot = 0;
        
        printf("Nouveau joueur connecte: %s depuis %s:%d\n",
               client->nom, ipstr, ntohs(client->addr.sin_port));

        Logger_JoueurConnecte(g_logger, client->nom, ipstr, ntohs(client->addr.sin_port));

        pthread_mutex_lock(&mutex_clients);
        
        if (nb_clients < nb_joueurs_max && !partie_en_cours) {
            clients_connectes[nb_clients] = client;
            nb_clients++;
            
            printf("Joueurs connectes: %d/%d\n", nb_clients, nb_joueurs_max);
            
            if (nb_clients == 1 && !timer_active) {
                timer_active = 1;
                timer_start = time(NULL);
                printf("TIMER LANCE (%d secondes avant lancement auto)\n\n", TIMEOUT_TIMER);
                
                pthread_t tid_timer;
                pthread_create(&tid_timer, NULL, timer_thread, NULL);
                pthread_detach(tid_timer);
            }
            
            if (nb_clients >= nb_joueurs_max && !partie_en_cours) {
                partie_en_cours = 1;
                timer_active = 0;
                printf("\nLANCEMENT DE LA PARTIE (nombre max atteint) avec %d joueurs!\n\n", nb_clients);
                
                const char *noms[MAX_JOUEURS];
                for (int i = 0; i < nb_clients; i++) {
                    noms[i] = clients_connectes[i]->nom;
                }
                Logger_PartieCommencee(g_logger, nb_clients, noms);
                
                pthread_t tid_partie;
                pthread_create(&tid_partie, NULL, lancer_partie, NULL);
                pthread_detach(tid_partie);
            }
        } else {
            char *msg = "Partie deja en cours ou serveur plein\n";
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

void *timer_thread(void *arg) {
    (void)arg;
    
    time_t elapsed = 0;
    int remaining = TIMEOUT_TIMER;
    
    while (timer_active && remaining > 0) {
        sleep(1);
        elapsed = time(NULL) - timer_start;
        remaining = TIMEOUT_TIMER - (int)elapsed;
        
        if (remaining > 0 && remaining % 10 == 0) {
            printf("%d secondes avant lancement auto...\n", remaining);
        }
    }
    
    if (!timer_active) {
        return NULL;
    }
    
    pthread_mutex_lock(&mutex_clients);
    
    if (partie_en_cours) {
        pthread_mutex_unlock(&mutex_clients);
        return NULL;
    }
    
    partie_en_cours = 1;
    timer_active = 0;
    
    int nb_a_ajouter = MIN_JOUEURS - nb_clients;
    printf("\nAjout de %d bot(s)\n", nb_a_ajouter);
    
    for (int i = 0; i < nb_a_ajouter && nb_clients < nb_joueurs_max; i++) {
        client_t *bot = malloc(sizeof(client_t));
        if (!bot) continue;
        
        memset(bot, 0, sizeof(client_t));
        bot->sock = -1;
        bot->is_bot = 1;
        snprintf(bot->nom, MAX_NAME_LEN, "Bot%d", i + 1);
        
        clients_connectes[nb_clients] = bot;
        nb_clients++;
        
        printf("  Bot %d connecte: %s\n", i + 1, bot->nom);
        Logger_JoueurConnecte(g_logger, bot->nom, "127.0.0.1", 0);
    }
    
    printf("\nLANCEMENT DE LA PARTIE (timer ecoule) avec %d joueurs!\n\n", nb_clients);
    
    const char *noms[MAX_JOUEURS];
    for (int i = 0; i < nb_clients; i++) {
        noms[i] = clients_connectes[i]->nom;
    }
    Logger_PartieCommencee(g_logger, nb_clients, noms);
    
    pthread_t tid_partie;
    pthread_create(&tid_partie, NULL, lancer_partie, NULL);
    pthread_detach(tid_partie);
    
    pthread_mutex_unlock(&mutex_clients);
    
    return NULL;
}

void *lancer_partie(void *arg) {
    (void)arg;
    
    pthread_mutex_lock(&mutex_clients);
    
    Joueur *joueurs = malloc(nb_clients * sizeof(Joueur));
    for (int i = 0; i < nb_clients; i++) {
        Joueur_Init(&joueurs[i], 0, clients_connectes[i]->nom);
        joueurs[i].socket = clients_connectes[i]->sock;
        joueurs[i].is_bot = clients_connectes[i]->is_bot;
        if (!clients_connectes[i]->is_bot) {
            snprintf(joueurs[i].ip, MAX_CHARS, "%d", clients_connectes[i]->sock);
        } else {
            snprintf(joueurs[i].ip, MAX_CHARS, "BOT");
        }
    }
    
    Jeu jeu;
    Jeu_Init(&jeu, joueurs, nb_clients);
    
    ServerComm_AnnonceGameStart(nb_clients);
    for (int i = 0; i < nb_clients; i++) {
        if (!clients_connectes[i]->is_bot) {
            char msg[256];
            snprintf(msg, sizeof(msg), "La partie commence avec %d joueurs!\n", nb_clients);
            send(clients_connectes[i]->sock, msg, strlen(msg), 0);
        }
    }
    
    pthread_mutex_unlock(&mutex_clients);
    
    int tour_count = 0;
    while (!Jeu_estTermine(&jeu)) {
        tour_count++;
        
        
        Jeu_jouerTour(&jeu);
        
        Jeu_afficherTableau(&jeu.table);
        Jeu_afficherScores(&jeu);
        
        ServerComm_SendBoardState(&jeu);
        ServerComm_SendScores(joueurs, nb_clients);
        
        sleep(1);
    }
    
    Joueur *gagnant = Jeu_determinerGagnant(&jeu);
    printf("\nGagnant: %s avec %d points!\n", gagnant->nom, gagnant->score);
    
    ServerComm_AnnounceGameEnd(gagnant->nom, gagnant->score, joueurs, nb_clients);
    
    if (g_logger) {
        const char *noms[MAX_JOUEURS];
        int scores[MAX_JOUEURS];
        for (int i = 0; i < nb_clients; i++) {
            noms[i] = joueurs[i].nom;
            scores[i] = joueurs[i].score;
        }
        Logger_PartieTerminee(g_logger, gagnant->nom, gagnant->score, scores, nb_clients, noms);
    }
    
    pthread_mutex_lock(&mutex_clients);
    for (int i = 0; i < nb_clients; i++) {
        if (!clients_connectes[i]->is_bot) {
            char msg[512];
            snprintf(msg, sizeof(msg), "Partie terminee! Gagnant: %s (%d points)\n", 
                     gagnant->nom, gagnant->score);
            send(clients_connectes[i]->sock, msg, strlen(msg), 0);
            close(clients_connectes[i]->sock);
        }
        free(clients_connectes[i]);
    }
    
    nb_clients = 0;
    partie_en_cours = 0;
    timer_active = 0;
    pthread_mutex_unlock(&mutex_clients);
    
    free(joueurs);
    
    printf("\nPartie terminee. En attente de nouveaux joueurs...\n");
    return NULL;
}
