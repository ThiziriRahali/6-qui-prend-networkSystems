# Compilateur et options
CC       = gcc
CFLAGS   = -Wall -Wextra -Wpedantic -std=c11 -g
LDFLAGS  = -pthread

# Fichiers sources communs (utilises par serveur et client)
SRC_COMMON = \
    Carte.c \
    Collection.c \
    Joueur.c \
    GestionnaireJeu.c \
    global.c \
    template.c \
    jeu.c \
    logging.c

# Fichiers sources specifiques au serveur
SRC_SERVER = Serveur.c client_handler.c server_communication.c

# Fichiers sources specifiques au client
SRC_CLIENT = client.c

# Fichiers sources specifiques au robot
SRC_ROBOT = robot.c robot_strategies.c

# Objets
OBJ_COMMON = $(SRC_COMMON:.c=.o)
OBJ_SERVER = $(SRC_SERVER:.c=.o)
OBJ_CLIENT = $(SRC_CLIENT:.c=.o)
OBJ_ROBOT = $(SRC_ROBOT:.c=.o)

# Binaires
TARGETS = serveur client robot

# Cible par defaut
all: $(TARGETS) scripts

# Rendre les scripts executables
scripts:
	chmod +x stats.sh 2>/dev/null || true

# Binaire serveur
serveur: $(OBJ_COMMON) $(OBJ_SERVER)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Binaire client
client: $(OBJ_COMMON) $(OBJ_CLIENT)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Binaire robot
robot: $(OBJ_COMMON) $(OBJ_ROBOT)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Regle generique pour compiler les .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Dependencies (generation automatique)
%.d: %.c
	$(CC) -MM $(CFLAGS) $< > $@

# Inclure les fichiers de dependances
-include $(SRC_COMMON:.c=.d) $(SRC_SERVER:.c=.d) $(SRC_CLIENT:.c=.d) $(SRC_ROBOT:.c=.d)

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

# Nettoyage complet (+ objets inutiles)
distclean: clean
	rm -f *.o *.d jeu.log

.PHONY: all clean distclean scripts stats-shell
