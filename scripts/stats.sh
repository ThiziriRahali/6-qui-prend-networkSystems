#!/bin/bash

# Script de generation de statistiques a partir de jeu.log
# Usage: ./stats.sh [jeu.log]

LOG_FILE="${1:-jeu.log}"

if [ ! -f "$LOG_FILE" ]; then
    echo "Erreur: fichier $LOG_FILE non trouve"
    exit 1
fi

echo "============================================"
echo "STATISTIQUES DU JEU 6 QUI PREND"
echo "============================================"
echo ""

# 1. Nombre total de parties
echo "[1] PARTIES"
NB_PARTIES=$(grep -c "DEBUT DE PARTIE\|=== NOUVEAU JEU" "$LOG_FILE")
echo "  Nombre total de parties: $NB_PARTIES"
echo ""

# 2. Joueurs et leurs stats
echo "[2] JOUEURS ET CONNEXIONS"
echo "  Joueurs connectes:"
grep "\[CONNEXION\]" "$LOG_FILE" | awk -F"'" '{print $2}' | sort | uniq -c | sort -rn | while read count name; do
    if [ -n "$name" ]; then
        echo "    - $name: $count connexion(s)"
    fi
done
echo ""

# 3. Gagnants
echo "[3] GAGNANTS"
echo "  Top des gagnants:"
grep "\[GAGNANT\]" "$LOG_FILE" | awk '{print $2}' | sort | uniq -c | sort -rn | while read count name; do
    if [ -n "$name" ] && [ "$name" != "avec" ]; then
        echo "    - $name: $count victoire(s)"
    fi
done
echo ""

# 4. Scores moyens
echo "[4] SCORES"
echo "  Analyse des points:"

# Extraire tous les scores finaux
grep "\[GAGNANT\]" "$LOG_FILE" | awk '{print $(NF-1)}' | grep -E '^[0-9]+$' | awk '
    BEGIN { sum = 0; count = 0; min = 999999; max = 0 }
    {
        sum += $1
        count++
        if ($1 < min) min = $1
        if ($1 > max) max = $1
    }
    END {
        if (count > 0) {
            avg = sum / count
            printf("    Score moyen du gagnant: %.1f\n", avg)
            printf("    Score min: %d\n", min)
            printf("    Score max: %d\n", max)
        } else {
            printf("    Aucun score valide trouve\n")
        }
    }
'
echo ""

# 5. Nombre de tours par partie
echo "[5] TOURS"
echo "  Analyse des tours:"

awk '
    /DEBUT DE PARTIE|=== NOUVEAU JEU/ { in_game = 1; tour_count = 0; next }
    /FIN DE PARTIE|=== FIN DU JEU/ { in_game = 0; if (tour_count > 0) print tour_count; next }
    in_game && /\[TOUR/ { tour_count++ }
' "$LOG_FILE" | awk '
    BEGIN { sum = 0; count = 0; min = 999999; max = 0 }
    {
        sum += $1
        count++
        if ($1 < min) min = $1
        if ($1 > max) max = $1
    }
    END {
        if (count > 0) {
            avg = sum / count
            printf("    Nombre moyen de tours: %.1f\n", avg)
            printf("    Min tours: %d\n", min)
            printf("    Max tours: %d\n", max)
        }
    }
'
echo ""

# 6. Temps des parties
echo "[6] CHRONOLOGIE"
echo "  Premiere partie: $(grep "DEBUT DE PARTIE\|=== NOUVEAU JEU" "$LOG_FILE" | head -1 | awk '{print $1, $2}')"
echo "  Derniere partie: $(grep "DEBUT DE PARTIE\|=== NOUVEAU JEU" "$LOG_FILE" | tail -1 | awk '{print $1, $2}')"
echo ""

# 7. Bots vs Humains
echo "[7] BOTS"
NB_BOTS=$(grep "\[CONNEXION\] Joueur 'Bot" "$LOG_FILE" | wc -l)
NB_HUMAINS=$(grep "\[CONNEXION\]" "$LOG_FILE" | grep -v "Joueur 'Bot" | wc -l)
echo "  Connexions humaines: $NB_HUMAINS"
echo "  Connexions bots: $NB_BOTS"
echo ""

# 8. Statistiques detaillees par joueur (optionnel)
echo "[8] DETAILS PAR JOUEUR"
echo "  Victoires et scores finaux:"
grep "\[GAGNANT\]" "$LOG_FILE" | awk '
    {
        winner = $2
        score = $(NF-1)
        printf("    %s: %s points\n", winner, score)
    }
' | sort | uniq

echo ""
echo "============================================"
echo "Fin des statistiques"
echo "============================================"
