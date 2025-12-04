#ifndef TEMPLATE
#define TEMPLATE

#define MAX_CHARS 256

typedef struct {
    int x;
    char y[MAX_CHARS];
} Template;

/* “méthodes” */
void Template_Init(Template *p, int x, char* y);
void Template_Method(Template *p, int dx, char* dy);

#endif
