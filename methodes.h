#include "global.h"

void *thread_reception(void *arg);
void *thread_envoi(void *arg);

void Carte_Init(Carte *p, int num, int tete);
void Carte_InitNum(Carte *p, int num);
int Carte_getValeurNum(Carte *p);
int Carte_getValeurTete(Carte *p);
char* Carte_toString(Carte *p);
char* Carte_toStringCache(Carte *p);

void Collection_Init(Collection *p, Carte *cartes, int nbCartes, int maxCartes);
Carte* Collection_getCartes(Collection *p);
Carte* Collection_getCarte(Collection *p, int index);
int Collection_getNbCartes(Collection *p);
int Collection_getMaxCartes(Collection *p);
void Collection_retirerCarte(Collection *p, int index);
void Collection_ajouterCarte(Collection *p, Carte *carte);
int Collection_getTetesBoeufTotal(Collection *p);
int Collectionn_isPleine(Collection *p);
char* Collection_toString(Collection *p, int afficher_indices);
int Collection_getScore(Collection *p);

void GestionnaireJeu_Init(GestionnaireJeu *p, char *ip, int port);
void GestionnaireJeu_AjouteJoueur(GestionnaireJeu *p, Joueur *j);
int GestionnaireJeu_poserCarte(GestionnaireJeu *p, int colonne, Carte *c);

void send_error(const char* message);
void send_info(const char* message);

void Jeu_Init(Jeu *jeu, Joueur *joueurs, int nbJoueurs);
void Jeu_jouerTour(Jeu *jeu);
int Jeu_trouverMeilleureRangee(TableJeu *table, Carte carte);
void Jeu_prendreRangee(Joueur *joueur, Rangee *rangee);
int Jeu_calculerPointsRangee(Rangee *rangee);
int Jeu_estTermine(Jeu *jeu);
void Jeu_afficherTableau(TableJeu *table);
void Jeu_afficherScores(Jeu *jeu);
Joueur* Jeu_determinerGagnant(Jeu *jeu);

void Rangee_Init(Rangee *rangee);
void Rangee_ajouterCarte(Rangee *rangee, Carte carte);
Carte Rangee_derniereCarte(Rangee *rangee);
int Rangee_getTetesBoeuf(Rangee *rangee);

void Joueur_Init(Joueur *p, int x, char* y);
void Joueur_Method(Joueur *p, int dx, char* dy);
void Joueur_AfficherCartes(Joueur *p);
void Joueur_retirerCarte(Joueur *p, int i);
void Joueur_updateScore(Joueur *p, Collection *c);

Logger* Logger_Init(void);

void Logger_Close(Logger *logger);
void Logger_JoueurConnecte(Logger *logger, const char *nom, const char *ip, int port);
void Logger_PartieCommencee(Logger *logger, int nbJoueurs, const char **nomsJoueurs);
void Logger_CartePlacee(Logger *logger, int numTour, const char *nomJoueur, int numeroCarte, int rangee, int scoreAcquis);
void Logger_MancheTerminee(Logger *logger, int numManche, int *scores, int nbJoueurs);
void Logger_PartieTerminee(Logger *logger, const char *gagnant, int scoreGagnant, int *scores, int nbJoueurs, const char **nomsJoueurs);

int ServerComm_BroadcastMessage(const char *format, ...);
int ServerComm_SendToClient(int client_index, const char *format, ...);
void ServerComm_AnnonceGameStart(int nb_joueurs);
void ServerComm_SendBoardState(Jeu *jeu);
void ServerComm_SendScores(Joueur *joueurs, int nb_joueurs);
void ServerComm_AnnonceTurn(const char *joueur_nom, int num_tour);
void ServerComm_AnnounceGameEnd(const char *gagnant_nom, int gagnant_score, Joueur *joueurs, int nb_joueurs);
void ServerComm_DisconnectAllClients(const char *reason);

int initialiser_serveur(const char *adresse_ip, int port);
int accepter_connexion(int serveur_socket);
char* recevoir_nom_client(int client_socket);

void *client_handler(void *arg);
void *lancer_partie(void *arg);
void *timer_thread(void *arg);