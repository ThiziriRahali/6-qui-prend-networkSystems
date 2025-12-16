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
NB_PARTIES=$(grep -c "DÉBUT DE PARTIE\|=== NOUVEAU JEU" "$LOG_FILE")
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
grep "\[GAGNANT\]" "$LOG_FILE" | awk -F' ' '{print $2}' | sort | uniq -c | sort -rn | while read count name; do
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

# 5. Temps des parties
echo "[5] CHRONOLOGIE"
# Chercher la première partie avec la date/heure complète après "NOUVEAU JEU"
FIRST_GAME=$(grep "=== NOUVEAU JEU" "$LOG_FILE" | head -1)
if [ -n "$FIRST_GAME" ]; then
    # Extraire tout après "NOUVEAU JEU - "
    FIRST_TIME=$(echo "$FIRST_GAME" | sed 's/.*NOUVEAU JEU - //')
    echo "  Premiere partie: $FIRST_TIME"
else
    echo "  Premiere partie: (aucune partie trouvee)"
fi

LAST_GAME=$(grep "=== NOUVEAU JEU" "$LOG_FILE" | tail -1)
if [ -n "$LAST_GAME" ]; then
    LAST_TIME=$(echo "$LAST_GAME" | sed 's/.*NOUVEAU JEU - //')
    echo "  Derniere partie: $LAST_TIME"
else
    echo "  Derniere partie: (aucune partie trouvee)"
fi
echo ""

# 6. Bots vs Humains
echo "[6] BOTS"
# Compter les joueurs uniques humains et bots
NB_HUMAINS=$(grep "\[CONNEXION\]" "$LOG_FILE" | grep -v "Joueur 'Bot" | awk -F"'" '{print $2}' | sort -u | wc -l)
NB_BOTS=$(grep "\[CONNEXION\] Joueur 'Bot" "$LOG_FILE" | awk -F"'" '{print $2}' | sort -u | wc -l)
echo "  Joueurs uniques (humains): $NB_HUMAINS"
echo "  Joueurs uniques (bots): $NB_BOTS"
echo ""

# 7. Statistiques detaillees par joueur (optionnel)
echo "[7] DETAILS PAR JOUEUR"
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
