#ifndef CARTE
#define CARTE

typedef struct {
    int valeurNum;  // de 1 à 104 //
    int teteBoeuf;  // de 1 à 7 //
} Carte;

/* “méthodes” */
void Carte_Init(Carte *p, int num, int tete);
void Carte_InitNum(Carte *p, int num);
int Carte_getValeurNum(Carte *p);
int Carte_getTeteBoeuf(Carte *p);

#endif


