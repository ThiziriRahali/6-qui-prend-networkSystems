#include "Carte.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    Carte maCarte;
    Carte_Init(&maCarte, 10, 1);

    printf("Valeur Num: %d\n", Carte_getValeurNum(&maCarte));
    printf("Tete Boeuf: %d\n", Carte_getTeteBoeuf(&maCarte));
    Carte maCarte2;
    Carte_InitNum(&maCarte2, 10);

    printf("Valeur Num2: %d\n", Carte_getValeurNum(&maCarte2));
    printf("Tete Boeuf2: %d\n", Carte_getTeteBoeuf(&maCarte2));

    return 0;
}