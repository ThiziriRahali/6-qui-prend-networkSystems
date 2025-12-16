// server_communication.c : gestion des communications serveur <-> clients

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#include "protocol.h"
#include "Joueur.h"
#include "jeu.h"
#include "Collection.h"


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
 * Helper: convertir une Rangee en Collection
 */
static Collection Rangee_asCollection_comm(Rangee *rangee) {
    Collection c;
    c.cartes = rangee->cartes;
    c.nbCartes = rangee->nbCartes;
    c.maxCartes = NB_CARTES_MAX_RANGEE;
    return c;
}

/**
 * Envoyer l'etat du plateau a tous les joueurs avec Collection_toString
 */
void ServerComm_SendBoardState(Jeu *jeu) {
    if (!jeu) return;

    ServerComm_BroadcastMessage("--- Etat du plateau ---");

    // Afficher les 4 rangees en utilisant Collection_toString
    for (int i = 0; i < 4; i++) {
        Rangee *rangee = &jeu->table.rangees[i];
        int pts = Rangee_getTetesBoeuf(rangee);
        
        char header[128];
        snprintf(header, sizeof(header), "Rangee %d [%d pts]:", i + 1, pts);
        ServerComm_BroadcastMessage("%s", header);
        
        if (rangee->nbCartes > 0) {
            Collection c = Rangee_asCollection_comm(rangee);
            char *rangee_str = Collection_toString(&c, 0);  // 0 = pas d'indices
            if (rangee_str) {
                // Envoyer ligne par ligne pour eviter les depassements de buffer
                char *line = strtok(rangee_str, "\n");
                while (line != NULL) {
                    ServerComm_BroadcastMessage("%s", line);
                    line = strtok(NULL, "\n");
                }
                free(rangee_str);
            }
        } else {
            ServerComm_BroadcastMessage("(vide)");
        }
        ServerComm_BroadcastMessage("");  // Ligne vide entre les rangees
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
 * Fermer tous les clients et expulser tout le monde
 * Appelée quand un joueur se déconnecte pendant la partie
 */
void ServerComm_DisconnectAllClients(const char *reason) {
    if (!reason) {
        reason = "Un joueur s'est deconnecte, la partie est interrompue.";
    }
    
    printf("\n❌ Interruption de la partie...\n");
    printf("   Raison: %s\n", reason);
    printf("   Expulsion de tous les joueurs...\n\n");
    
    // Envoyer le message à tous les clients AVANT de fermer
    ServerComm_BroadcastMessage("=== PARTIE INTERROMPUE ===");
    ServerComm_BroadcastMessage("%s", reason);
    ServerComm_BroadcastMessage("Tous les joueurs vont être déconnectés.");
    
    // Attendre un peu pour que les clients reçoivent le message
    fflush(stdout);
    sleep(1);
    
    // ✅ APRÈS (sans mutex):
    for (int i = 0; i < nb_clients; i++) {
        if (clients_connectes[i] && clients_connectes[i]->sock != -1) {
            printf("   Fermeture socket client %d (%s)\n", i, clients_connectes[i]->nom);
            close(clients_connectes[i]->sock);
            clients_connectes[i]->sock = -1;
        }
    }

    
    printf("✅ Tous les clients ont été expulsés.\n\n");
}
