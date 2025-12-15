#ifndef SERVER_COMMUNICATION_H
#define SERVER_COMMUNICATION_H

#include "Joueur.h"
#include "jeu.h"

/**
 * Envoyer un message a tous les clients (sauf les bots)
 */
int ServerComm_BroadcastMessage(const char *format, ...);

/**
 * Envoyer un message a un client specifique
 */
int ServerComm_SendToClient(int client_index, const char *format, ...);

/**
 * Annoncer le debut de la partie
 */
void ServerComm_AnnonceGameStart(int nb_joueurs);

/**
 * Envoyer l'etat du plateau a tous les joueurs
 */
void ServerComm_SendBoardState(Jeu *jeu);

/**
 * Envoyer les scores a tous les joueurs
 */
void ServerComm_SendScores(Joueur *joueurs, int nb_joueurs);

/**
 * Annoncer le tour d'un joueur
 */
void ServerComm_AnnonceTurn(const char *joueur_nom, int num_tour);

/**
 * Annoncer une carte jouee
 */
void ServerComm_AnnonceCardPlayed(const char *joueur_nom, int carte_valeur, int rangee);

/**
 * Annoncer la fin d'une manche
 */
void ServerComm_AnnounceRoundEnd(int num_manche, Joueur *joueurs, int nb_joueurs);

/**
 * Annoncer la fin de la partie
 */
void ServerComm_AnnounceGameEnd(const char *gagnant_nom, int gagnant_score,
                                 Joueur *joueurs, int nb_joueurs);

/**
 * Demander une carte a un joueur
 */
int ServerComm_RequestCard(int client_index);

#endif
