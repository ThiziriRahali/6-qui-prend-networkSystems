// server_communication.c : gestion des communications serveur <-> clients

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdarg.h>
#include "protocol.h"
#include "Joueur.h"
#include "jeu.h"

// Variables externes du serveur (definis dans Serveur.c)
extern int nb_clients;
extern int MAX_JOUEURS;
typedef struct {
    int sock;
    struct sockaddr_in addr;
    char nom[32];
    int is_bot;
} client_t;
extern client_t *clients_connectes[10];

/**
 * Envoyer un message a tous les clients (sauf les bots)
 */
int ServerComm_BroadcastMessage(const char *format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    strcat(buffer, "\n");

    int sent_count = 0;
    for (int i = 0; i < nb_clients; i++) {
        if (!clients_connectes[i]->is_bot && clients_connectes[i]->sock != -1) {
            ssize_t sent = send(clients_connectes[i]->sock, buffer, strlen(buffer), 0);
            if (sent != -1) {
                sent_count++;
            }
        }
    }

    return sent_count;
}

/**
 * Envoyer un message a un client specifique
 */
int ServerComm_SendToClient(int client_index, const char *format, ...) {
    if (client_index < 0 || client_index >= nb_clients) return -1;
    if (clients_connectes[client_index]->is_bot) return -1;
    if (clients_connectes[client_index]->sock == -1) return -1;

    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    strcat(buffer, "\n");

    ssize_t sent = send(clients_connectes[client_index]->sock, buffer, strlen(buffer), 0);
    return (sent != -1) ? 0 : -1;
}

/**
 * Annoncer le debut de la partie
 */
void ServerComm_AnnonceGameStart(int nb_joueurs) {
    ServerComm_BroadcastMessage("=== DEBUT DE LA PARTIE ===");
    ServerComm_BroadcastMessage("Nombre de joueurs: %d", nb_joueurs);
    ServerComm_BroadcastMessage("");
}

/**
 * Envoyer l'etat du plateau a tous les joueurs
 */
void ServerComm_SendBoardState(Jeu *jeu) {
    if (!jeu) return;

    ServerComm_BroadcastMessage("--- Etat du plateau ---");

    // Afficher les 4 rangees
    for (int i = 0; i < 4; i++) {
        char buffer[512] = "Rangee ";
        char num[2];
        snprintf(num, sizeof(num), "%d: ", i + 1);
        strcat(buffer, num);

        // Ajouter les cartes de la rangee
        // rangees[i] est une struct Rangee (pas un pointeur)
        for (int j = 0; j < jeu->table.rangees[i].nb_cartes; j++) {
            char carte_str[10];
            snprintf(carte_str, sizeof(carte_str), "%d ", 
                     jeu->table.rangees[i].cartes[j].valeurNum);
            strcat(buffer, carte_str);
        }
        strcat(buffer, "\n");
        ServerComm_BroadcastMessage(buffer);
    }
}

/**
 * Envoyer les scores a tous les joueurs
 */
void ServerComm_SendScores(Joueur *joueurs, int nb_joueurs) {
    if (!joueurs) return;

    ServerComm_BroadcastMessage("--- Scores ---");
    for (int i = 0; i < nb_joueurs; i++) {
        ServerComm_BroadcastMessage("%s: %d points", joueurs[i].nom, joueurs[i].score);
    }
    ServerComm_BroadcastMessage("");
}

/**
 * Annoncer le tour d'un joueur
 */
void ServerComm_AnnonceTurn(const char *joueur_nom, int num_tour) {
    ServerComm_BroadcastMessage("[Tour %d] %s joue...", num_tour, joueur_nom);
}

/**
 * Annoncer une carte jouee
 */
void ServerComm_AnnonceCardPlayed(const char *joueur_nom, int carte_valeur, int rangee) {
    ServerComm_BroadcastMessage("%s joue la carte %d -> Rangee %d", 
                                joueur_nom, carte_valeur, rangee);
}

/**
 * Annoncer la fin d'une manche
 */
void ServerComm_AnnounceRoundEnd(int num_manche, Joueur *joueurs, int nb_joueurs) {
    if (!joueurs) return;

    ServerComm_BroadcastMessage("\n=== MANCHE %d TERMINEE ===", num_manche);
    ServerComm_BroadcastMessage("Scores actuels:");
    for (int i = 0; i < nb_joueurs; i++) {
        ServerComm_BroadcastMessage("  %s: %d points", joueurs[i].nom, joueurs[i].score);
    }
    ServerComm_BroadcastMessage("");
}

/**
 * Annoncer la fin de la partie
 */
void ServerComm_AnnounceGameEnd(const char *gagnant_nom, int gagnant_score, 
                                 Joueur *joueurs, int nb_joueurs) {
    if (!joueurs) return;

    ServerComm_BroadcastMessage("\n=== FIN DE LA PARTIE ===");
    ServerComm_BroadcastMessage("Gagnant: %s avec %d points!", gagnant_nom, gagnant_score);
    ServerComm_BroadcastMessage("");
    ServerComm_BroadcastMessage("Classement final:");

    // Tri simple des joueurs par score (optionnel)
    for (int i = 0; i < nb_joueurs; i++) {
        ServerComm_BroadcastMessage("  %d. %s: %d points", i + 1, joueurs[i].nom, joueurs[i].score);
    }
    ServerComm_BroadcastMessage("");
}

/**
 * Demander une carte a un joueur (pour le robot/client)
 */
int ServerComm_RequestCard(int client_index) {
    if (client_index < 0 || client_index >= nb_clients) return -1;

    // Pour l'instant, juste informer qu'on attend une carte
    ServerComm_SendToClient(client_index, "[Attente de votre carte...]");
    return 0;
}
