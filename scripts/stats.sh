#!/bin/bash

# Script d'analyse des statistiques de jeu
# Parse le fichier jeu.log et génère des statistiques

LOG_FILE="logs/jeu.log"

if [ ! -f "$LOG_FILE" ]; then
    echo "❌ Fichier $LOG_FILE introuvable"
    exit 1
fi

echo "📊 Statistiques du jeu"
echo "====================="
echo ""

echo "🎮 Parties jouées:"
grep -c "\[PARTIE COMMENCEE\]" "$LOG_FILE" || echo "0"

echo ""
echo "👥 Joueurs connectés:"
grep "\[JOUEUR CONNECTE\]" "$LOG_FILE" | wc -l

echo ""
echo "🏆 Gagnants:"
grep "\[PARTIE TERMINEE\]" "$LOG_FILE" | sed 's/.*Gagnant: //' | sed 's/ .*//' || echo "Aucun"

echo ""
echo "📈 Résumé complet:"
awk -f stats.awk "$LOG_FILE"
