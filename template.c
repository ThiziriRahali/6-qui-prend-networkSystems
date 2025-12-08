#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "template.h"


void Template_Init(Template *p, int x, char* y) {
    p->x = x;
    strncpy(p->y, y, MAX_CHARS - 1);
    p->y[MAX_CHARS - 1] = '\0';  /* sécurité */
}

void Template_Method(Template *p, int dx, char* dy) {
    p->x += dx;
    strncpy(p->y, dy, MAX_CHARS - 1);
    p->y[MAX_CHARS - 1] = '\0';
}
