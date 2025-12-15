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

/* Helper: convertir une Rangée en Collection pour utiliser Collection_toString */
static Collection Rangee_asCollection(Rangee *rangee) {
    Collection c;
    c.cartes = rangee->cartes;
    c.nbCartes = rangee->nbCartes;
    c.maxCartes = NB_CARTES_MAX_RANGEE;
    return c;
}

/* ============ PLACEMENT DE CARTES ============ */

int Jeu_trouverMeilleureRangee(TableJeu *table, Carte carte) {
    if (table == NULL) return -1;
    
    int meilleure_rangee = -1;
    int min_difference = INT_MAX;
    
    // RÈGLE: Chercher la rangée avec la PLUS PETITE différence
    // où carte > dernière_carte
    for (int i = 0; i < NB_RANGEES_JEU; i++) {
        Rangee *rangee = &table->rangees[i];
        
        if (rangee->nbCartes == 0) continue;
        
        Carte derniere = Rangee_derniereCarte(rangee);
        
        // La carte doit être plus grande que la dernière carte
        if (carte.valeurNum > derniere.valeurNum) {
            int difference = carte.valeurNum - derniere.valeurNum;
            
            // Prendre la rangée avec la PLUS PETITE différence
            if (difference < min_difference) {
                min_difference = difference;
                meilleure_rangee = i;
            }
        }
    }
    
    // Si aucune rangée ne convient (carte trop petite)
    // Retourner -1 pour que le joueur choisisse
    return meilleure_rangee;
}

/* Demander au joueur de choisir une rangée quand sa carte est trop petite */
int Jeu_choisirRangee(Joueur *joueur, TableJeu *table) {
    printf("\n⚠️  %s : Ta carte est trop petite !\n", joueur->nom);
    printf("Tu dois choisir une rangée à prendre.\n\n");
    
    Jeu_afficherTableau(table);
    
    int choix = -1;
    while (choix < 1 || choix > 4) {
        printf("\nChoisis une rangée (1-4) : ");
        fflush(stdout);
        
        if (scanf("%d", &choix) != 1) {
            // Nettoyer le buffer
            while (getchar() != '\n');
            choix = -1;
            continue;
        }
        
        if (choix < 1 || choix > 4) {
            printf("❌ Choix invalide ! Entre un nombre entre 1 et 4.\n");
        }
    }
    
    return choix - 1; // Retourner l'index (0-3)
}

/* ============ PRISE DE RANGÉE ============ */

void Jeu_prendreRangee(Joueur *joueur, Rangee *rangee) {
    if (joueur == NULL || rangee == NULL) return;

    int points = Rangee_getTetesBoeuf(rangee);

    printf("\n❌ %s prend la rangée (%d pts): ", joueur->nom, points);
    
    // Utiliser Collection_toString pour afficher les cartes
    Collection c = Rangee_asCollection(rangee);
    char *cartes_str = Collection_toString(&c);
    if (cartes_str) {
        printf("\n%s", cartes_str);
        free(cartes_str);
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
    printf("\n\n");
    printf("══════════════════════════\n");
    printf("║   🎯 TOUR %d/%d   ║\n", jeu->tourActuel, NB_TOURS);
    printf("══════════════════════════\n");
    
    Jeu_afficherTableau(&jeu->table);
    
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
    
    printf("\n🎴 Sélection des cartes...\n\n");
    
    // 1. Chaque joueur choisit une carte
    for (int i = 0; i < jeu->nbJoueurs; i++) {
        Joueur *joueur = &jeu->joueurs[i];
        
        if (joueur->jeuCartes.nbCartes == 0) continue;
        
        printf("--- %s, c'est ton tour ! ---\n\n", joueur->nom);
        
        // Afficher la main du joueur
        printf("Ta main:\n");
        char *main_str = Collection_toString(&joueur->jeuCartes);
        if (main_str) {
            printf("%s\n", main_str);
            free(main_str);
        }
        
        // Demander au joueur de choisir une carte
        int choix_carte = -1;
        while (choix_carte < 1 || choix_carte > joueur->jeuCartes.nbCartes) {
            printf("\nChoisis une carte (1-%d) : ", joueur->jeuCartes.nbCartes);
            fflush(stdout);
            
            if (scanf("%d", &choix_carte) != 1) {
                while (getchar() != '\n');
                choix_carte = -1;
                continue;
            }
            
            if (choix_carte < 1 || choix_carte > joueur->jeuCartes.nbCartes) {
                printf("❌ Choix invalide !\n");
            }
        }
        
        int index = choix_carte - 1;
        cartes_jouees[i].carte = joueur->jeuCartes.cartes[index];
        cartes_jouees[i].joueur_id = i;
        
        printf("\n✅ Carte sélectionnée et posée face cachée !\n\n");
        
        // Retirer la carte de la main
        Joueur_retirerCarte(joueur, index);
    }
    
    printf("\n🔄 Révélation et placement des cartes...\n\n");
    
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
        
        // Afficher la carte jouée
        char *carte_str = Carte_toString(&carte);
        printf("%s joue:\n", joueur->nom);
        if (carte_str) {
            printf("%s\n", carte_str);
            free(carte_str);
        }
        
        // Trouver la meilleure rangée (automatique)
        int rangee_idx = Jeu_trouverMeilleureRangee(&jeu->table, carte);
        
        // Si aucune rangée ne convient (carte trop petite)
        if (rangee_idx == -1) {
            rangee_idx = Jeu_choisirRangee(joueur, &jeu->table);
            Rangee *rangee = &jeu->table.rangees[rangee_idx];
            Jeu_prendreRangee(joueur, rangee);
            Rangee_ajouterCarte(rangee, carte);
            printf("✅ Nouvelle rangée %d commencée\n\n", rangee_idx + 1);
            continue;
        }
        
        Rangee *rangee = &jeu->table.rangees[rangee_idx];
        
        // Vérifier si la rangée est pleine (5 cartes = 6ème carte)
        if (rangee->nbCartes >= 5) {
            printf("\n🚨 6ème carte ! %s doit prendre la rangée !\n", joueur->nom);
            Jeu_prendreRangee(joueur, rangee);
            Rangee_ajouterCarte(rangee, carte);
            printf("✅ Nouvelle rangée %d commencée\n\n", rangee_idx + 1);
        } else {
            // Placement normal
            Rangee_ajouterCarte(rangee, carte);
            printf("✅ Carte placée sur rangée %d\n\n", rangee_idx + 1);
        }
    }
    
    free(cartes_jouees);
    
    printf("--- Fin du tour %d ---\n", jeu->tourActuel);
    Jeu_afficherTableau(&jeu->table);
}

void Jeu_appliquerTour(Jeu *jeu, Carte *cartes_jouees, int *indices_rangees) {
    // Cette fonction est pour le mode réseau (non utilisée en local)
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
        
        if (rangee->nbCartes >= 5) {
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
            printf("(vide)\n\n");
        } else {
            printf("\n");
            // Utiliser Collection_toString pour afficher toutes les cartes
            Collection c = Rangee_asCollection(rangee);
            char *rangee_str = Collection_toString(&c);
            if (rangee_str) {
                printf("%s\n", rangee_str);
                free(rangee_str);
            }
        }
    }
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
