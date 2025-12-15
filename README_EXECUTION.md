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
./serveur <adresse_ip> <port>
```

### Exemples

**Sur la machine locale (localhost) :**
```bash
./serveur 127.0.0.1 4242
```

**Sur une interface réseau spécifique :**
```bash
./serveur 192.168.1.100 4242
```

**Sur toutes les interfaces (0.0.0.0) :**
```bash
./serveur 0.0.0.0 4242
```

## Lancement des Clients

### Syntaxe
```bash
./client <adresse_ip_serveur> <port_serveur> <nom_joueur>
```

### Exemples

**Client 1 (Alice) :**
```bash
./client 127.0.0.1 4242 Alice
```

**Client 2 (Bob) :**
```bash
./client 127.0.0.1 4242 Bob
```

**Client 3 (Charlie) - dans un autre terminal :**
```bash
./client 127.0.0.1 4242 Charlie
```

## Scénario Complet de Test

### Terminal 1 - Démarrer le serveur
```bash
make clean && make
./serveur 127.0.0.1 4242
```

Vous verrez :
```
🎮 Serveur lancé sur 127.0.0.1:4242
En attente de 2 joueurs minimum...
```

### Terminal 2 - Connecter Alice
```bash
./client 127.0.0.1 4242 Alice
```

Vous verrez côté serveur :
```
✅ Nouveau joueur connecté: Alice depuis 127.0.0.1:12345
Joueurs connectés: 1/2
En attente de 2 joueurs minimum...
```

### Terminal 3 - Connecter Bob
```bash
./client 127.0.0.1 4242 Bob
```

Vous verrez côté serveur :
```
✅ Nouveau joueur connecté: Bob depuis 127.0.0.1:12346
Joueurs connectés: 2/2

🎮 LANCEMENT DE LA PARTIE avec 2 joueurs!
```

Et les clients verront :
```
La partie commence avec 2 joueurs!
```

## Fichier de Log

Chaque partie est enregistrée dans le fichier `jeu.log` avec :

- **Connexions** : `[CONNEXION] Joueur 'Alice' depuis 127.0.0.1:12345`
- **Début de partie** : `[PARTIE] Lancement avec 2 joueurs: Alice, Bob`
- **Fin de partie** : scores finaux et gagnant

### Exemple de contenu jeu.log

```
=== NOUVEAU JEU - Mon Dec 15 18:42:30 2025
[18:42:35] [CONNEXION] Joueur 'Alice' depuis 127.0.0.1:54321
[18:42:40] [CONNEXION] Joueur 'Bob' depuis 127.0.0.1:54322

--- DÉBUT DE PARTIE ---
[PARTIE] Lancement avec 2 joueurs: Alice, Bob

[18:42:42] [TOUR 1] Alice joue carte #42 -> Rangée 2 | Points acqu: 0
[18:42:42] [TOUR 1] Bob joue carte #35 -> Rangée 1 | Points acqu: 0

[MANCHE 1 TERMINÉE] Scores actuels:
  Joueur 1: 0 points
  Joueur 2: 0 points

--- FIN DE PARTIE ---
[GAGNANT] Alice avec 15 points!

Classement final:
  1. Alice: 15 points
  2. Bob: 28 points

=== FIN DU JEU ===
```

## Tester avec plus de joueurs

### Pour 4 joueurs

Terminal 1 : Serveur
```bash
./serveur 127.0.0.1 4242
```

Terminals 2-5 : Clients
```bash
./client 127.0.0.1 4242 Alice
./client 127.0.0.1 4242 Bob
./client 127.0.0.1 4242 Charlie
./client 127.0.0.1 4242 Diana
```

> **Note** : Le minimum de joueurs est 2, le maximum est 10 (configurable dans Serveur.c)

## Modifier les paramètres

Dans `Serveur.c`, ligne 14-15 :
```c
#define MIN_JOUEURS 2    // Nombre minimum de joueurs
#define MAX_JOUEURS 10   // Nombre maximum de joueurs
```

## Vérifier les logs

```bash
cat jeu.log
```

Ou en continu :
```bash
tail -f jeu.log
```

## Nettoyer

```bash
make distclean    # Supprime exécutables, objets ET jeu.log
```
