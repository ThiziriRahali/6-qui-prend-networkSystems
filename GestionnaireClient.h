#ifndef GESTIONNAIRE_CLIENT_H
#define GESTIONNAIRE_CLIENT_H
#include "Carte.h"
#include "Joueur.h"  
#include "Collection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define NB_CARTES_MAX 10
#define NB_RANGEES 4
#define MAX_CARTES_RANGEE 5



int initialiser_socket(const char *adresse_ip, int port);

void definir_nom_joueur(Joueur *joueur, int joueur_id, const char *nom);

void envoyer_nom_joueur(int sockfd, const char *nom_joueur);

#endif