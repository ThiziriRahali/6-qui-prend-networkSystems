# Compilateur et options
CC       = gcc
CFLAGS   = -Wall -Wextra -Wpedantic -std=c11 -g
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
SRC_SERVER = Serveur.c client_handler.c

# Fichiers sources spécifiques au client
SRC_CLIENT = client.c

# Fichiers sources spécifiques au robot
SRC_ROBOT = robot.c

# Objets
OBJ_COMMON = $(SRC_COMMON:.c=.o)
OBJ_SERVER = $(SRC_SERVER:.c=.o)
OBJ_CLIENT = $(SRC_CLIENT:.c=.o)
OBJ_ROBOT = $(SRC_ROBOT:.c=.o)

# Binaires
TARGETS = serveur client robot

# Cible par défaut
all: $(TARGETS)

# Binaire serveur
serveur: $(OBJ_COMMON) $(OBJ_SERVER)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Binaire client
client: $(OBJ_COMMON) $(OBJ_CLIENT)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Binaire robot
robot: $(OBJ_COMMON) $(OBJ_ROBOT)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Règle générique pour compiler les .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Dépendances (génération automatique)
%.d: %.c
	$(CC) -MM $(CFLAGS) $< > $@

# Inclure les fichiers de dépendances
-include $(SRC_COMMON:.c=.d) $(SRC_SERVER:.c=.d) $(SRC_CLIENT:.c=.d) $(SRC_ROBOT:.c=.d)

# Nettoyage
clean:
	rm -f *.o *.d $(TARGETS)

# Nettoyage complet (+ objets inutiles)
distclean: clean
	rm -f *.o *.d jeu.log

.PHONY: all clean distclean
