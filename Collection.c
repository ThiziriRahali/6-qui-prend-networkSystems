#include <stdlib.h>
#include <stdio.h>
#include "Collection.h"
#include "global.h"
#include <string.h> 

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

int Collection_getTetesBoeufTotal(Collection *p)
{
    int total = 0;
    for (int i = 0; i < Collection_getNbCartes(p); i++) {
        total += Carte_getValeurTete(Collection_getCarte(p, i));
    }
    return total;
}

int Collectionn_isPleine(Collection *p)
{
    if (Collection_getNbCartes(p) > Collection_getMaxCartes(p))
    {
        send_error("Collection has more cards than its maximum capacity in Collectionn_isPleine");
    }
    return Collection_getNbCartes(p) >= Collection_getMaxCartes(p);
}

char* Collection_toString(Collection *p) {
    if (p == NULL || Collection_getNbCartes(p) == 0) {
        char *buffer = malloc(50);
        strcpy(buffer, "Collection vide\n");
        return buffer;
    }

    size_t buffer_size = Collection_getNbCartes(p) * 300 + 100;
    char *buffer = malloc(buffer_size);
    memset(buffer, 0, buffer_size);  // Initialiser à 0
    
    if (buffer == NULL) {
        fprintf(stderr, "Erreur allocation\n");
        exit(EXIT_FAILURE);
    }

    for (int ligne = 0; ligne < 7; ligne++) {
        
        for (int i = 0; i < Collection_getNbCartes(p); i++) {
            char *carte_str = Carte_toString(Collection_getCarte(p, i));
            
            int current_line = 0;
            char *start = carte_str;
            char *end = start;
            
            while (current_line < ligne && *end != '\0') {
                if (*end == '\n') current_line++;
                end++;
            }
            
            start = end;
            while (*end != '\n' && *end != '\0') {
                end++;
            }
            
            int len = end - start;
            strncat(buffer, start, len);
            strcat(buffer, " ");
            
            free(carte_str);
        }
        
        strcat(buffer, "\n");
    }

    return buffer;
}

int Collection_getScore(Collection *p){
    int somme = 0;
    for(int i = 0 ; i < Collection_getNbCartes(p) ; i++){
        somme += Carte_getValeurTete(Collection_getCarte(p, i));
    }
    return somme;
}