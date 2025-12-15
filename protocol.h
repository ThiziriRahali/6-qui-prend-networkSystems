#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

/**
 * Protocole de communication serveur <-> clients/robots
 * Messages texte simples, un par ligne, termine par \n
 */

/* Messages du serveur -> clients */
#define MSG_GAME_START "GAME_START:%d" /* Nombre de joueurs */
#define MSG_PLAYER_HAND "HAND:%s" /* Cartes du joueur (separees par virgules) */
#define MSG_YOUR_TURN "YOUR_TURN" /* C'est ton tour, envoie une carte */
#define MSG_BOARD_STATE "BOARD:%s" /* Etat du plateau (format a definir) */
#define MSG_SCORES "SCORES:%s" /* Scores (joueur1:score1,joueur2:score2...) */
#define MSG_GAME_END "GAME_END:%s" /* Gagnant */
#define MSG_ROUND_END "ROUND_END" /* Fin de manche */

/* Messages des clients -> serveur */
#define MSG_PLAY_CARD "PLAY:%d" /* Jouer la carte numero N */

/**
 * Envoyer un message a un client
 */
static inline int Protocol_SendMessage(int sock, const char *format, ...) {
    if (sock == -1) return -1;

    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    strcat(buffer, "\n");

    ssize_t sent = send(sock, buffer, strlen(buffer), 0);
    if (sent == -1) {
        perror("send");
        return -1;
    }

    return 0;
}

/**
 * Recevoir un message d'un client (bloquant)
 */
static inline int Protocol_RecvMessage(int sock, char *buffer, int size) {
    if (sock == -1 || !buffer || size <= 0) return -1;

    ssize_t n = recv(sock, buffer, size - 1, 0);
    if (n <= 0) return -1;

    buffer[n] = '\0';

    /* Supprimer le \n final */
    if (buffer[n - 1] == '\n') {
        buffer[n - 1] = '\0';
    }

    return 0;
}

#endif
