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
   - ⏱️ Le timer expire → Lancement avec les joueurs présents + bots pour atteindre le minimum (2 joueurs)

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
⏱️  0 secondes avant lancement auto...

🤖 Ajout de 0 bot(s)
🎮 LANCEMENT DE LA PARTIE (timer écoulé) avec 2 joueurs!
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

### Scénario 1 : Attendre le timer (2 joueurs humains)

**Terminal 1 - Serveur (max 4 joueurs) :**
```bash
./serveur 127.0.0.1 4242 4
```

**Terminal 2 - Alice :**
```bash
./client 127.0.0.1 4242 Alice
```

*[Attend le timer]* → Au bout de 30 secondes, la partie se lance avec Alice + 1 bot

**Terminal 3 - Bob (optionnel, avant les 30 sec) :**
```bash
./client 127.0.0.1 4242 Bob
```

Si Bob se connecte dans les 30 secondes → la partie se lance avec Alice + Bob (pas de bot)

### Scénario 2 : Lancer au max de joueurs

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

Dès que Bob se connecte (2 = max) → Lancement immédiat de la partie (sans attendre le timer)

### Scénario 3 : Comblement de bots

**Terminal 1 - Serveur (max 3 joueurs) :**
```bash
./serveur 127.0.0.1 4242 3
```

**Terminal 2 - Alice :**
```bash
./client 127.0.0.1 4242 Alice
```

*[Attendre 30 secondes - Bob ne se connecte pas]*

La partie se lance avec :
- Alice (joueur humain)
- Bot1 (créé auto pour atteindre le minimum de 2 joueurs)

## Fichier de Log

Chaque partie est enregistrée dans `jeu.log` avec :

- **Connexions** : `[CONNEXION] Joueur 'Alice' depuis 127.0.0.1:54321`
- **Début de partie** : `[PARTIE] Lancement avec 2 joueurs: Alice, Bob`
- **Fin de partie** : scores finaux et gagnant

### Consulter les logs

```bash
cat jeu.log
tail -f jeu.log      # en temps réel
```

## Nettoyage

```bash
make distclean    # Supprime exécutables, objets, fichiers .d ET jeu.log
```

## Notes

- Le **minimum de joueurs** est fixé à **2** (défini comme `MIN_JOUEURS` dans Serveur.c)
- Le **timeout du timer** est fixé à **30 secondes** (défini comme `TIMEOUT_TIMER` dans Serveur.c)
- Les **bots** se connectent automatiquement au timer pour combler jusqu'au minimum
- Les bots sont identifiés par le nom `Bot1`, `Bot2`, etc.
