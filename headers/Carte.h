#ifndef CARTE_H
#define CARTE_H

#include "structures.h"

void Carte_Init(Carte *p, int num, int tete);
void Carte_InitNum(Carte *p, int num);
int Carte_getValeurNum(Carte *p);
int Carte_getValeurTete(Carte *p);
char* Carte_toString(Carte *p);
char* Carte_toStringCache(Carte *p);

#endif
