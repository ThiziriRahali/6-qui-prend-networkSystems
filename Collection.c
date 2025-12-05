#include <stdlib.h>
#include <stdio.h>
#include "Collection.h"
#include "global.h"


void Collection_Init(Collection *p, Carte *cartes, int nbCartes, int maxCartes) {
    p->cartes = cartes;
    p->nbCartes = nbCartes;
    p->maxCartes = maxCartes;
}

Carte* Collection_getCartes(Collection *p) {
    return p->cartes;
}

Carte* Collection_getCarte(Collection *p, int index) {
    if (index < 0 || index >= Collection_getNbCartes(p)) {
        send_error("Index out of bounds in Collection_getCarte");
        return NULL;
    } else{
        return &(p->cartes[index]);
    }
}

int Collection_getNbCartes(Collection *p) {
    return p->nbCartes;
}

int Collection_getMaxCartes(Collection *p) {
    return p->maxCartes;
}

void Collection_retirerCarte(Collection *p, int index) {
    if (index < 0 || index >= Collection_getNbCartes(p)) {
        send_error("Index out of bounds in Collection_retirerCarte");
        return;
    }
    for (int i = index; i < p->nbCartes - 1; i++) {
        p->cartes[i] = p->cartes[i + 1];
    }
    p->nbCartes--;
}

void Collection_ajouterCarte(Collection *p, Carte *carte) {
    if (Collection_getNbCartes(p) >= Collection_getMaxCartes(p)) {
        send_error("Collection is full in Collection_ajouterCarte");
        return;
    }
    p->cartes[p->nbCartes] = *carte;
    p->nbCartes++;
}