/* init.c -- instala los predefinidos en la tabla de simbolos (como hoc3)
 *
 * CAMBIOS respecto a hoc3 original:
 *   - se QUITAN las constantes (PI, E, GAMMA, DEG, PHI): son reales y la
 *     calculadora solo maneja enteros grandes
 *   - se QUITAN sin, cos, atan, log, log10, exp, sqrt, int: trabajan con double
 *   + se dejan dos predefinidos que SI tienen sentido con enteros grandes:
 *       abs(x)  y  fact(x)
 */
#include "hoc.h"
#include "y.tab.h"                /* para los codigos VAR, BLTIN, INDEF */

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
    for (i = 0; builtins[i].name; i++) {
        s = install(builtins[i].name, BLTIN, NULL);
        s->u.ptr = builtins[i].func;
    }
}
