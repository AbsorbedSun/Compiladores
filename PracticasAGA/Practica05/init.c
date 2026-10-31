/* init.c -- instala palabras clave y predefinidos en la tabla de simbolos
 *
 * CAMBIOS para la Practica 5 (hoc5):
 *   + keywords[]: if, else, while y print se instalan con su codigo de token
 *     (IF, ELSE, WHILE, PRINT), igual que en hoc5. Asi yylex() los reconoce
 *     como palabras reservadas y no como variables.
 *
 * CAMBIOS respecto a hoc3 original:
 *   - se QUITAN las constantes (PI, E, GAMMA, DEG, PHI): son reales y la
 *     calculadora solo maneja enteros grandes
 *   - se QUITAN sin, cos, atan, log, log10, exp, sqrt, int: trabajan con double
 *   + se dejan dos predefinidos que SI tienen sentido con enteros grandes:
 *       abs(x)  y  fact(x)
 */
#include "hoc.h"
#include "y.tab.h"                /* para los codigos VAR, BLTIN, INDEF, IF, ELSE... */

static struct {                   /* Palabras clave */
    char *name;
    int   kval;
} keywords[] = {
    { "if",    IF    },
    { "else",  ELSE  },
    { "while", WHILE },
    { "print", PRINT },
    { 0,       0     }
};

static struct {                   /* Predefinidos */
    char *name;
    Grande *(*func)(const Grande *);
} builtins[] = {
    { "abs",  g_abs  },
    { "fact", g_fact },
    { 0,      0      }
};

void init(void)                   /* instalar predefinidos en la tabla */
{
    int i;
    Symbol *s;
    for (i = 0; keywords[i].name; i++)
        install(keywords[i].name, keywords[i].kval, NULL);
    for (i = 0; builtins[i].name; i++) {
        s = install(builtins[i].name, BLTIN, NULL);
        s->u.ptr = builtins[i].func;
    }
}
