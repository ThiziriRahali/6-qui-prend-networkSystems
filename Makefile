CC := gcc
CFLAGS := -Wall -Wextra -I. -O2 -g -MMD -MP
LDFLAGS :=

# Sources détectées automatiquement
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
DEPS := $(OBJS:.o=.d)
TARGET := main

.PHONY: all clean run debug rebuild

# Par défaut : construit l'exécutable principal
all: $(TARGET)

# Cible principale : compile et lie tous les fichiers nécessaires
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# Règle générique : compiler .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Cible debug (construit avec optimisation désactivée et infos de debug)
debug: CFLAGS += -g -O0 -DDEBUG
debug: clean all

# Lancer le programme
run: $(TARGET)
	./$(TARGET)

# Nettoyer les fichiers objets, dépendances et exécutables
clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

# Nettoyer et reconstruire
rebuild: clean all

# Inclure les fichiers de dépendances si présents
-include $(DEPS)
