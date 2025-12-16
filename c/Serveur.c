#include "../headers/global.h"

int nb_joueurs_max = 0;
int timer_active = 0;
time_t timer_start = 0;
pthread_mutex_t mutex_clients = PTHREAD_MUTEX_INITIALIZER;

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
    printf("En attente de joueurs...\n\n");

    close(server_sock);
    Logger_Close(g_logger);
    return EXIT_SUCCESS;
}

void *timer_thread(void *arg) {
    (void)arg;
    return NULL;
}

void *lancer_partie(void *arg) {
    (void)arg;
    return NULL;
}
