# Guide d'Exécution - 6 Qui Prend Network

## Compilation

```bash
make clean
make
```

Cela génère deux binaires :
- `serveur` : serveur de jeu
- `client` : client joueur

## Lancement du Serveur

### Syntaxe
```bash
./serveur <adresse_ip> <port> <nb_joueurs_max>
```

### Paramètres
- `<adresse_ip>` : IP sur laquelle le serveur écoute
- `<port>` : port TCP (1-65535)
- `<nb_joueurs_max>` : nombre de joueurs max (2-10). La partie lance quand ce nombre est atteint OU quand le timer expire

### Exemples

**Sur localhost avec max 4 joueurs :**
```bash
./serveur 127.0.0.1 4242 4
```

**Sur une interface réseau spécifique :**
```bash
./serveur 192.168.1.100 4242 3
```

**Sur toutes les interfaces (0.0.0.0) :**
```bash
./serveur 0.0.0.0 4242 6
```

## Comportement du Serveur

1. **Attente du premier joueur** : Le serveur attend passivement
2. **Dès le premier joueur connecté** : Un **timer de 30 secondes** se lance (⏱️)
3. **Deux scénarios possibles** :
   - ✅ Le nombre max de joueurs est atteint → Lancement immédiat
   - ⏱️ Le timer expire → Lancement avec les joueurs présents + **bots pour atteindre le nombre max de joueurs**

### Exemple de sortie serveur

```
🎮 Serveur lancé sur 127.0.0.1:4242
Maximum de joueurs: 4
En attente de joueurs...
⏱️  Une fois le premier joueur connecté, un timer de 30 secondes démarre.
   La partie lancera avec les joueurs connectés + des bots (si nécessaire)

✅ Nouveau joueur connecté: Alice depuis 127.0.0.1:54321
Joueurs connectés: 1/4
⏱️  TIMER LANCÉ (30 secondes avant lancement auto)

✅ Nouveau joueur connecté: Bob depuis 127.0.0.1:54322
Joueurs connectés: 2/4
⏱️  10 secondes avant lancement auto...
⏱️  5 secondes avant lancement auto...

🤖 Ajout de 2 bot(s) pour atteindre 4 joueurs
  Bot 1 connecté: Bot1
  Bot 2 connecté: Bot2

🎮 LANCEMENT DE LA PARTIE (timer écoulé) avec 4 joueurs!
```

## Architecture des Bots

### 🤖 Threads Dédiés

Chaque bot dispose d'un **thread dédié** qui gère son comportement de jeu :

- **1 bot = 1 thread** (si 3 bots dans la partie → 3 threads créés)
- Chaque thread bot est créé au début de **chaque tour**
- Le thread joue la carte du bot puis se termine (`pthread_join`)
- **Synchronisation** : Les threads humains et bots jouent en parallèle, puis le serveur attend que tous aient fini avant de révéler les cartes

### Stratégie IA Simple

Les bots utilisent un algorithme glouton basique :
- **Choix de carte** : Joue toujours sa **plus petite carte** (index 0 après tri)
- **Choix de rangée** (si carte trop petite) : Sélectionne la rangée avec le **moins de points de pénalité**

### Logs de Debug

Pendant le jeu, vous verrez :
```
🤖 [Thread Bot1] Démarré (PID thread: 140234567890)
🤖 [Thread Bot1] Joue la carte 15
✅ [Thread Bot1] Carte 15 enregistrée et retirée
✅ Bot thread 0 terminé
```

## Lancement des Clients

### Syntaxe
```bash
./client <adresse_ip_serveur> <port_serveur> <nom_joueur>
```

### Paramètres
- `<adresse_ip_serveur>` : IP du serveur
- `<port_serveur>` : port du serveur (doit correspondre)
- `<nom_joueur>` : nom du joueur (1-31 caractères)

### Exemples

**Alice se connecte :**
```bash
./client 127.0.0.1 4242 Alice
```

**Bob se connecte :**
```bash
./client 127.0.0.1 4242 Bob
```

**Charlie se connecte (crée un 3e joueur) :**
```bash
./client 127.0.0.1 4242 Charlie
```

## Scénarios de Test

### Scénario 1 : 1 joueur humain + 3 bots (max 4)

**Terminal 1 - Serveur (max 4 joueurs) :**
```bash
./serveur 127.0.0.1 4242 4
```

**Terminal 2 - Alice :**
```bash
./client 127.0.0.1 4242 Alice
```

*[Attendre 30 secondes - Personne d'autre ne se connecte]*

→ La partie se lance avec : **Alice + Bot1 + Bot2 + Bot3** (4 joueurs)

### Scénario 2 : 2 joueurs humains + 2 bots (max 4)

**Terminal 1 - Serveur (max 4 joueurs) :**
```bash
./serveur 127.0.0.1 4242 4
```

**Terminal 2 - Alice :**
```bash
./client 127.0.0.1 4242 Alice
```

**Terminal 3 - Bob (dans les 30 sec) :**
```bash
./client 127.0.0.1 4242 Bob
```

*[Attendre que le timer expire]*

→ La partie se lance avec : **Alice + Bob + Bot1 + Bot2** (4 joueurs)

### Scénario 3 : Lancer au max de joueurs (sans timer)

**Terminal 1 - Serveur (max 3 joueurs) :**
```bash
./serveur 127.0.0.1 4242 3
```

**Terminal 2 - Alice :**
```bash
./client 127.0.0.1 4242 Alice
```

**Terminal 3 - Bob :**
```bash
./client 127.0.0.1 4242 Bob
```

**Terminal 4 - Charlie (dans les 30 sec) :**
```bash
./client 127.0.0.1 4242 Charlie
```

Dès que Charlie se connecte (3 = max) → **Lancement immédiat** de la partie avec Alice + Bob + Charlie (aucun bot)

### Scénario 4 : Partie à 2 joueurs (minimum)

**Terminal 1 - Serveur (max 2 joueurs) :**
```bash
./serveur 127.0.0.1 4242 2
```

**Terminal 2 - Alice :**
```bash
./client 127.0.0.1 4242 Alice
```

**Terminal 3 - Bob (dans les 30 sec) :**
```bash
./client 127.0.0.1 4242 Bob
```

Dès que Bob se connecte (2 = max) → Lancement immédiat avec Alice + Bob (aucun bot)

## Fichier de Log

Chaque partie est enregistrée dans `logs/jeu.log` avec :

- **Connexions** : `[CONNEXION] Joueur 'Alice' depuis 127.0.0.1:54321`
- **Connexions bots** : `[CONNEXION] Joueur 'Bot1' depuis 127.0.0.1:0`
- **Début de partie** : `[PARTIE] Lancement avec 4 joueurs: Alice, Bot1, Bot2, Bot3`
- **Fin de partie** : scores finaux et gagnant

### Consulter les logs

```bash
cat logs/jeu.log
tail -f logs/jeu.log      # en temps réel
```

### Générer des statistiques

```bash
make stats-shell
```

Cela exécute le script `logs/stats.sh` qui analyse `logs/jeu.log` et affiche :
- Nombre total de parties jouées
- Taux de victoire par joueur
- Statistiques des bots
- Plus grand nombre de points dans une partie

## Architecture Technique

### Threads

- **Thread principal** : Gère les connexions entrantes
- **Thread timer** : Lance le compte à rebours de 30 secondes
- **Thread partie** : Gère la logique du jeu
- **Threads bots** : 1 thread par bot par tour (créés et détruits dynamiquement)

### Synchronisation

- **Mutex** : Protection des ressources partagées (clients connectés, cartes jouées)
- **pthread_join** : Attente de fin des threads bots avant révélation des cartes

### Compilation avec POSIX

Le projet utilise `-D_XOPEN_SOURCE=700` pour accéder aux extensions POSIX (threads, barrières, etc.)

## Nettoyage

```bash
make clean        # Supprime objets et exécutables
make distclean    # Supprime tout + logs
```

## Notes Importantes

- Le **nombre de joueurs au lancement** est **toujours égal à `nb_joueurs_max`**
- Les bots comblent **jusqu'à `nb_joueurs_max`** (et non plus jusqu'à `MIN_JOUEURS`)
- Le **timeout du timer** est fixé à **30 secondes** (défini comme `TIMEOUT_TIMER` dans `defines.h`)
- Les bots sont identifiés par le nom `Bot1`, `Bot2`, etc.
- Chaque bot possède son propre thread d'exécution pendant les tours de jeu

## Dépannage

### Le serveur ne se lance pas
```bash
# Vérifier que le port n'est pas déjà utilisé
netstat -tuln | grep 4242

# Tuer le processus si nécessaire
kill -9 $(lsof -t -i:4242)
```

### Les bots ne jouent pas
```bash
# Vérifier que la compilation inclut les flags POSIX
make clean
make

# Vérifier les logs de debug dans la sortie du serveur
```

### La partie ne se lance jamais
```bash
# Vérifier que nb_joueurs_max >= MIN_JOUEURS (2)
./serveur 127.0.0.1 4242 2  # minimum
```

## Contributeurs

Ce projet a été développé dans le cadre d'un cours de systèmes réseaux avec l'implémentation de :
- Architecture client-serveur TCP
- Gestion multi-threads avec synchronisation
- IA simple pour bots autonomes
- Logging et analyse de statistiques
