#ifndef DEFINES_H
#define DEFINES_H

#define NB_RANGEES_JEU 4
#define NB_CARTES_MAX_RANGEE 6
#define NB_CARTES_PAR_JOUEUR 10
#define NB_TOURS 10
#define DECK_TOTAL 104
#define POINTS_LIMITE 66
#define MAX_SIZE 128
#define MAX_CHARS 256

#define LOG_FILE "jeu.log"
#define MAX_LOG_LINE 1024

#define MSG_GAME_START "GAME_START:%d" /* Nombre de joueurs */
#define MSG_PLAYER_HAND "HAND:%s" /* Cartes du joueur (separees par virgules) */
#define MSG_YOUR_TURN "YOUR_TURN" /* C'est ton tour, envoie une carte */
#define MSG_BOARD_STATE "BOARD:%s" /* Etat du plateau (format a definir) */
#define MSG_SCORES "SCORES:%s" /* Scores (joueur1:score1,joueur2:score2...) */
#define MSG_GAME_END "GAME_END:%s" /* Gagnant */
#define MSG_ROUND_END "ROUND_END" /* Fin de manche */
#define MSG_PLAY_CARD "PLAY:%d" /* Jouer la carte numero N */

#define MAX_CLIENTS 10
#define TAILLE_BUFFER 2048
#define TAILLE_NOM_JOUEUR 256

#define BACKLOG 10
#define MAX_NAME_LEN 32
#define MIN_JOUEURS 2
#define MAX_JOUEURS 10
#define TIMEOUT_TIMER 30

#endif