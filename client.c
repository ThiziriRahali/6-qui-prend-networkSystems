
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

void *thread_reception(void *arg);
void *thread_envoi(void *arg);

int sock_global = -1;
int partie_terminee = 0;
pthread_mutex_t mutex_termine = PTHREAD_MUTEX_INITIALIZER;

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
        fprintf(stderr, "Nom de joueur invalide (1-31 caract\u00e8res)\n");
        return EXIT_FAILURE;
    }

    int sock;
    struct sockaddr_in serv_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    sock_global = sock;

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        return EXIT_FAILURE;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("connect");
        close(sock);
        return EXIT_FAILURE;
    }

    printf("🎮 Connect\u00e9 au serveur %s:%d\n", server_ip, server_port);
    printf("Envoi du nom de joueur: %s\n", nom_joueur);

    ssize_t sent = send(sock, nom_joueur, strlen(nom_joueur), 0);
    if (sent == -1) {
        perror("send");
        close(sock);
        return EXIT_FAILURE;
    }

    printf("En attente du d\u00e9marrage de la partie...\n\n");

    pthread_t tid_recv, tid_send;
    
    if (pthread_create(&tid_recv, NULL, thread_reception, NULL) != 0) {
        perror("pthread_create recv");
        close(sock);
        return EXIT_FAILURE;
    }
    
    if (pthread_create(&tid_send, NULL, thread_envoi, NULL) != 0) {
        perror("pthread_create send");
        close(sock);
        return EXIT_FAILURE;
    }

    pthread_join(tid_recv, NULL);
    
    pthread_mutex_lock(&mutex_termine);
    partie_terminee = 1;
    pthread_mutex_unlock(&mutex_termine);
    
    pthread_cancel(tid_send);
    
    close(sock);
    printf("\nFin de la connexion.\n");
    return EXIT_SUCCESS;
}

void *thread_reception(void *arg) {
    (void)arg;
    char buffer[4096];
    
    while (1) {
        ssize_t n = recv(sock_global, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            printf("\nD\u00e9connexion du serveur.\n");
            pthread_mutex_lock(&mutex_termine);
            partie_terminee = 1;
            pthread_mutex_unlock(&mutex_termine);
            break;
        }

        buffer[n] = '\0';
        printf("%s", buffer);
        fflush(stdout);
    }
    
    return NULL;
}

void *thread_envoi(void *arg) {
    (void)arg;
    char input[256];
    
    while (1) {
        pthread_mutex_lock(&mutex_termine);
        if (partie_terminee) {
            pthread_mutex_unlock(&mutex_termine);
            break;
        }
        pthread_mutex_unlock(&mutex_termine);
        
        if (fgets(input, sizeof(input), stdin) != NULL) {
            ssize_t sent = send(sock_global, input, strlen(input), 0);
            if (sent == -1) {
                perror("send");
                break;
            }
        }
    }
    
    return NULL;
}
