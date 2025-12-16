BEGIN {
    print ""
    print "=== STATISTIQUES FINALES ==="
    total_joueurs = 0
    total_parties = 0
    total_manches = 0
}

/\[JOUEUR CONNECTE\]/ {
    total_joueurs++
}

/\[PARTIE COMMENCEE\]/ {
    total_parties++
}

/\[MANCHE.*TERMINEE\]/ {
    total_manches++
}

/\[PARTIE TERMINEE\]/ {
    match($0, /Gagnant: ([^ ]+)/, arr)
    if (arr[1]) {
        gagnants[arr[1]]++
        total_gagnants++
    }
}

END {
    print ""
    print "Joueurs connects: " total_joueurs
    print "Parties lancees: " total_parties
    print "Manches jouees: " total_manches
    print ""
    print "=== VICTOIRES ==="
    if (total_gagnants > 0) {
        for (joueur in gagnants) {
            printf "%s: %d victoire(s)\n", joueur, gagnants[joueur]
        }
    } else {
        print "Aucune partie terminee"
    }
    print ""
}
