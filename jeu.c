#include "jeu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>

/* ============ INITIALISATION ============ */

void Rangee_Init(Rangee *rangee) {
    if (rangee == NULL) return;
    rangee->nbCartes = 0;
    memset(rangee->cartes, 0, sizeof(rangee->cartes));
}

void Jeu_Init(Jeu *jeu, Joueur *joueurs, int nbJoueurs) {
    if (jeu == NULL || joueurs == NULL || nbJoueurs <= 0) return;
    
    jeu->joueurs = joueurs;
    jeu->nbJoueurs = nbJoueurs;
    jeu->tourActuel = 0;
    
    // Initialiser les 4 rangées
    for (int i = 0; i < NB_RANGEES_JEU; i++) {
        Rangee_Init(&jeu->table.rangees[i]);
    }
    
    // Créer le deck de 104 cartes
    Carte *deck_cartes = malloc(DECK_TOTAL * sizeof(Carte));
    if (!deck_cartes) {
        fprintf(stderr, "Erreur allocation deck\n");
        return;
    }
    
    // Initialiser le deck avec les cartes 1-104
    for (int i = 0; i < DECK_TOTAL; i++) {
        Carte_InitNum(&deck_cartes[i], i + 1);
    }
    
    // Mélanger le deck (Fisher-Yates)
    srand(time(NULL) + getpid());
    for (int i = DECK_TOTAL - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Carte temp = deck_cartes[i];
        deck_cartes[i] = deck_cartes[j];
        deck_cartes[j] = temp;
    }
    
    // Distribuer 10 cartes à chaque joueur
    int carte_index = 0;
    for (int j = 0; j < nbJoueurs; j++) {
        // Allouer l'espace pour les cartes du joueur
        joueurs[j].jeuCartes.cartes = malloc(NB_CARTES_PAR_JOUEUR * sizeof(Carte));
        if (!joueurs[j].jeuCartes.cartes) {
            fprintf(stderr, "Erreur allocation cartes joueur %d\n", j);
            continue;
        }
        
        joueurs[j].jeuCartes.nbCartes = NB_CARTES_PAR_JOUEUR;
        joueurs[j].jeuCartes.maxCartes = NB_CARTES_PAR_JOUEUR;
        
        for (int c = 0; c < NB_CARTES_PAR_JOUEUR; c++) {
            joueurs[j].jeuCartes.cartes[c] = deck_cartes[carte_index++];
        }
        
        // Trier la main du joueur par valeur croissante
        for (int a = 0; a < joueurs[j].jeuCartes.nbCartes - 1; a++) {
            for (int b = a + 1; b < joueurs[j].jeuCartes.nbCartes; b++) {
                if (joueurs[j].jeuCartes.cartes[a].valeurNum > joueurs[j].jeuCartes.cartes[b].valeurNum) {
                    Carte temp = joueurs[j].jeuCartes.cartes[a];
                    joueurs[j].jeuCartes.cartes[a] = joueurs[j].jeuCartes.cartes[b];
                    joueurs[j].jeuCartes.cartes[b] = temp;
                }
            }
        }
    }
    
    // Placer 4 cartes initiales sur les rangées
    for (int i = 0; i < NB_RANGEES_JEU; i++) {
        Rangee_ajouterCarte(&jeu->table.rangees[i], deck_cartes[carte_index++]);
    }
    
    // Sauvegarder le reste du deck
    jeu->deck.cartes = deck_cartes;
    jeu->deck.nbCartes = DECK_TOTAL;
    jeu->deck.maxCartes = DECK_TOTAL;
    
    printf("\n🎮 Jeu initialisé avec %d joueurs\n", nbJoueurs);
    printf("📊 Cartes distribuées: %d cartes par joueur\n", NB_CARTES_PAR_JOUEUR);
    printf("🃏 Rangées initiales placées\n\n");
}

/* ============ RANGÉES ============ */

void Rangee_ajouterCarte(Rangee *rangee, Carte carte) {
    if (rangee == NULL || rangee->nbCartes >= NB_CARTES_MAX_RANGEE) return;
    
    rangee->cartes[rangee->nbCartes] = carte;
    rangee->nbCartes++;
}

Carte Rangee_derniereCarte(Rangee *rangee) {
    if (rangee == NULL || rangee->nbCartes == 0) {
        Carte carte_nulle;
        Carte_Init(&carte_nulle, 0, 0);
        return carte_nulle;
    }
    return rangee->cartes[rangee->nbCartes - 1];
}

int Rangee_getTetesBoeuf(Rangee *rangee) {
    if (rangee == NULL) return 0;
    
    int total = 0;
    for (int i = 0; i < rangee->nbCartes; i++) {
        total += rangee->cartes[i].teteBoeuf;
    }
    return total;
}

/* ============ PLACEMENT DE CARTES ============ */

int Jeu_trouverMeilleureRangee(TableJeu *table, Carte carte) {
    if (table == NULL) return -1;
    
    int meilleure_rangee = -1;
    int max_valeur = -1;
    
    // Chercher la rangée avec la plus grande dernière carte < carte jouée
    for (int i = 0; i < NB_RANGEES_JEU; i++) {
        Rangee *rangee = &table->rangees[i];
        
        if (rangee->nbCartes == 0) continue;
        
        Carte derniere = Rangee_derniereCarte(rangee);
        
        // La carte doit être plus grande que la dernière carte de la rangée
        if (carte.valeurNum > derniere.valeurNum) {
            // Prendre la rangée avec la plus grande valeur finale < carte
            if (derniere.valeurNum > max_valeur) {
                max_valeur = derniere.valeurNum;
                meilleure_rangee = i;
            }
        }
    }
    
    // Si aucune rangée ne convient (carte trop petite)
    if (meilleure_rangee == -1) {
        // Prendre la rangée avec le moins de têtes de boeuf
        int min_tetes = INT_MAX;
        for (int i = 0; i < NB_RANGEES_JEU; i++) {
            int tetes = Rangee_getTetesBoeuf(&table->rangees[i]);
            if (tetes < min_tetes) {
                min_tetes = tetes;
                meilleure_rangee = i;
            }
        }
    }
    
    return meilleure_rangee;
}

/* ============ PRISE DE RANGÉE ============ */

void Jeu_prendreRangee(Joueur *joueur, Rangee *rangee) {
    if (joueur == NULL || rangee == NULL) return;

    int points = Rangee_getTetesBoeuf(rangee);

    printf("❌ %s prend la rangée (%d pts): ", joueur->nom, points);
    for (int i = 0; i < rangee->nbCartes; i++) {
        printf("%d ", rangee->cartes[i].valeurNum);
    }
    printf("\n");

    joueur->score += points;
    Rangee_Init(rangee);
}

int Jeu_calculerPointsRangee(Rangee *rangee) {
    if (rangee == NULL) return 0;
    return Rangee_getTetesBoeuf(rangee);
}

/* ============ DÉROULEMENT DU JEU ============ */

void Jeu_jouerTour(Jeu *jeu) {
    if (jeu == NULL || jeu->tourActuel >= NB_TOURS) return;
    
    jeu->tourActuel++;
    printf("\n========== 🎯 TOUR %d/%d ==========\n\n", jeu->tourActuel, NB_TOURS);
    
    // Tableau pour stocker les cartes jouées par chaque joueur
    typedef struct {
        Carte carte;
        int joueur_id;
    } CarteJouee;
    
    CarteJouee *cartes_jouees = malloc(jeu->nbJoueurs * sizeof(CarteJouee));
    if (!cartes_jouees) {
        fprintf(stderr, "Erreur allocation cartes_jouees\n");
        return;
    }
    
    // 1. Chaque joueur choisit une carte (stratégie simple: première carte)
    for (int i = 0; i < jeu->nbJoueurs; i++) {
        Joueur *joueur = &jeu->joueurs[i];
        
        if (joueur->jeuCartes.nbCartes > 0) {
            cartes_jouees[i].carte = joueur->jeuCartes.cartes[0];
            cartes_jouees[i].joueur_id = i;
            
            // Retirer la carte de la main (utilise la fonction existante)
            Joueur_retirerCarte(joueur, 0);
            
            printf("🎴 %s joue: %d\n", joueur->nom, cartes_jouees[i].carte.valeurNum);
        }
    }
    
    printf("\n--- Placement des cartes ---\n\n");
    
    // 2. Trier les cartes par valeur croissante
    for (int i = 0; i < jeu->nbJoueurs - 1; i++) {
        for (int j = i + 1; j < jeu->nbJoueurs; j++) {
            if (cartes_jouees[i].carte.valeurNum > cartes_jouees[j].carte.valeurNum) {
                CarteJouee temp = cartes_jouees[i];
                cartes_jouees[i] = cartes_jouees[j];
                cartes_jouees[j] = temp;
            }
        }
    }
    
    // 3. Placer chaque carte dans l'ordre croissant
    for (int i = 0; i < jeu->nbJoueurs; i++) {
        Carte carte = cartes_jouees[i].carte;
        int joueur_id = cartes_jouees[i].joueur_id;
        Joueur *joueur = &jeu->joueurs[joueur_id];
        
        int rangee_idx = Jeu_trouverMeilleureRangee(&jeu->table, carte);
        
        if (rangee_idx == -1) {
            fprintf(stderr, "Erreur: Aucune rangée trouvée\n");
            continue;
        }
        
        Rangee *rangee = &jeu->table.rangees[rangee_idx];
        
        // Vérifier si la rangée est pleine (5 cartes)
        if (rangee->nbCartes >= 5) {
            Jeu_prendreRangee(joueur, rangee);
            Rangee_ajouterCarte(rangee, carte);
            printf("✅ Carte %d placée en début de rangée %d\n", carte.valeurNum, rangee_idx + 1);
        } else {
            Carte derniere = Rangee_derniereCarte(rangee);
            if (carte.valeurNum < derniere.valeurNum) {
                Jeu_prendreRangee(joueur, rangee);
                Rangee_ajouterCarte(rangee, carte);
                printf("✅ Carte %d placée en début de rangée %d (trop petite)\n", 
                       carte.valeurNum, rangee_idx + 1);
            } else {
                Rangee_ajouterCarte(rangee, carte);
                printf("✅ %s place %d sur rangée %d\n", 
                       joueur->nom, carte.valeurNum, rangee_idx + 1);
            }
        }
    }
    
    free(cartes_jouees);
    printf("\n--- Fin du tour %d ---\n", jeu->tourActuel);
}

void Jeu_appliquerTour(Jeu *jeu, Carte *cartes_jouees, int *indices_rangees) {
    if (jeu == NULL || cartes_jouees == NULL) return;
    
    typedef struct {
        Carte carte;
        int joueur_idx;
    } CarteTour;
    
    CarteTour cartes_tour[jeu->nbJoueurs];
    for (int i = 0; i < jeu->nbJoueurs; i++) {
        cartes_tour[i].carte = cartes_jouees[i];
        cartes_tour[i].joueur_idx = i;
    }
    
    // Trier par valeur
    for (int i = 0; i < jeu->nbJoueurs - 1; i++) {
        for (int j = i + 1; j < jeu->nbJoueurs; j++) {
            if (cartes_tour[i].carte.valeurNum > cartes_tour[j].carte.valeurNum) {
                CarteTour temp = cartes_tour[i];
                cartes_tour[i] = cartes_tour[j];
                cartes_tour[j] = temp;
            }
        }
    }
    
    for (int i = 0; i < jeu->nbJoueurs; i++) {
        Carte carte = cartes_tour[i].carte;
        int joueur_idx = cartes_tour[i].joueur_idx;
        
        int rangee_idx = Jeu_trouverMeilleureRangee(&jeu->table, carte);
        if (rangee_idx == -1) continue;
        
        Rangee *rangee = &jeu->table.rangees[rangee_idx];
        Carte derniere = Rangee_derniereCarte(rangee);
        
        if (derniere.valeurNum >= carte.valeurNum || rangee->nbCartes >= 5) {
            Jeu_prendreRangee(&jeu->joueurs[joueur_idx], rangee);
        }
        
        Rangee_ajouterCarte(rangee, carte);
        if (indices_rangees) indices_rangees[joueur_idx] = rangee_idx;
    }
}

int Jeu_estTermine(Jeu *jeu) {
    if (jeu == NULL) return 0;
    return jeu->tourActuel >= NB_TOURS;
}

void Jeu_placerCarte(Rangee *rangee, Carte carte) {
    if (rangee == NULL) return;
    Rangee_ajouterCarte(rangee, carte);
}

void afficher_carte(Carte *carte) {
    char *s = Carte_toString(carte);
    if (s) {
        printf("%s\n", s);
        free(s);
    }
}

/* ============ AFFICHAGE ============ */

void Jeu_afficherTableau(TableJeu *table) {
    if (table == NULL) return;

    printf("\n┌─────────────────────────────────────┐\n");
    printf("│       🎮 TABLEAU DE JEU 🎮          │\n");
    printf("└─────────────────────────────────────┘\n\n");
    
    for (int i = 0; i < NB_RANGEES_JEU; i++) {
        Rangee *rangee = &table->rangees[i];
        int pts = Rangee_getTetesBoeuf(rangee);
        
        printf("Rangée %d [%d 🐮]: ", i + 1, pts);
        
        if (rangee->nbCartes == 0) {
            printf("(vide)\n");
        } else {
            for (int j = 0; j < rangee->nbCartes; j++) {
                printf("%d(%d🐮) ", 
                       rangee->cartes[j].valeurNum, 
                       rangee->cartes[j].teteBoeuf);
            }
            printf("\n");
        }
    }
    printf("\n");
}

void Jeu_afficherScores(Jeu *jeu) {
    if (jeu == NULL) return;
    
    printf("\n┌─────────────────────────────────────┐\n");
    printf("│         📊 SCORES ACTUELS           │\n");
    printf("└─────────────────────────────────────┘\n");
    
    for (int i = 0; i < jeu->nbJoueurs; i++) {
        printf("  %s : %d points\n", jeu->joueurs[i].nom, jeu->joueurs[i].score);
    }
    printf("\n");
}

Joueur* Jeu_determinerGagnant(Jeu *jeu) {
    if (jeu == NULL || jeu->nbJoueurs <= 0) return NULL;
    
    Joueur *gagnant = &jeu->joueurs[0];
    
    for (int i = 1; i < jeu->nbJoueurs; i++) {
        if (jeu->joueurs[i].score < gagnant->score) {
            gagnant = &jeu->joueurs[i];
        }
    }
    
    return gagnant;
}
