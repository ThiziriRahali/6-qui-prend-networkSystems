#!/bin/bash

# Script de test pour lancer serveur + clients

set -e  # Arréter si une commande échoue

# Vérifier si make est nécessaire
if [ ! -f "serveur" ] || [ ! -f "client" ]; then
    echo "🔨 Compilation en cours..."
    make clean && make
fi

echo "🎮 Script de test 6 Qui Prend Network"
echo "
================================"
echo "
Ce script va lancer :
1. Le serveur sur 127.0.0.1:4242
2. Deux clients de test (Alice et Bob)

Le serveur et les clients s'afficheront dans des nouveaux terminaux.

================================
"

# Configuration
SERVER_IP="127.0.0.1"
SERVER_PORT="4242"

# Fonction pour lancer le serveur
run_server() {
    echo "🎮 Démarrage du serveur sur $SERVER_IP:$SERVER_PORT..."
    ./serveur "$SERVER_IP" "$SERVER_PORT"
}

# Fonction pour lancer un client
run_client() {
    local name="$1"
    sleep 1  # Laisser le temps au serveur de démarrer
    echo "🎮 Démarrage du client: $name"
    ./client "$SERVER_IP" "$SERVER_PORT" "$name"
}

# Lancer dans des terminaux séparés
if command -v gnome-terminal &> /dev/null; then
    echo "Détecté: gnome-terminal"
    gnome-terminal --tab --title="Serveur" -- bash -c "$(declare -f run_server); run_server" \
                   --tab --title="Client Alice" -- bash -c "$(declare -f run_client); run_client Alice" \
                   --tab --title="Client Bob" -- bash -c "$(declare -f run_client); run_client Bob"
elif command -v xterm &> /dev/null; then
    echo "Détecté: xterm"
    xterm -T "Serveur" -e bash -c "$(declare -f run_server); run_server" &
    sleep 2
    xterm -T "Client Alice" -e bash -c "$(declare -f run_client); run_client Alice" &
    sleep 1
    xterm -T "Client Bob" -e bash -c "$(declare -f run_client); run_client Bob" &
elif command -v tmux &> /dev/null; then
    echo "Détecté: tmux"
    tmux new-session -d -s gametest
    tmux new-window -t gametest -n "Serveur"
    tmux new-window -t gametest -n "Alice"
    tmux new-window -t gametest -n "Bob"
    tmux send-keys -t gametest:Serveur "./serveur $SERVER_IP $SERVER_PORT" Enter
    sleep 2
    tmux send-keys -t gametest:Alice "./client $SERVER_IP $SERVER_PORT Alice" Enter
    sleep 1
    tmux send-keys -t gametest:Bob "./client $SERVER_IP $SERVER_PORT Bob" Enter
    tmux attach-session -t gametest
else
    echo "⚠️  Aucun terminal supporté détecté (gnome-terminal, xterm, tmux)"
    echo "
Lancement manuel en cours..."
    echo "
Terminal 1 : Serveur"
    echo "------"
    run_server &
    SERVER_PID=$!
    
    sleep 2
    
    echo "
Terminal 2 : Client Alice"
    echo "------"
    run_client "Alice" &
    
    sleep 1
    
    echo "
Terminal 3 : Client Bob"
    echo "------"
    run_client "Bob" &
    
    # Attendre que le serveur termine
    wait $SERVER_PID
fi

echo "
================================"
echo "✅ Test terminé!"
echo "Consultez jeu.log pour voir les logs de la partie"
echo "================================"
