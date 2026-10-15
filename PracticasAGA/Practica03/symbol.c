/* symbol.c -- tabla de simbolos (lista ligada), tomada de hoc3
 *
 * CAMBIOS respecto a hoc3 original:
 *   + install() guarda un Grande * (antes un double)
 *   + install() ahora SI usa emalloc() (antes llamaba a malloc directo)
 *   + emalloc() termina el programa si no hay memoria (reemplaza a
 *     reserva() de la calculadora; ya no usa execerror/longjmp)
 *   + se agregan los #include que faltaban (stdio/stdlib/string)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hoc.h"

static Symbol *symlist = 0;       /* tabla de simbolos: lista ligada */

Symbol *lookup(char *s)           /* encontrar s en la tabla de simbolos */
{
    Symbol *sp;
    for (sp = symlist; sp != (Symbol *)0; sp = sp->next)
        if (strcmp(sp->name, s) == 0)
            return sp;
    return 0;                     /* 0 ==> no se encontro */
}

Symbol *install(char *s, int t, Grande *g)   /* instalar s en la tabla de simbolos */
{
    Symbol *sp = (Symbol *) emalloc(sizeof(Symbol));
    sp->name = emalloc(strlen(s) + 1);       /* +1 para '\0' */
    strcpy(sp->name, s);
    sp->type  = t;
    sp->u.val = g;                           /* NULL para INDEF y BLTIN */
    sp->next  = symlist;                     /* poner al frente de la lista */
    symlist   = sp;
    return sp;
}

void *emalloc(size_t n)           /* revisar el regreso desde malloc */
{
    void *p = malloc(n ? n : 1);
    if (!p) {
        fprintf(stderr, "Error: memoria insuficiente\n");
        exit(EXIT_FAILURE);
    }
    return p;
}
