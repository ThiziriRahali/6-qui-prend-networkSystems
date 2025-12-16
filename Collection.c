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

char* Collection_toString(Collection *p, int afficher_indices) {
    if (p == NULL || Collection_getNbCartes(p) == 0) {
        char *buffer = malloc(50);
        strcpy(buffer, "Collection vide");
        return buffer;
    }
    
    size_t buffer_size = Collection_getNbCartes(p) * 600;
    char *buffer = malloc(buffer_size);
    memset(buffer, 0, buffer_size);
    
    if (buffer == NULL) {
        fprintf(stderr, "Erreur allocation\n");
        exit(EXIT_FAILURE);
    }
    
    char **cartes_str = malloc(Collection_getNbCartes(p) * sizeof(char*));
    for (int i = 0; i < Collection_getNbCartes(p); i++) {
        cartes_str[i] = Carte_toString(&(p->cartes[i]));
    }
    
    for (int ligne = 0; ligne < 7; ligne++) {
        for (int i = 0; i < Collection_getNbCartes(p); i++) {
            char *carte_str = cartes_str[i];
            
            int current_line = 0;
            char *start = carte_str;
            char *end = start;
            
            while (current_line < ligne && end != NULL && *end != '\0') {
                if (*end == '\n') current_line++;
                end++;
            }
            
            if (end != NULL && *end != '\0') {
                char *line_end = end;
                while (*line_end != '\n' && *line_end != '\0') line_end++;
                int len = line_end - end;
                strncat(buffer, end, len);
            }
            
            if (i < Collection_getNbCartes(p) - 1) {
                strcat(buffer, " ");
            }
        }
        
        strcat(buffer, "\n");
    }
    
    if (afficher_indices) {
        for (int i = 0; i < Collection_getNbCartes(p); i++) {
            char idx[20];
            snprintf(idx, sizeof(idx), "[%d]", i + 1);
            
            int padding = (12 - (int)strlen(idx)) / 2;
            for (int j = 0; j < padding; j++) strcat(buffer, " ");
            strcat(buffer, idx);
            for (int j = 0; j < 12 - (int)strlen(idx) - padding; j++) strcat(buffer, " ");
            
            if (i < Collection_getNbCartes(p) - 1) {
                strcat(buffer, " ");
            }
        }
        strcat(buffer, "\n");
    }
    
    for (int i = 0; i < Collection_getNbCartes(p); i++) {
        free(cartes_str[i]);
    }
    free(cartes_str);
    
    return buffer;
}

 int Collection_getScore(Collection *p){
    int somme = 0;
    for(int i = 0 ; i < Collection_getNbCartes(p) ; i++){
        somme += Carte_getValeurTete(Collection_getCarte(p, i));
    }
    return somme;
}
