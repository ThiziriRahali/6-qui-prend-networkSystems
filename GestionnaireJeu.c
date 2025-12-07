#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GestionnaireJeu.h"
#include "global.h"


void GestionnaireJeu_Init(GestionnaireJeu *p, char *ip, int port){
    p->port = port;
    strncpy(p->ip, ip, MAX_CHARS - 1);
    p->ip[MAX_CHARS - 1] = '\0';  /* sécurité */
    Carte cartes[104];
    Collection c;
    for (int i = 0; i < 104; i++) {
        Carte_InitNum(&cartes[i], i);
    }
    Collection_Init(&c, &cartes, 104, 104);
    p->carteNonDistribuees = &c;    
    p->carteDejaDistribuees = NULL;
}
