CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -g -MMD -MP
LDFLAGS = -pthread

# ========================================
# Fichiers sources
# ========================================

# Fichiers sources COMMUNS (métier du jeu, pas de dépendances serveur)
SRCCOMMON = Carte.c Collection.c Joueur.c GestionnaireJeu.c global.c jeu.c logging.c

# Fichiers sources SPÉCIFIQUES AU SERVEUR
SRCSERVER = Serveur.c client_handler.c server_communication.c

# Fichiers sources SPÉCIFIQUES AU CLIENT
SRCCLIENT = client.c

# ========================================
# Objets compilés
# ========================================

OBJCOMMON = $(SRCCOMMON:.c=.o)
OBJSERVER = $(SRCSERVER:.c=.o)
OBJCLIENT = $(SRCCLIENT:.c=.o)

# Fichiers de dépendances
DEPS = $(OBJCOMMON:.o=.d) $(OBJSERVER:.o=.d) $(OBJCLIENT:.o=.d)

# Cibles finales
TARGETS = serveur client

# ========================================
# Règles de compilation
# ========================================

all: $(TARGETS) scripts

scripts:
	@chmod +x stats.sh 2>/dev/null; true

# SERVEUR : inclut objets communs + serveur + communication
serveur: $(OBJCOMMON) $(OBJSERVER)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# CLIENT : inclut UNIQUEMENT objets communs + client (PAS server_communication.o)
client: $(OBJCOMMON) $(OBJCLIENT)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Compilation générique .c → .o
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Inclusion des fichiers de dépendances
-include $(DEPS)

# ========================================
# Cibles auxiliaires
# ========================================

stats-shell:
	@if [ -f jeu.log ]; then \
		./stats.sh jeu.log; \
	else \
		echo "❌ Erreur: fichier jeu.log non trouvé"; \
		echo "💡 Remarque: Joue quelques parties pour générer jeu.log"; \
	fi

clean:
	rm -f *.o *.d $(TARGETS)

distclean: clean
	rm -f jeu.log

# ========================================
# Déclaration des cibles non-fichier
# ========================================

.PHONY: all clean distclean scripts stats-shell