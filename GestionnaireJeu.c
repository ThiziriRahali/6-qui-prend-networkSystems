#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GestionnaireJeu.h"
#include "global.h"


void GestionnaireJeu_Init(GestionnaireJeu *p, char *ip, int port){
    p->port = port;
    strncpy(p->ip, ip, MAX_SIZE - 1);
    p->ip[MAX_SIZE - 1] = '\0';

    Carte *cartes = (Carte*) malloc(sizeof(Carte) * 104);
    if (cartes == NULL) {
        send_error("Allocation mémoire pour cartes impossible");
    }

    for (int i = 0; i < 104; i++) {
        Carte_InitNum(&cartes[i], i + 1);
    }

    Collection *c = (Collection*) malloc(sizeof(Collection));
    if (c == NULL) {
        free(cartes);
        send_error("Allocation mémoire pour collection impossible");
    }

    Collection_Init(c, cartes, 104, 104);
    p->carteNonDistribuees = c;
    p->carteDejaDistribuees = NULL;
}
