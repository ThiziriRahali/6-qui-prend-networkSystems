BEGIN {
    print "============================================"
    print "STATISTIQUES DU JEU 6 QUI PREND"
    print "============================================"
    print ""
}

# Compter les parties
/=== NOUVEAU JEU/ { nb_parties++ }

# Compter les connexions par joueur
/\[CONNEXION\]/ {
    match($0, /Joueur '([^']+)'/, arr)
    joueur = arr[1]
    if (joueur != "") {
        connexions[joueur]++
        joueurs_unique[joueur] = 1
    }
}

# Compter les victoires et scores
/\[GAGNANT\]/ {
    match($0, /\[GAGNANT\] ([^ ]+) avec ([0-9]+)/, arr)
    gagnant = arr[1]
    score = arr[2]
    if (gagnant != "") {
        victoires[gagnant]++
        scores[gagnant] = score
        tous_scores[++count_scores] = score
    }
}

# Compter les tours par partie
/DEBUT DE PARTIE|=== NOUVEAU JEU/ { in_game = 1; tour_count = 0 }
/FIN DE PARTIE|=== FIN DU JEU/ {
    if (in_game && tour_count > 0) {
        tours[++count_tours] = tour_count
    }
    in_game = 0
}
in_game && /\[TOUR/ { tour_count++ }

# Chronologie
/=== NOUVEAU JEU/ {
    if (first_game == "") {
        first_game = $0
    }
    last_game = $0
}

# Bots vs Humains
/\[CONNEXION\] Joueur 'Bot/ {
    match($0, /Joueur '([^']+)'/, arr)
    bots[arr[1]] = 1
}

END {
    # [1] PARTIES
    print "[1] PARTIES"
    printf "  Nombre total de parties: %d\n", nb_parties
    print ""

    # [2] JOUEURS
    print "[2] JOUEURS ET CONNEXIONS"
    print "  Joueurs connectes:"
    for (j in connexions) {
        printf "    - %s: %d connexion(s)\n", j, connexions[j]
    }
    print ""

    # [3] GAGNANTS
    print "[3] GAGNANTS"
    print "  Top des gagnants:"
    for (g in victoires) {
        printf "    - %s: %d victoire(s)\n", g, victoires[g]
    }
    print ""

    # [4] SCORES
    print "[4] SCORES"
    print "  Analyse des points:"
    if (count_scores > 0) {
        sum = 0
        min = tous_scores[1]
        max = tous_scores[1]
        for (i = 1; i <= count_scores; i++) {
            sum += tous_scores[i]
            if (tous_scores[i] < min) min = tous_scores[i]
            if (tous_scores[i] > max) max = tous_scores[i]
        }
        avg = sum / count_scores
        printf "    Score moyen du gagnant: %.1f\n", avg
        printf "    Score min: %d\n", min
        printf "    Score max: %d\n", max
    } else {
        print "    Aucun score valide trouve"
    }
    print ""

    # [5] TOURS
    print "[5] TOURS"
    print "  Analyse des tours:"
    if (count_tours > 0) {
        sum = 0
        min = tours[1]
        max = tours[1]
        for (i = 1; i <= count_tours; i++) {
            sum += tours[i]
            if (tours[i] < min) min = tours[i]
            if (tours[i] > max) max = tours[i]
        }
        avg = sum / count_tours
        printf "    Nombre moyen de tours: %.1f\n", avg
        printf "    Min tours: %d\n", min
        printf "    Max tours: %d\n", max
    } else {
        print "    Aucun donnees de tours trouvees"
    }
    print ""

    # [6] CHRONOLOGIE
    print "[6] CHRONOLOGIE"
    if (first_game != "") {
        sub(/^.*=== NOUVEAU JEU - /, "", first_game)
        printf "  Premiere partie: %s\n", first_game
    } else {
        print "  Premiere partie: (aucune partie trouvee)"
    }
    if (last_game != "") {
        sub(/^.*=== NOUVEAU JEU - /, "", last_game)
        printf "  Derniere partie: %s\n", last_game
    } else {
        print "  Derniere partie: (aucune partie trouvee)"
    }
    print ""

    # [7] BOTS
    print "[7] BOTS"
    nb_humains = 0
    nb_bots_count = 0
    for (j in joueurs_unique) {
        if (j in bots) {
            nb_bots_count++
        } else {
            nb_humains++
        }
    }
    printf "  Joueurs uniques (humains): %d\n", nb_humains
    printf "  Joueurs uniques (bots): %d\n", nb_bots_count
    print ""

    # [8] DETAILS PAR JOUEUR
    print "[8] DETAILS PAR JOUEUR"
    print "  Victoires et scores finaux:"
    for (g in victoires) {
        printf "    %s: %s points\n", g, scores[g]
    }

    print ""
    print "============================================"
    print "Fin des statistiques"
    print "============================================"
}
