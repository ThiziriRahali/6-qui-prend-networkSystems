#ifndef COLLECTION
#define COLLECTION

#include "Carte.h"

typedef struct {
    Carte *cartes;      // Pointeur vers le tableau de cartes
    int nbCartes;       // Nombre de cartes dans la collection
    int maxCartes;    // Capacité maximale de la collection
    
} Collection;

/* "méthodes" */
void Collection_Init(Collection *p, Carte *cartes, int nbCartes, int maxCartes);
Carte* Collection_getCartes(Collection *p);
Carte* Collection_getCarte(Collection *p, int index);
int Collection_getNbCartes(Collection *p);
int Collection_getMaxCartes(Collection *p);
void Collection_retirerCarte(Collection *p, int index);
void Collection_ajouterCarte(Collection *p, Carte *carte);

int Collection_getTetesBoeufTotal(Collection *p);
int Collectionn_isPleine(Collection *p);

char* Collection_toString(Collection *p);

int Collection_getScore(Collection *p);


#endif
