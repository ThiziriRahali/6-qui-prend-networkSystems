# Compilateur et flags
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -g -I./headers
LDFLAGS = -pthread

# Répertoires
HEADERS_DIR = headers
C_DIR = c
LOGS_DIR = logs
SCRIPTS_DIR = scripts

# Fichiers serveur (.c)
SERVEUR_SOURCES = $(C_DIR)/Carte.c $(C_DIR)/Collection.c $(C_DIR)/Joueur.c \
                  $(C_DIR)/jeu.c $(C_DIR)/logging.c \
                  $(C_DIR)/Serveur.c $(C_DIR)/server_communication.c $(C_DIR)/global.c

# Fichiers client (.c)
CLIENT_SOURCES = $(C_DIR)/client.c $(C_DIR)/global.c

# Fichiers objets
SERVEUR_OBJECTS = $(SERVEUR_SOURCES:.c=.o)
CLIENT_OBJECTS = $(CLIENT_SOURCES:.c=.o)

# Exécutables
SERVEUR_EXEC = serveur
CLIENT_EXEC = client

# Headers (dépendances)
HEADERS = $(HEADERS_DIR)/defines.h $(HEADERS_DIR)/structures.h \
          $(HEADERS_DIR)/methodes.h $(HEADERS_DIR)/global.h \
          $(HEADERS_DIR)/protocol.h $(HEADERS_DIR)/server_communication.h

LOGS_HEADERS = $(LOGS_DIR)/logging.h

# Cibles principales
all: $(SERVEUR_EXEC) $(CLIENT_EXEC)

$(SERVEUR_EXEC): $(SERVEUR_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo "✅ Serveur compilé: $@"

$(CLIENT_EXEC): $(CLIENT_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo "✅ Client compilé: $@"

%.o: %.c $(HEADERS) $(LOGS_HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "💾 Compilé: $<"

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

# Stats
stats-shell:
	@if [ -f "jeu.log" ]; then \
		bash $(SCRIPTS_DIR)/stats.sh jeu.log; \
	else \
		echo "❌ Fichier jeu.log introuvable. Jouez d'abord une partie!"; \
	fi

stats:
	@if [ -f "jeu.log" ]; then \
		awk -f $(SCRIPTS_DIR)/stats.awk jeu.log; \
	else \
		echo "❌ Fichier jeu.log introuvable. Jouez d'abord une partie!"; \
	fi


test:
	@echo "🎮 Lancement du test automatique..."
	bash $(SCRIPTS_DIR)/run_test.sh

# Phony targets
.PHONY: all clean rebuild run-serveur run-client stats-shell stats test info

# Debug
info:
	@echo "Serveur sources: $(SERVEUR_SOURCES)"
	@echo "Client sources: $(CLIENT_SOURCES)"
	@echo "Serveur objects: $(SERVEUR_OBJECTS)"
	@echo "Client objects: $(CLIENT_OBJECTS)"
