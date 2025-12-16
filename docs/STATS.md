# Générateur de Statistiques - 6 Qui Prend

## Vue d'ensemble

Deux scripts sont disponibles pour générer des statistiques à partir des logs de jeu (`jeu.log`) :

1. **`stats.awk`** - Version AWK (rapide et efficace)
2. **`stats.sh`** - Version Shell (plus détaillée)

## Utilisation

### Avec AWK (recommandé)

```bash
# Via le Makefile
make stats

# Ou directement
awk -f stats.awk jeu.log
```

### Avec le script Shell

```bash
# Via le Makefile
make stats-shell

# Ou directement
./stats.sh jeu.log
./stats.sh        # utilise jeu.log par défaut
```

### Exemple de sortie

```
=============================================
STATISTIQUES DU JEU 6 QUI PREND
=============================================

[RESUME]
  Nombre de parties: 5
  Nombre de joueurs uniques: 4
  Nombre de bots: 3

[JOUEURS - CONNEXIONS]
  Alice: 5 connexion(s)
  Robot1: 5 connexion(s)
  Bot1: 3 connexion(s)
  Bot2: 2 connexion(s)

[GAGNANTS]
  Alice: 2 victoire(s) (dernier score: 18 points)
  Robot1: 2 victoire(s) (dernier score: 22 points)
  Bot1: 1 victoire(s) (dernier score: 25 points)

[SCORES]
  Score moyen: 21.5
  Score min: 18
  Score max: 32

[TOURS PAR PARTIE]
  Tours moyen par partie: 8.4
  Min tours: 5
  Max tours: 12

=============================================
```

## Statistiques calculées

### Résumé général
- Nombre total de parties
- Nombre de joueurs uniques
- Nombre de bots

### Joueurs
- Connexions par joueur
- Nombre de victoires
- Score au dernier jeu

### Scores
- Score moyen des gagnants
- Score minimum
- Score maximum

### Tours
- Nombre moyen de tours par partie
- Nombre minimum de tours
- Nombre maximum de tours

## Fichier jeu.log

Le fichier `jeu.log` est généré automatiquement lors de chaque partie. Il contient :

- **Connexions** : `[CONNEXION] Joueur 'Alice' depuis 127.0.0.1:54321`
- **Parties** : `[PARTIE] Lancement avec 2 joueurs: Alice, Bob`
- **Tours** : `[TOUR 1] Alice joue carte #42 -> Rangée 2`
- **Scores** : scores de chaque joueur
- **Gagnants** : `[GAGNANT] Alice avec 15 points!`

## Filtrer les données

Tu peux aussi combiner avec d'autres commandes :

```bash
# Voir toutes les connexions
grep "\[CONNEXION\]" jeu.log

# Voir tous les gagnants
grep "\[GAGNANT\]" jeu.log

# Compter les parties
grep -c "=== NOUVEAU JEU" jeu.log

# Voir les scores finaux
grep "\[GAGNANT\]" jeu.log | awk '{print $(NF-1), $NF}'

# Extraire les 10 dernières lignes du log
tail -10 jeu.log
```

## Intégration Makefile

Des cibles pratiques ont été ajoutées :

```bash
make stats         # Affiche stats avec AWK
make stats-shell   # Affiche stats avec le script shell
make distclean     # Nettoie tout y compris jeu.log
```

## Personnalisation

Tu peux modifier les scripts pour ajouter d'autres statistiques :

- **stats.awk** : Facile à étendre avec des patterns AWK
- **stats.sh** : Combine grep, awk, sed pour plus de flexibilité

## Dépannage

**Erreur : "fichier jeu.log non trouvé"**
- Assure-toi que le serveur a exécuté au moins une partie
- Le log est créé dans le répertoire courant

**Les stats ne sont pas à jour**
- Le log s'accumule au fil du temps
- Utilise `make distclean` pour recommencer
- Ou manuellement : `rm jeu.log`

**Problème de permissions sur stats.sh**
```bash
chmod +x stats.sh
```

## Notes

- Les stats incluent aussi les bots
- Les scores affichés sont ceux des gagnants (plus bas = mieux)
- Les statistiques sont cumulatives depuis le dernier nettoyage
