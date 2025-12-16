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
    Carte cartes[NB_CARTES_MAX_RANGEE];  
    int nbCartes;                        
} Rangee;

typedef struct {
    Rangee rangees[NB_RANGEES_JEU];     
    int tour;                             
} TableJeu;

typedef struct {
    Joueur *joueurs;                     
    int nbJoueurs;                       
    TableJeu table;                      
    Collection deck;                     
    int tourActuel;                      
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
