#include "../headers/global.h"


static int envoyer_message(int socket, const char *message) {
    if (socket == -1) return -1;
    ssize_t sent = send(socket, message, strlen(message), 0);
    return (sent == -1) ? -1 : 0;
}

static int recevoir_message(int socket, char *buffer, size_t max_len) {
    if (socket == -1) return -1;
    ssize_t received = recv(socket, buffer, max_len - 1, 0);
    
    if (received <= 0) {
        printf("\n⚠️ Un joueur s'est déconnecté, fin de la partie.\n\n");
        fflush(stdout);
        return -1;
    }
    
    buffer[received] = '\0';
    if (buffer[received - 1] == '\n') {
        buffer[received - 1] = '\0';
    }
    
    return 0;
}

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
    
    for (int i = 0; i < NB_RANGEES_JEU; i++) {
        Rangee_Init(&jeu->table.rangees[i]);
    }
    
    Carte *deck_cartes = malloc(DECK_TOTAL * sizeof(Carte));
    if (!deck_cartes) {
        fprintf(stderr, "Erreur allocation deck\n");
        return;
    }
    
    for (int i = 0; i < DECK_TOTAL; i++) {
        Carte_InitNum(&deck_cartes[i], i + 1);
    }
    
    srand(time(NULL) + getpid());
    for (int i = DECK_TOTAL - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Carte temp = deck_cartes[i];
        deck_cartes[i] = deck_cartes[j];
        deck_cartes[j] = temp;
    }
    
    int carte_index = 0;
    for (int j = 0; j < nbJoueurs; j++) {
        joueurs[j].jeuCartes.cartes = malloc(NB_CARTES_PAR_JOUEUR * sizeof(Carte));
        if (!joueurs[j].jeuCartes.cartes) {
            fprintf(stderr, "Erreur allocation cartes joueur %d\n", j);
            continue;
        }
        
        joueurs[j].jeuCartes.nbCartes = NB_CARTES_PAR_JOUEUR;
        joueurs[j].jeuCartes.maxCartes = NB_CARTES_PAR_JOUEUR;
        joueurs[j].id = j;
        
        for (int c = 0; c < NB_CARTES_PAR_JOUEUR; c++) {
            joueurs[j].jeuCartes.cartes[c] = deck_cartes[carte_index++];
        }
        
        for (int a = 0; a < joueurs[j].jeuCartes.nbCartes - 1; a++) {
            for (int b = a + 1; b < joueurs[j].jeuCartes.nbCartes; b++) {
                if (joueurs[j].jeuCartes.cartes[a].valeurNum > joueurs[j].jeuCartes.cartes[b].valeurNum) {
                    Carte temp = joueurs[j].jeuCartes.cartes[a];
                    joueurs[j].jeuCartes.cartes[a] = joueurs[j].jeuCartes.cartes[b];
                    joueurs[j].jeuCartes.cartes[b] = temp;
                }
            }
        }
        
        if (!joueurs[j].is_bot) {
            char msg[2048];
            snprintf(msg, sizeof(msg), "\n=== Ta main ===\n");
            envoyer_message(joueurs[j].socket, msg);
            
            char *main_str = Collection_toString(&joueurs[j].jeuCartes, 1);
            if (main_str) {
                envoyer_message(joueurs[j].socket, main_str);
                envoyer_message(joueurs[j].socket, "\n");
                free(main_str);
            }
        }
    }
    
    for (int i = 0; i < NB_RANGEES_JEU; i++) {
        Rangee_ajouterCarte(&jeu->table.rangees[i], deck_cartes[carte_index++]);
    }
    
    jeu->deck.cartes = deck_cartes;
    jeu->deck.nbCartes = DECK_TOTAL;
    jeu->deck.maxCartes = DECK_TOTAL;
    
    printf("\n🎮 Jeu initialisé avec %d joueurs\n", nbJoueurs);
    printf("📊 Cartes distribuées: %d cartes par joueur\n", NB_CARTES_PAR_JOUEUR);
    printf("🃏 Rangées initiales placées\n\n");
    
    printf("\n═══════════════════════════════════════\n");
    printf("  📊 ÉTAT INITIAL DU PLATEAU 📊\n");
    printf("═══════════════════════════════════════\n\n");
    Jeu_afficherTableau(&jeu->table);

    for (int i = 0; i < nbJoueurs; i++) {
        if (!joueurs[i].is_bot) {
            char msg[4096];
            snprintf(msg, sizeof(msg), "\n═══════════════════════════════════════\n");
            envoyer_message(joueurs[i].socket, msg);
            snprintf(msg, sizeof(msg), "  📊 ÉTAT INITIAL DU PLATEAU 📊\n");
            envoyer_message(joueurs[i].socket, msg);
            snprintf(msg, sizeof(msg), "═══════════════════════════════════════\n\n");
            envoyer_message(joueurs[i].socket, msg);
            
            for (int r = 0; r < NB_RANGEES_JEU; r++) {
                Rangee *rangee = &jeu->table.rangees[r];
                int pts = Rangee_getTetesBoeuf(rangee);
                snprintf(msg, sizeof(msg), "Rangée %d [%d 🐮]:\n", r + 1, pts);
                envoyer_message(joueurs[i].socket, msg);
                
                if (rangee->nbCartes > 0) {
                    Collection c;
                    c.cartes = rangee->cartes;
                    c.nbCartes = rangee->nbCartes;
                    c.maxCartes = NB_CARTES_MAX_RANGEE;
                    
                    char *rangee_str = Collection_toString(&c, 0);
                    if (rangee_str) {
                        envoyer_message(joueurs[i].socket, rangee_str);
                        free(rangee_str);
                    }
                } else {
                    snprintf(msg, sizeof(msg), "(vide)\n");
                    envoyer_message(joueurs[i].socket, msg);
                }
            }
            
            snprintf(msg, sizeof(msg), "\n⏳ En attente du démarrage du jeu...\n");
            envoyer_message(joueurs[i].socket, msg);
        }
    }

    sleep(3);

}

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

static Collection Rangee_asCollection(Rangee *rangee) {
    Collection c;
    c.cartes = rangee->cartes;
    c.nbCartes = rangee->nbCartes;
    c.maxCartes = NB_CARTES_MAX_RANGEE;
    return c;
}


int Jeu_trouverMeilleureRangee(TableJeu *table, Carte carte) {
    if (table == NULL) return -1;
    
    int meilleure_rangee = -1;
    int min_difference = INT_MAX;
    
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
    
    return meilleure_rangee;
}

int Jeu_choisirRangee(Joueur *joueur, TableJeu *table) {
    printf("\n⚠️ %s : Carte trop petite, doit choisir une rangée\n", joueur->nom);
    
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
        printf(" Bot choisit rangée %d (%d pts)\n", meilleure + 1, min_pts);
        return meilleure;
    }
    
    char msg[4096];
    snprintf(msg, sizeof(msg), "\n⚠️ Ta carte est trop petite ! Choisis une rangée à prendre (1-%d):\n\n", NB_RANGEES_JEU);
    envoyer_message(joueur->socket, msg);
    
    for (int i = 0; i < NB_RANGEES_JEU; i++) {
        Rangee *rangee = &table->rangees[i];
        int pts = Rangee_getTetesBoeuf(rangee);
        snprintf(msg, sizeof(msg), "Rangée %d [%d pts]:\n", i + 1, pts);
        envoyer_message(joueur->socket, msg);
        
        if (rangee->nbCartes > 0) {
            Collection c = Rangee_asCollection(rangee);
            char *rangee_str = Collection_toString(&c, 0);
            if (rangee_str) {
                envoyer_message(joueur->socket, rangee_str);
                envoyer_message(joueur->socket, "\n");
                free(rangee_str);
            }
        } else {
            snprintf(msg, sizeof(msg), "(vide)\n");
            envoyer_message(joueur->socket, msg);
        }
    }
    
    snprintf(msg, sizeof(msg), "\nTon choix (1-%d): ", NB_RANGEES_JEU);
    envoyer_message(joueur->socket, msg);
    
    char buffer[32];
    int choix = -1;
    
    while (choix < 1 || choix > 4) {
        int recv_result = recevoir_message(joueur->socket, buffer, sizeof(buffer));
        
        if (recv_result != 0) {
            printf("\n❌ %s s'est deconnecte!\n", joueur->nom);
            return -1;
        }
        
        choix = atoi(buffer);
        if (choix < 1 || choix > 4) {
            snprintf(msg, sizeof(msg), "Choix invalide. Réessaie (1-%d): ", NB_RANGEES_JEU);
            envoyer_message(joueur->socket, msg);
        }
    }
    
    printf(" %s choisit rangée %d\n", joueur->nom, choix);
    return choix - 1;
}

void Jeu_prendreRangee(Joueur *joueur, Rangee *rangee) {
    if (joueur == NULL || rangee == NULL) return;
    
    int points = Rangee_getTetesBoeuf(rangee);
    printf("\n❌ %s prend la rangée (%d pts)\n", joueur->nom, points);
    
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

void Jeu_reinitialiserManche(Jeu *jeu) {
    if (jeu == NULL) return;
    
    printf("\n\n═══════════════════════════════════════\n");
    printf("  🎰 NOUVELLE MANCHE 🎰\n");
    printf("═══════════════════════════════════════\n\n");
    
    for (int i = 0; i < NB_RANGEES_JEU; i++) {
        Rangee_Init(&jeu->table.rangees[i]);
    }
    
    if (jeu->deck.cartes != NULL) {
        free(jeu->deck.cartes);
    }
    
    Carte *deck_cartes = malloc(DECK_TOTAL * sizeof(Carte));
    if (!deck_cartes) {
        fprintf(stderr, "❌ Erreur allocation deck\n");
        return;
    }
    
    for (int i = 0; i < DECK_TOTAL; i++) {
        Carte_InitNum(&deck_cartes[i], i + 1);
    }
    
    srand(time(NULL) + getpid());
    for (int i = DECK_TOTAL - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Carte temp = deck_cartes[i];
        deck_cartes[i] = deck_cartes[j];
        deck_cartes[j] = temp;
    }
    
    int carte_index = 0;
    for (int j = 0; j < jeu->nbJoueurs; j++) {
        Joueur *joueur = &jeu->joueurs[j];
        
        if (joueur->jeuCartes.cartes != NULL) {
            free(joueur->jeuCartes.cartes);
        }
        
        joueur->jeuCartes.cartes = malloc(NB_CARTES_PAR_JOUEUR * sizeof(Carte));
        if (!joueur->jeuCartes.cartes) {
            fprintf(stderr, "❌ Erreur allocation cartes joueur %d\n", j);
            continue;
        }
        
        joueur->jeuCartes.nbCartes = NB_CARTES_PAR_JOUEUR;
        joueur->jeuCartes.maxCartes = NB_CARTES_PAR_JOUEUR;
        
        for (int c = 0; c < NB_CARTES_PAR_JOUEUR; c++) {
            joueur->jeuCartes.cartes[c] = deck_cartes[carte_index++];
        }
        
        for (int a = 0; a < joueur->jeuCartes.nbCartes - 1; a++) {
            for (int b = a + 1; b < joueur->jeuCartes.nbCartes; b++) {
                if (joueur->jeuCartes.cartes[a].valeurNum > joueur->jeuCartes.cartes[b].valeurNum) {
                    Carte temp = joueur->jeuCartes.cartes[a];
                    joueur->jeuCartes.cartes[a] = joueur->jeuCartes.cartes[b];
                    joueur->jeuCartes.cartes[b] = temp;
                }
            }
        }
        
        if (!joueur->is_bot) {
            char msg[2048];
            snprintf(msg, sizeof(msg), "\n🎰 === NOUVELLE MANCHE ===\nTa nouvelle main:\n");
            envoyer_message(joueur->socket, msg);
            
            char *main_str = Collection_toString(&joueur->jeuCartes, 1);
            if (main_str) {
                envoyer_message(joueur->socket, main_str);
                envoyer_message(joueur->socket, "\n");
                free(main_str);
            }
        }
    }
    
    for (int i = 0; i < NB_RANGEES_JEU; i++) {
        Rangee_ajouterCarte(&jeu->table.rangees[i], deck_cartes[carte_index++]);
    }
    
    jeu->deck.cartes = deck_cartes;
    jeu->deck.nbCartes = DECK_TOTAL;
    jeu->deck.maxCartes = DECK_TOTAL;
    
    printf("✅ Manche réinitialisée!\n");
    printf("📊 Scores conservés:\n");
    for (int i = 0; i < jeu->nbJoueurs; i++) {
        printf("   %s: %d points\n", jeu->joueurs[i].nom, jeu->joueurs[i].score);
    }
    printf("\n");
}

void Jeu_jouerTour(Jeu *jeu) {
    if (jeu == NULL || Jeu_estTermine(jeu)) return;
    
    jeu->tourActuel++;
    
    printf("\n\n");
    printf("══════════════════════════\n");
    printf("║ 🎯 TOUR %d ║\n", jeu->tourActuel);
    printf("══════════════════════════\n\n");
    
    char msg[4096];
    
    CarteJouee *cartes_jouees = malloc(jeu->nbJoueurs * sizeof(CarteJouee));
    if (!cartes_jouees) {
        fprintf(stderr, "Erreur allocation cartes_jouees\n");
        return;
    }
    
    Jeu_afficherTableau(&jeu->table);
    Jeu_afficherScores(jeu);
    
    sleep(2);
    
    printf("\n🎴 Sélection des cartes...\n\n");
    
    for (int i = 0; i < jeu->nbJoueurs; i++) {
        Joueur *joueur = &jeu->joueurs[i];
        
        for (int j = 0; j < jeu->nbJoueurs; j++) {
            if (!jeu->joueurs[j].is_bot) {
                if (i == j) {
                    snprintf(msg, sizeof(msg), "\n⏳ C'est à TON TOUR de choisir une carte!\n");
                    envoyer_message(jeu->joueurs[j].socket, msg);
                } else if (j > i) {
                    snprintf(msg, sizeof(msg), "⏳ En attente de %s...\n", joueur->nom);
                    envoyer_message(jeu->joueurs[j].socket, msg);
                }
                envoyer_message(jeu->joueurs[j].socket, msg);
            }
        }
        
        if (joueur->jeuCartes.nbCartes == 0) {
            printf("⚠️ %s n'a plus de cartes! Redistribution...\n", joueur->nom);
            
            for (int c = 0; c < NB_CARTES_PAR_JOUEUR && jeu->deck.nbCartes > 0; c++) {
                joueur->jeuCartes.cartes[c] = jeu->deck.cartes[jeu->deck.nbCartes - 1];
                jeu->deck.nbCartes--;
                joueur->jeuCartes.nbCartes++;
            }
            
            if (joueur->jeuCartes.nbCartes == 0) {
                printf("❌ Pas assez de cartes dans le deck! Fin de manche.\n");
                free(cartes_jouees);
                return;
            }
            
            printf("✅ %s a reçu %d cartes\n", joueur->nom, joueur->jeuCartes.nbCartes);
        }
        
        int choix_carte = -1;
        
        if (joueur->is_bot) {
            choix_carte = 0;
            printf(" 🤖 Bot %s joue sa carte\n", joueur->nom);
            sleep(1);
        } else {
            snprintf(msg, sizeof(msg), "=== C'est ton tour %s ! ===\nTa main:\n", joueur->nom);
            envoyer_message(joueur->socket, msg);
            
            char *main_str = Collection_toString(&joueur->jeuCartes, 1);
            if (main_str) {
                envoyer_message(joueur->socket, main_str);
                free(main_str);
            }
            
            snprintf(msg, sizeof(msg), "Choisis une carte (1-%d): ", joueur->jeuCartes.nbCartes);
            envoyer_message(joueur->socket, msg);
            
            char buffer[32];
            choix_carte = -1;
            
            while (choix_carte < 1 || choix_carte > joueur->jeuCartes.nbCartes) {
                int recv_result = recevoir_message(joueur->socket, buffer, sizeof(buffer));
                
                if (recv_result != 0) {
                    printf("\n❌ %s s'est deconnecte pendant la partie!\n", joueur->nom);
                    free(cartes_jouees);
                    return;
                }
                
                choix_carte = atoi(buffer);
                
                if (choix_carte < 1 || choix_carte > joueur->jeuCartes.nbCartes) {
                    snprintf(msg, sizeof(msg), "Choix invalide ! (1-%d): ", joueur->jeuCartes.nbCartes);
                    envoyer_message(joueur->socket, msg);
                    choix_carte = -1;
                }
            }
            
            choix_carte--;
        }
        
        printf(" %s a choisi sa carte\n", joueur->nom);
        snprintf(msg, sizeof(msg), "\n✅ Carte posée face cachée ! En attente des autres joueurs...\n");
        envoyer_message(joueur->socket, msg);
        
        cartes_jouees[i].carte = joueur->jeuCartes.cartes[choix_carte];
        cartes_jouees[i].joueur_id = i;
        
        Joueur_retirerCarte(joueur, choix_carte);
    }
    
    system("clear");
    printf("\n═══════════════════════════════════════\n");
    printf("  🔄 RÉVÉLATION DES CARTES 🔄\n");
    printf("═══════════════════════════════════════\n\n");
    
    sleep(2);
    
    for (int i = 0; i < jeu->nbJoueurs - 1; i++) {
        for (int j = i + 1; j < jeu->nbJoueurs; j++) {
            if (cartes_jouees[i].carte.valeurNum > cartes_jouees[j].carte.valeurNum) {
                CarteJouee temp = cartes_jouees[i];
                cartes_jouees[i] = cartes_jouees[j];
                cartes_jouees[j] = temp;
            }
        }
    }
    
    for (int i = 0; i < jeu->nbJoueurs; i++) {
        Carte carte = cartes_jouees[i].carte;
        int joueur_id = cartes_jouees[i].joueur_id;
        Joueur *joueur = &jeu->joueurs[joueur_id];
        
        printf("📌 %s joue: %d\n", joueur->nom, carte.valeurNum);
        
        int rangee_idx = Jeu_trouverMeilleureRangee(&jeu->table, carte);
        
        if (rangee_idx == -1) {
            rangee_idx = Jeu_choisirRangee(joueur, &jeu->table);
            
            if (rangee_idx == -1) {
                printf("❌ Joueur déconnecté, partie interrompue!\n");
                free(cartes_jouees);
                return;
            }
            
            Rangee *rangee = &jeu->table.rangees[rangee_idx];
            int points = Rangee_getTetesBoeuf(rangee);
            
            printf("\n💥 %s - CARTE TROP PETITE (%d) !\n", joueur->nom, carte.valeurNum);
            printf("   → Prend la rangée %d et empoche %d tête(s) de boeuf 🐮\n\n", rangee_idx + 1, points);
            
            for (int j = 0; j < jeu->nbJoueurs; j++) {
                if (!jeu->joueurs[j].is_bot) {
                    snprintf(msg, sizeof(msg), "\n💥 %s - Carte %d trop petite!\n   → Prend la rangée %d et empoche %d tête(s) de boeuf 🐮\n", 
                             joueur->nom, carte.valeurNum, rangee_idx + 1, points);
                    envoyer_message(jeu->joueurs[j].socket, msg);
                }
            }
            
            Jeu_prendreRangee(joueur, rangee);
            Rangee_ajouterCarte(rangee, carte);
            
            sleep(2);
            system("clear");
            Jeu_afficherTableau(&jeu->table);
            sleep(1);
            continue;
        }
        
        Rangee *rangee = &jeu->table.rangees[rangee_idx];
        
        if (rangee->nbCartes >= 5) {
            int points = Rangee_getTetesBoeuf(rangee);
            
            printf("\n🚨 %s - 6ÈME CARTE SUR RANGÉE %d !\n", joueur->nom, rangee_idx + 1);
            printf("   → Récolte la rangée et empoche %d tête(s) de boeuf 🐮\n\n", points);
            
            for (int j = 0; j < jeu->nbJoueurs; j++) {
                if (!jeu->joueurs[j].is_bot) {
                    snprintf(msg, sizeof(msg), "\n🚨 %s - 6ème carte sur rangée %d!\n   → Récolte la rangée et empoche %d tête(s) de boeuf 🐮\n", 
                             joueur->nom, rangee_idx + 1, points);
                    envoyer_message(jeu->joueurs[j].socket, msg);
                }
            }
            
            Jeu_prendreRangee(joueur, rangee);
            Rangee_ajouterCarte(rangee, carte);
            
            sleep(2);
            system("clear");
            Jeu_afficherTableau(&jeu->table);
            sleep(1);
        } else {
            Rangee_ajouterCarte(rangee, carte);
            printf("✅ %s - Carte %d placée sur rangée %d\n\n", joueur->nom, carte.valeurNum, rangee_idx + 1);
            
            for (int j = 0; j < jeu->nbJoueurs; j++) {
                if (!jeu->joueurs[j].is_bot) {
                    snprintf(msg, sizeof(msg), "✅ %s - Carte %d placée sur rangée %d\n", 
                             joueur->nom, carte.valeurNum, rangee_idx + 1);
                    envoyer_message(jeu->joueurs[j].socket, msg);
                }
            }
        }
    }
    
    free(cartes_jouees);
    
    system("clear");
    printf("\n");
    printf("══════════════════════════\n");
    printf("--- Fin du tour %d ---\n", jeu->tourActuel);
    printf("══════════════════════════\n\n");
    
    Jeu_afficherTableau(&jeu->table);
    Jeu_afficherScores(jeu);
    
    sleep(4);
    system("clear");
    
    int all_empty = 1;
    for (int i = 0; i < jeu->nbJoueurs; i++) {
        if (jeu->joueurs[i].jeuCartes.nbCartes > 0) {
            all_empty = 0;
            break;
        }
    }
    
    if (all_empty && !Jeu_estTermine(jeu)) {
        Jeu_reinitialiserManche(jeu);
    }
}


int Jeu_estTermine(Jeu *jeu) {
    if (jeu == NULL) return 0;
    
    for (int i = 0; i < jeu->nbJoueurs; i++) {
        if (jeu->joueurs[i].score >= POINTS_LIMITE) {
            return 1;
        }
    }
    
    if (jeu->deck.nbCartes == 0) {
        int all_empty = 1;
        for (int i = 0; i < jeu->nbJoueurs; i++) {
            if (jeu->joueurs[i].jeuCartes.nbCartes > 0) {
                all_empty = 0;
                break;
            }
        }
        if (all_empty) return 1;
    }
    
    return 0;
}

void Jeu_afficherTableau(TableJeu *table) {
    if (table == NULL) return;
    
    printf("\n┌─────────────────────────────────────┐\n");
    printf("│ 🎮 TABLEAU DE JEU 🎮 │\n");
    printf("└─────────────────────────────────────┘\n\n");
    
    for (int i = 0; i < NB_RANGEES_JEU; i++) {
        Rangee *rangee = &table->rangees[i];
        int pts = Rangee_getTetesBoeuf(rangee);
        printf("Rangée %d [%d 🐮]:\n", i + 1, pts);
        
        if (rangee->nbCartes == 0) {
            printf("(vide)\n");
        } else {
            Collection c = Rangee_asCollection(rangee);
            char *rangee_str = Collection_toString(&c, 0);
            if (rangee_str) {
                printf("%s", rangee_str);
                free(rangee_str);
            }
        }
    }
}


void Jeu_afficherScores(Jeu *jeu) {
    if (jeu == NULL) return;
    
    printf("\n┌─────────────────────────────────────┐\n");
    printf("│ 📊 SCORES ACTUELS │\n");
    printf("└─────────────────────────────────────┘\n");
    
    for (int i = 0; i < jeu->nbJoueurs; i++) {
        printf(" %s : %d points\n", jeu->joueurs[i].nom, jeu->joueurs[i].score);
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
