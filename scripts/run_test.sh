#!/bin/bash

# Script de test du jeu 6 qui prend
# Lance le serveur et les clients pour tester

set -e

echo "🚀 Compilation du projet..."
make clean
make

echo ""
echo "🎮 Lancement du serveur (4 joueurs max)..."
./serveur 127.0.0.1 4242 4 &
SERVER_PID=$!

sleep 2

echo "👥 Lancement des clients..."
for i in 1 2 3 4; do
    echo "  Client $i..."
    ./client 127.0.0.1 4242 "Joueur$i" &
    sleep 1
done

echo ""
echo "⏳ Laissez la partie se dérouler..."
echo "Appuyez sur Ctrl+C pour arrêter"

wait
