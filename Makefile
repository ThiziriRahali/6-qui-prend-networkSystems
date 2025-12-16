# Compilateur et options
CC       = gcc
CFLAGS   = -Wall -Wextra -Wpedantic -std=c11 -g -MMD -MP
LDFLAGS  = -pthread

# Fichiers sources communs (utilisés par serveur et client)
SRC_COMMON = \
    Carte.c \
    Collection.c \
    Joueur.c \
    GestionnaireJeu.c \
    global.c \
    template.c \
    jeu.c \
    logging.c

# Fichiers sources spécifiques au serveur
SRC_SERVER = Serveur.c client_handler.c server_communication.c

# Fichiers sources spécifiques au client
SRC_CLIENT = client.c

# Objets
OBJ_COMMON = $(SRC_COMMON:.c=.o)
OBJ_SERVER = $(SRC_SERVER:.c=.o)
OBJ_CLIENT = $(SRC_CLIENT:.c=.o)

DEPS = $(OBJ_COMMON:.o=.d) $(OBJ_SERVER:.o=.d) $(OBJ_CLIENT:.o=.d)

# Binaires
TARGETS = serveur client

# Cible par défaut
all: $(TARGETS) scripts

# Rendre les scripts exécutables
scripts:
	chmod +x stats.sh 2>/dev/null || true

# Binaire serveur
serveur: $(OBJ_COMMON) $(OBJ_SERVER)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Binaire client
client: $(OBJ_COMMON) $(OBJ_CLIENT)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Règle générique pour compiler les .c en .o + .d
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Inclure les fichiers de dépendances (silencieusement)
-include $(DEPS)

# Cible pour afficher les stats avec le script shell
stats-shell:
	@if [ -f jeu.log ]; then \
		./stats.sh jeu.log; \
	else \
		echo "Erreur: fichier jeu.log non trouve"; \
		echo "Remarque: Joue quelques parties pour generer jeu.log"; \
	fi

# Nettoyage
clean:
	rm -f *.o *.d $(TARGETS)

# Nettoyage complet
distclean: clean
	rm -f jeu.log

.PHONY: all clean distclean scripts stats-shell
