#include "Carte.h"
#include "Collection.h"
#include "global.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_Carte() {
    printf("\n=== TEST CARTE ===\n");
    
    // Test 1: Carte_Init
    printf("\n[Test 1] Carte_Init avec valeurs valides\n");
    Carte maCarte;
    Carte_Init(&maCarte, 10, 1);
    printf("Valeur Num: %d (attendu: 10)\n", Carte_getValeurNum(&maCarte));
    printf("Tete Boeuf: %d (attendu: 1)\n", Carte_getValeurTete(&maCarte));
    
    // Test 2: Carte_InitNum
    printf("\n[Test 2] Carte_InitNum\n");
    Carte maCarte2;
    Carte_InitNum(&maCarte2, 50);
    printf("Valeur Num: %d (attendu: 50)\n", Carte_getValeurNum(&maCarte2));
    printf("Tete Boeuf: %d (attendu: calculé automatiquement)\n", Carte_getValeurTete(&maCarte2));
    printf("%s\n", Carte_toString(&maCarte2));
    printf("%s\n", Carte_toStringCache(&maCarte2));
    
    // Test 3: Cartes avec différentes valeurs
    printf("\n[Test 3] Cartes avec différentes valeurs\n");
    Carte cartes_test[5];
    int valeurs[] = {1, 5, 10, 11, 55};
    for (int i = 0; i < 5; i++) {
        Carte_InitNum(&cartes_test[i], valeurs[i]);
        //printf("Carte %d: Valeur=%d, Têtes=%d\n", i+1, 
        //       Carte_getValeurNum(&cartes_test[i]), 
        //       Carte_getValeurTete(&cartes_test[i]));
        printf("%s\n", Carte_toString(&cartes_test[i]));
    }
}

void test_Collection() {
    printf("\n=== TEST COLLECTION ===\n");
    
    // Créer un tableau de cartes
    printf("\n[Test 1] Initialisation d'une collection\n");
    Carte cartes[10];
    for (int i = 0; i < 10; i++) {
        Carte_InitNum(&cartes[i], (i + 1) * 10);
    }

    Collection maCollection;
    Collection_Init(&maCollection, cartes, 10, 10);

    printf("%s\n", Collection_toString(&maCollection));
    printf("Score: %d\n", Collection_getScore(&maCollection));
    /*printf("Collection créée avec %d cartes (max: %d)\n", 
           Collection_getNbCartes(&maCollection), 
           Collection_getMaxCartes(&maCollection));
    
    // Test 2: Récupérer toutes les cartes
    printf("\n[Test 2] Affichage de toutes les cartes\n");
    Carte* tous_cartes = Collection_getCartes(&maCollection);
    for (int i = 0; i < Collection_getNbCartes(&maCollection); i++) {
        printf("Carte %d: Valeur=%d\n", i+1, Carte_getValeurNum(&tous_cartes[i]));
    }
    
    // Test 3: Récupérer une carte spécifique
    printf("\n[Test 3] Récupération d'une carte spécifique (index 3)\n");
    Carte* carte_specifique = Collection_getCarte(&maCollection, 3);
    if (carte_specifique != NULL) {
        printf("Carte à l'index 3: Valeur=%d, Têtes=%d\n", 
               Carte_getValeurNum(carte_specifique),
               Carte_getValeurTete(carte_specifique));
    }
    
    // Test 4: Retirer une carte
    printf("\n[Test 4] Retrait de la carte à l'index 2\n");
    Collection_retirerCarte(&maCollection, 2);
    printf("Nombre de cartes après retrait: %d\n", Collection_getNbCartes(&maCollection));
    printf("Affichage des cartes restantes:\n");
    tous_cartes = Collection_getCartes(&maCollection);
    for (int i = 0; i < Collection_getNbCartes(&maCollection); i++) {
        printf("  Carte %d: Valeur=%d\n", i+1, Carte_getValeurNum(&tous_cartes[i]));
    }
    
    // Test 5: Ajouter une carte
    printf("\n[Test 5] Ajout d'une nouvelle carte\n");
    Carte nouvelle_carte;
    Carte_InitNum(&nouvelle_carte, 99);
    Collection_ajouterCarte(&maCollection, &nouvelle_carte);
    printf("Nombre de cartes après ajout: %d\n", Collection_getNbCartes(&maCollection));
    printf("Dernière carte ajoutée: Valeur=%d\n", 
           Carte_getValeurNum(&Collection_getCartes(&maCollection)[Collection_getNbCartes(&maCollection) - 1]));*/
}

int main() {
    printf("============================\n");
    printf("  TESTS DES CLASSES\n");
    printf("============================\n");
    
    test_Carte();
    test_Collection();
    
    printf("\n============================\n");
    printf("  FIN DES TESTS\n");
    printf("============================\n");
    
    return 0;
}