#include "global.h"
#include "defines.h"

typedef struct {
    int valeurNum;
    int teteBoeuf;
} Carte;

typedef struct {
    Carte *cartes;
    int nbCartes;
    int maxCartes;
    
} Collection;

typedef struct {
    char ip[MAX_CHARS];
    char nom[MAX_CHARS];
    Collection jeuCartes;
    int score;
    int socket;
    int is_bot;
    int id;
} Joueur;

typedef struct {
    char ip[MAX_SIZE];
    int port;
    Joueur listeJoueurs[10];
    Collection *carteNonDistribuees;
    Collection *carteDejaDistribuees;
    Collection colonnes[4];
} GestionnaireJeu;

typedef struct {
    Carte cartes[NB_CARTES_MAX_RANGEE];  // Max 6 cartes par rangée
    int nbCartes;                        // Nombre actuel de cartes
} Rangee;

typedef struct {
    Rangee rangees[NB_RANGEES_JEU];      // 4 rangées
    int tour;                             // Tour actuel (1-10)
} TableJeu;

typedef struct {
    Joueur *joueurs;                     // Tableau des joueurs
    int nbJoueurs;                       // Nombre de joueurs
    TableJeu table;                      // L'état du tableau
    Collection deck;                     // Deck de cartes restantes
    int tourActuel;                      // Tour courant (1-10)
} Jeu;

typedef struct {
    FILE *file;
    pthread_mutex_t lock;
} Logger;

typedef struct {
    int sock;
    struct sockaddr_in addr;
    char nom[MAX_NAME_LEN];
    int is_bot;
} client_t;
