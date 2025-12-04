CC := gcc
CFLAGS := -Wall -Wextra -O2 -g -I.
LDFLAGS :=

SRCS_MAIN := main.c Carte.c
OBJS_MAIN := $(SRCS_MAIN:.c=.o)
TARGET_MAIN := main

SRCS_CARTE := Carte.c
OBJS_CARTE := $(SRCS_CARTE:.c=.o)
TARGET_CARTE := carte

.PHONY: all clean

# Par défaut : construit l'exécutable principal
all: $(TARGET_MAIN)

# Cible principale : lie main.o et Carte.o
$(TARGET_MAIN): $(OBJS_MAIN)
	$(CC) $(LDFLAGS) -o $@ $^

# Cible optionnelle 'carte' : construit un exécutable à partir de Carte.c
# (utile seulement si `Carte.c` contient un `main` ou pour tests)
$(TARGET_CARTE): $(OBJS_CARTE)
	$(CC) $(LDFLAGS) -o $@ $^

# Règle générique : compiler .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS_MAIN) $(OBJS_CARTE) $(TARGET_MAIN) $(TARGET_CARTE)

# Règles supplémentaires utiles
.PHONY: rebuild
rebuild: clean all
