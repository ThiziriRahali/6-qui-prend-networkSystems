#!/usr/bin/awk -f
# Script AWK pour generer des statistiques du jeu.log
# Usage: awk -f stats.awk jeu.log


BEGIN {
    FS = " "
    nb_parties = 0
    nb_tours = 0
    in_game = 0
}

# Detecter debut de partie
/=== NOUVEAU JEU/ {
    nb_parties++
    in_game = 1
    current_game = nb_parties
    tour_count[current_game] = 0
}

# Detecter fin de partie
/=== FIN DU JEU/ {
    in_game = 0
}

# Compter les connexions
/\[CONNEXION\] Joueur/ {
    match($0, /Joueur '([^']+)'/, arr)
    joueur = arr[1]
    connexions[joueur]++
    all_players[joueur] = 1
}

# Compter les bots
/\[CONNEXION\] Joueur 'Bot/ {
    nb_bots++
}

# Compter les tours
if (in_game && /\[TOUR [0-9]+\]/) {
    tour_count[current_game]++
}

# Extraire les gagnants et leurs scores
/\[GAGNANT\]/ {
    match($0, /Gagnant: ([^ ]+)/, arr)
    winner = arr[1]
    match($0, /([0-9]+) points!/, arr2)
    score = arr2[1]
    
    gagnants[winner]++
    scores_gagnants[winner] = score
    
    all_scores[score] = winner
}

END {
    print "============================================="
    print "STATISTIQUES DU JEU 6 QUI PREND"
    print "============================================="
    print ""
    
    # 1. Resume
    print "[RESUME]"
    print "  Nombre de parties: " nb_parties
    print "  Nombre de joueurs uniques: " length(all_players)
    print "  Nombre de bots: " nb_bots
    print ""
    
    # 2. Top joueurs par connexions
    print "[JOUEURS - CONNEXIONS]"
    for (joueur in connexions) {
        joueurs_list[joueur] = connexions[joueur]
    }
    
    # Tri simple (bubblesort sur le nombre de connexions)
    n = 1
    for (joueur in joueurs_list) {
        sorted[n] = joueur
        n++
    }
    
    for (i = 1; i <= n; i++) {
        for (j = i + 1; j <= n; j++) {
            if (joueurs_list[sorted[i]] < joueurs_list[sorted[j]]) {
                temp = sorted[i]
                sorted[i] = sorted[j]
                sorted[j] = temp
            }
        }
    }
    
    for (i = 1; i <= n; i++) {
        if (sorted[i] != "") {
            printf("  %s: %d connexion(s)\n", sorted[i], joueurs_list[sorted[i]])
        }
    }
    print ""
    
    # 3. Top gagnants
    print "[GAGNANTS]"
    for (joueur in gagnants) {
        printf("  %s: %d victoire(s) (dernier score: %d points)\n", joueur, gagnants[joueur], scores_gagnants[joueur])
    }
    print ""
    
    # 4. Scores
    print "[SCORES]"
    min_score = 999999
    max_score = 0
    sum_score = 0
    count_score = 0
    
    for (score in all_scores) {
        if (score < min_score) min_score = score
        if (score > max_score) max_score = score
        sum_score += score
        count_score++
    }
    
    if (count_score > 0) {
        avg_score = sum_score / count_score
        printf("  Score moyen: %.1f\n", avg_score)
        printf("  Score min: %d\n", min_score)
        printf("  Score max: %d\n", max_score)
    }
    print ""
    
    # 5. Nombre de tours
    print "[TOURS PAR PARTIE]"
    min_tours = 999999
    max_tours = 0
    sum_tours = 0
    
    for (i = 1; i <= nb_parties; i++) {
        if (tour_count[i] > 0) {
            if (tour_count[i] < min_tours) min_tours = tour_count[i]
            if (tour_count[i] > max_tours) max_tours = tour_count[i]
            sum_tours += tour_count[i]
        }
    }
    
    if (nb_parties > 0) {
        avg_tours = sum_tours / nb_parties
        printf("  Tours moyen par partie: %.1f\n", avg_tours)
        printf("  Min tours: %d\n", min_tours)
        printf("  Max tours: %d\n", max_tours)
    }
    print ""
    
    print "============================================="
}
