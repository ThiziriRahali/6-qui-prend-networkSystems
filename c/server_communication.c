#include "../headers/server_communication.h"

int ServerComm_BroadcastMessage(const char *format, ...) {
    return 0;
}

int ServerComm_SendToClient(int client_index, const char *format, ...) {
    return 0;
}

void ServerComm_AnnonceGameStart(int nb_joueurs) {
    printf("[SERVEUR] Partie lancee avec %d joueurs\n", nb_joueurs);
}

void ServerComm_SendBoardState(Jeu *jeu) {
}

void ServerComm_SendScores(Joueur *joueurs, int nb_joueurs) {
}

void ServerComm_AnnonceTurn(const char *joueur_nom, int num_tour) {
}

void ServerComm_AnnounceGameEnd(const char *gagnant_nom, int gagnant_score, Joueur *joueurs, int nb_joueurs) {
}

void ServerComm_DisconnectAllClients(const char *reason) {
}
