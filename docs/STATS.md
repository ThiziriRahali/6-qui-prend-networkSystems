# Statistiques du Jeu 6 qui Prend

## Résumé

Ce document décrit les statistiques collectées durant les parties du jeu 6 qui prend en réseau.

## Fichiers de logs

- **logs/jeu.log** : Fichier principal contenant tous les événements du jeu

## Événements tracés

### Connexion des joueurs
```
[JOUEUR CONNECTE] <nom> from <ip>:<port>
```

### Début de partie
```
[PARTIE COMMENCEE] <nb_joueurs> joueurs:
  - Joueur1
  - Joueur2
  ...
```

### Cartes placées
```
[TOUR X] <joueur> joue <numero_carte> sur rangee <num> (score: <points>)
```

### Fin de manche
```
[MANCHE X TERMINEE] Scores: <score1> <score2> ...
```

### Fin de partie
```
[PARTIE TERMINEE] Gagnant: <nom> (<score> pts)
  - Joueur1: score1
  - Joueur2: score2
  ...
```

## Scripts d'analyse

### run_test.sh
Lance une partie de test automatiquement avec 4 clients.

```bash
bash scripts/run_test.sh
```

### stats.sh
Analyse le fichier jeu.log et affiche les statistiques.

```bash
bash scripts/stats.sh
```

## Formats de données

### Scores
Le score final d'une partie est la somme des "têtes de boeuf" de toutes les cartes récupérées.

### Rangées
Chaque carte est placée sur l'une des 4 rangées du plateau.

## Visualisation

Les statistiques peuvent être générées avec :
```bash
make stats
```
