#include "jeu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>

/* ============ FONCTIONS RÉSEAU ============ */

/* Envoyer un message à un client */
static int envoyer_message(int socket, const char *message) {
    if (socket == -1) return -1;  // Bot, pas de socket
    ssize_t sent = send(socket, message, strlen(message), 0);
    return (sent == -1) ? -1 : 0;
}

/* Recevoir un message d'un client */
static int recevoir_message(int socket, char *buffer, size_t max_len) {
    if (socket == -1) return -1;  // Bot, pas de socket
    ssize_t received = recv(socket, buffer, max_len - 1, 0);
    if (received <= 0) return -1;
    buffer[received] = '\0';
    // Supprimer le newline
    if (buffer[received - 1] == '\n') {
        buffer[received - 1] = '\0';
    }
    return 0;
}

/* Vérifier si une carte peut être posée sur au moins une rangée */
static int carte_est_jouable(TableJeu *table, Carte carte) {
    // Une carte est jouable si elle est plus grande qu'au moins une dernière carte
    for (int i = 0; i < NB_RANGEES_JEU; i++) {
        if (table->rangees[i].nbCartes > 0) {
            Carte derniere = Rangee_derniereCarte(&table->rangees[i]);
            if (carte.valeurNum > derniere.valeurNum) {
                return 1;  // Jouable
            }
        }
    }
    return 0;  // Trop petite partout
}

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
        joueurs[j].id = j;  // Définir l'ID
        
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
        
        // Envoyer la main au joueur humain
        if (!joueurs[j].is_bot) {
            char msg[2048];
            snprintf(msg, sizeof(msg), "\n=== Ta main ===\n");
            envoyer_message(joueurs[j].socket, msg);
            
            char *main_str = Collection_toString(&joueurs[j].jeuCartes);
            if (main_str) {
                envoyer_message(joueurs[j].socket, main_str);
                envoyer_message(joueurs[j].socket, "\n");
                free(main_str);
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

/* Helper: convertir une Rangée en Collection */
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
    for (int i = 0; i < NB_RANGEES_JEU; i++) {
        Rangee *rangee = &table->rangees[i];
        
        if (rangee->nbCartes == 0) continue;
        
        Carte derniere = Rangee_derniereCarte(rangee);
        
        if (carte.valeurNum > derniere.valeurNum) {
            int difference = carte.valeurNum - derniere.valeurNum;
            
            if (difference < min_difference) {
                min_difference = difference;
                meilleure_rangee = i;
            }
        }
    }
    
    return meilleure_rangee;  // -1 si carte trop petite
}

/* Demander au joueur de choisir une rangée (carte trop petite) */
int Jeu_choisirRangee(Joueur *joueur, TableJeu *table) {
    printf("\n⚠️  %s : Carte trop petite, doit choisir une rangée\n", joueur->nom);
    
    // Si c'est un bot, choisir la rangée avec le moins de points
    if (joueur->is_bot) {
        int min_pts = INT_MAX;
        int meilleure = 0;
        for (int i = 0; i < NB_RANGEES_JEU; i++) {
            int pts = Rangee_getTetesBoeuf(&table->rangees[i]);
            if (pts < min_pts) {
                min_pts = pts;
                meilleure = i;
            }
        }
        printf("  Bot choisit rangée %d (%d pts)\n", meilleure + 1, min_pts);
        return meilleure;
    }
    
    // Envoyer l'état du tableau au joueur
    char msg[4096];
    snprintf(msg, sizeof(msg), "\n⚠️  Ta carte est trop petite ! Choisis une rangée à prendre (1-4):\n\n");
    envoyer_message(joueur->socket, msg);
    
    // Envoyer le tableau
    for (int i = 0; i < NB_RANGEES_JEU; i++) {
        Rangee *rangee = &table->rangees[i];
        int pts = Rangee_getTetesBoeuf(rangee);
        snprintf(msg, sizeof(msg), "Rangée %d [%d pts]: ", i + 1, pts);
        envoyer_message(joueur->socket, msg);
        
        if (rangee->nbCartes > 0) {
            Collection c = Rangee_asCollection(rangee);
            char *rangee_str = Collection_toString(&c);
            if (rangee_str) {
                envoyer_message(joueur->socket, rangee_str);
                envoyer_message(joueur->socket, "\n");
                free(rangee_str);
            }
        }
    }
    
    snprintf(msg, sizeof(msg), "\nTon choix (1-4): ");
    envoyer_message(joueur->socket, msg);
    
    // Recevoir le choix
    char buffer[32];
    int choix = -1;
    while (choix < 1 || choix > 4) {
        if (recevoir_message(joueur->socket, buffer, sizeof(buffer)) == 0) {
            choix = atoi(buffer);
            if (choix < 1 || choix > 4) {
                snprintf(msg, sizeof(msg), "Choix invalide. Réessaie (1-4): ");
                envoyer_message(joueur->socket, msg);
            }
        }
    }
    
    printf("  %s choisit rangée %d\n", joueur->nom, choix);
    return choix - 1;
}

/* ============ PRISE DE RANGÉE ============ */

void Jeu_prendreRangee(Joueur *joueur, Rangee *rangee) {
    if (joueur == NULL || rangee == NULL) return;

    int points = Rangee_getTetesBoeuf(rangee);

    printf("\n❌ %s prend la rangée (%d pts)\n", joueur->nom, points);
    
    // Notifier le joueur
    if (!joueur->is_bot) {
        char msg[512];
        snprintf(msg, sizeof(msg), "\n❌ Tu prends la rangée ! +%d points\n", points);
        envoyer_message(joueur->socket, msg);
    }

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
    
    // Envoyer l'état du tableau à tous les joueurs
    char msg[4096];
    for (int i = 0; i < jeu->nbJoueurs; i++) {
        if (!jeu->joueurs[i].is_bot) {
            snprintf(msg, sizeof(msg), "\n=== TOUR %d/%d ===\n\n", jeu->tourActuel, NB_TOURS);
            envoyer_message(jeu->joueurs[i].socket, msg);
            
            // Envoyer le tableau
            for (int r = 0; r < NB_RANGEES_JEU; r++) {
                Rangee *rangee = &jeu->table.rangees[r];
                int pts = Rangee_getTetesBoeuf(rangee);
                snprintf(msg, sizeof(msg), "Rangée %d [%d pts]: \n", r + 1, pts);
                envoyer_message(jeu->joueurs[i].socket, msg);
                
                if (rangee->nbCartes > 0) {
                    Collection c = Rangee_asCollection(rangee);
                    char *rangee_str = Collection_toString(&c);
                    if (rangee_str) {
                        envoyer_message(jeu->joueurs[i].socket, rangee_str);
                        envoyer_message(jeu->joueurs[i].socket, "\n");
                        free(rangee_str);
                    }
                }
            }
        }
    }
    
    Jeu_afficherTableau(&jeu->table);
    
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
        
        int choix_carte = -1;
        
        if (joueur->is_bot) {
            // Bot: jouer la plus petite carte
            choix_carte = 0;
            printf("  Bot %s joue sa carte\n", joueur->nom);
        } else {
            // Humain: demander via le réseau
            snprintf(msg, sizeof(msg), "\n=== C'est ton tour %s ! ===\n\nTa main:\n", joueur->nom);
            envoyer_message(joueur->socket, msg);
            
            char *main_str = Collection_toString(&joueur->jeuCartes);
            if (main_str) {
                envoyer_message(joueur->socket, main_str);
                envoyer_message(joueur->socket, "\n");
                free(main_str);
            }
            
            // Boucle de validation : redemander si la carte n'est pas jouable
            int carte_valide = 0;
            while (!carte_valide) {
                snprintf(msg, sizeof(msg), "\nChoisis une carte (1-%d): ", joueur->jeuCartes.nbCartes);
                envoyer_message(joueur->socket, msg);
                
                char buffer[32];
                choix_carte = -1;
                
                // Valider le choix (dans la plage)
                while (choix_carte < 1 || choix_carte > joueur->jeuCartes.nbCartes) {
                    if (recevoir_message(joueur->socket, buffer, sizeof(buffer)) == 0) {
                        choix_carte = atoi(buffer);
                        if (choix_carte < 1 || choix_carte > joueur->jeuCartes.nbCartes) {
                            snprintf(msg, sizeof(msg), "Choix invalide ! (1-%d): ", joueur->jeuCartes.nbCartes);
                            envoyer_message(joueur->socket, msg);
                        }
                    }
                }
                choix_carte--;  // Index 0-based
                
                // Vérifier si la carte est jouable
                Carte carte_choisie = joueur->jeuCartes.cartes[choix_carte];
                if (carte_est_jouable(&jeu->table, carte_choisie)) {
                    carte_valide = 1;
                } else {
                    // Vérifier s'il y a d'autres cartes jouables
                    int autre_jouable = 0;
                    for (int c = 0; c < joueur->jeuCartes.nbCartes; c++) {
                        if (carte_est_jouable(&jeu->table, joueur->jeuCartes.cartes[c])) {
                            autre_jouable = 1;
                            break;
                        }
                    }
                    
                    if (autre_jouable) {
                        snprintf(msg, sizeof(msg), "\n⚠️  Cette carte (%d) est trop petite et ne peut pas être posée !\n", carte_choisie.valeurNum);
                        envoyer_message(joueur->socket, msg);
                        snprintf(msg, sizeof(msg), "Tu as d'autres cartes jouables, choisis-en une autre.\n\n");
                        envoyer_message(joueur->socket, msg);
                        
                        // Réafficher la main
                        char *main_str2 = Collection_toString(&joueur->jeuCartes);
                        if (main_str2) {
                            envoyer_message(joueur->socket, main_str2);
                            envoyer_message(joueur->socket, "\n");
                            free(main_str2);
                        }
                    } else {
                        // Aucune carte jouable, accepter quand même
                        carte_valide = 1;
                    }
                }
            }
            
            printf("  %s a choisi sa carte\n", joueur->nom);
            
            snprintf(msg, sizeof(msg), "\nCarte posée face cachée ! En attente des autres joueurs...\n");
            envoyer_message(joueur->socket, msg);
        }
        
        cartes_jouees[i].carte = joueur->jeuCartes.cartes[choix_carte];
        cartes_jouees[i].joueur_id = i;
        
        Joueur_retirerCarte(joueur, choix_carte);
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
        
        printf("%s joue: %d\n", joueur->nom, carte.valeurNum);
        
        int rangee_idx = Jeu_trouverMeilleureRangee(&jeu->table, carte);
        
        if (rangee_idx == -1) {
            rangee_idx = Jeu_choisirRangee(joueur, &jeu->table);
            Rangee *rangee = &jeu->table.rangees[rangee_idx];
            Jeu_prendreRangee(joueur, rangee);
            Rangee_ajouterCarte(rangee, carte);
            printf("✅ Nouvelle rangée %d commencée\n\n", rangee_idx + 1);
            continue;
        }
        
        Rangee *rangee = &jeu->table.rangees[rangee_idx];
        
        if (rangee->nbCartes >= 5) {
            printf("\n🚨 6ème carte ! %s doit prendre la rangée !\n", joueur->nom);
            Jeu_prendreRangee(joueur, rangee);
            Rangee_ajouterCarte(rangee, carte);
            printf("✅ Nouvelle rangée %d commencée\n\n", rangee_idx + 1);
        } else {
            Rangee_ajouterCarte(rangee, carte);
            printf("✅ Carte placée sur rangée %d\n\n", rangee_idx + 1);
        }
    }
    
    free(cartes_jouees);
    
    printf("--- Fin du tour %d ---\n", jeu->tourActuel);
    Jeu_afficherTableau(&jeu->table);
    Jeu_afficherScores(jeu);
}

void Jeu_appliquerTour(Jeu *jeu, Carte *cartes_jouees, int *indices_rangees) {
    // Non utilisée en mode réseau
    (void)jeu;
    (void)cartes_jouees;
    (void)indices_rangees;
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
        
        printf("Rangée %d [%d 🐮]: \n", i + 1, pts);
        
        if (rangee->nbCartes == 0) {
            printf("(vide)\n\n");
        } else {
            printf("\n");
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
