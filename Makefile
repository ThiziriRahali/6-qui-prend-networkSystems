# Compilateur et flags
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -g
LDFLAGS = -pthread

# Fichiers serveur
# Fichiers serveur
SERVEUR_SOURCES = Carte.c Collection.c Joueur.c GestionnaireJeu.c jeu.c logging.c \
                  Serveur.c server_communication.c global.c



# Fichiers client
CLIENT_SOURCES = client.c

SERVEUR_OBJECTS = $(SERVEUR_SOURCES:.c=.o)
CLIENT_OBJECTS = $(CLIENT_SOURCES:.c=.o)

SERVEUR_EXEC = serveur
CLIENT_EXEC = client

# Headers
HEADERS = defines.h structures.h methodes.h global.h Carte.h Collection.h \
          Joueur.h GestionnaireJeu.h jeu.h logging.h protocol.h \
          server_communication.h

# Cibles principales
all: $(SERVEUR_EXEC) $(CLIENT_EXEC)

$(SERVEUR_EXEC): $(SERVEUR_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo "✅ Serveur compilé: $@"

$(CLIENT_EXEC): $(CLIENT_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo "✅ Client compilé: $@"

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "📦 Compilé: $<"

# Nettoyage
clean:
	rm -f $(SERVEUR_OBJECTS) $(CLIENT_OBJECTS) $(SERVEUR_EXEC) $(CLIENT_EXEC)
	@echo "🧹 Nettoyage effectué"

# Recompile
rebuild: clean all

# Exécuter
run-serveur: $(SERVEUR_EXEC)
	@echo "🚀 Lancement du serveur..."
	./$(SERVEUR_EXEC) 127.0.0.1 4242 4

run-client: $(CLIENT_EXEC)
	@echo "🚀 Lancement du client..."
	./$(CLIENT_EXEC) 127.0.0.1 4242 Alice

# Phony targets
.PHONY: all clean rebuild run-serveur run-client info

# Debug
info:
	@echo "Serveur sources: $(SERVEUR_SOURCES)"
	@echo "Client sources: $(CLIENT_SOURCES)"
