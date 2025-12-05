CC := gcc
CFLAGS := -Wall -Wextra -O2 -g -I.
LDFLAGS :=

SRCS_MAIN := main.c Carte.c Collection.c global.c
OBJS_MAIN := $(SRCS_MAIN:.c=.o)
TARGET_MAIN := main

.PHONY: all clean run rebuild

# Par défaut : construit l'exécutable principal
all: $(TARGET_MAIN)

# Cible principale : compile et lie tous les fichiers nécessaires
$(TARGET_MAIN): $(OBJS_MAIN)
	$(CC) $(LDFLAGS) -o $@ $^

# Règle générique : compiler .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Lancer le programme
run: $(TARGET_MAIN)
	./$(TARGET_MAIN)

# Nettoyer les fichiers objets et exécutables
clean:
	rm -f $(OBJS_MAIN) $(TARGET_MAIN)

# Nettoyer et reconstruire
rebuild: clean all
