/* code.c -- maquina de pila de hoc4, adaptada a numeros grandes
 *
 * CAMBIOS respecto a hoc4 original:
 *   - Datum.val es un Grande * (antes double). Todo Grande que viaja por la
 *     pila pertenece a la pila: quien lo saca (pop) lo libera con g_libera.
 *   + se agrega un arreglo paralelo "esval[]" que marca que entradas de la pila
 *     son Grande (hay que liberar) y cuales son Symbol (no se liberan); asi,
 *     si hay un error a medias, initcode() libera lo que haya quedado.
 *   + drop(): como pop pero libera el Grande (se usa al terminar una asignacion)
 *   + div_() hace division entera y detecta division entre cero
 *   + power()/bltin() detectan el NULL que devuelven g_pot()/g_fact()
 *   + constpush() mete una COPIA de la constante (la tabla conserva la suya)
 *   + eval() mete una COPIA del valor de la variable
 *   + assign(): la variable se queda con el Grande y la pila recibe una copia
 */
#include <stdio.h>
#include "hoc.h"
#include "y.tab.h"

#define NSTACK  256
static Datum stack[NSTACK];       /* la pila */
static char  esval[NSTACK];       /* esval[i] = 1 si stack[i] es un Grande (propio) */
static Datum *stackp;             /* siguiente lugar libre en la pila */
#define NPROG   2000
Inst  prog[NPROG];                /* RAM de la maquina */
Inst *progp;                      /* siguiente lugar libre para la generacion de codigo */
Inst *pc;                         /* contador de programa durante la ejecucion */

void initcode(void)
{
    if (stackp)                   /* liberar lo que haya quedado tras un error */
        while (stackp > stack) {
            stackp--;
            if (esval[stackp - stack])
                g_libera(stackp->val);
        }
    stackp = stack;
    progp  = prog;
}

static void push(Datum d, int es_grande)      /* meter d en la pila */
{
    if (stackp >= &stack[NSTACK]) {
        if (es_grande) g_libera(d.val);
        execerror("desbordamiento de pila", (char *)0);
    }
    esval[stackp - stack] = (char)es_grande;
    *stackp++ = d;
}

Datum pop(void)                   /* sacar y retornar el elemento del tope */
{
    if (stackp <= stack)
        execerror("pila vacia", (char *)0);
    return *--stackp;
}

void constpush(void)              /* meter una constante a la pila */
{
    Datum d;
    d.val = g_copia(((Symbol *)*pc++)->u.val);
    push(d, 1);
}

void varpush(void)                /* meter una variable (su simbolo) a la pila */
{
    Datum d;
    d.sym = (Symbol *)(*pc++);
    push(d, 0);
}

void eval(void)                   /* evaluar una variable en la pila */
{
    Datum d;
    d = pop();
    if (d.sym->type == INDEF)
        execerror("variable no definida", d.sym->name);
    d.val = g_copia(d.sym->u.val);
    push(d, 1);
}

void add(void)                    /* sumar los dos elementos superiores de la pila */
{
    Datum d1, d2;
    d2 = pop();
    d1 = pop();
    Grande *r = g_suma(d1.val, d2.val);
    g_libera(d1.val);
    g_libera(d2.val);
    d1.val = r;
    push(d1, 1);
}

void sub(void)
{
    Datum d1, d2;
    d2 = pop();
    d1 = pop();
    Grande *r = g_resta(d1.val, d2.val);
    g_libera(d1.val);
    g_libera(d2.val);
    d1.val = r;
    push(d1, 1);
}

void mul(void)
{
    Datum d1, d2;
    d2 = pop();
    d1 = pop();
    Grande *r = g_mult(d1.val, d2.val);
    g_libera(d1.val);
    g_libera(d2.val);
    d1.val = r;
    push(d1, 1);
}

void div_(void)                   /* division entera (trunca hacia cero) */
{
    Datum d1, d2;
    d2 = pop();
    d1 = pop();
    if (g_es_cero(d2.val)) {
        g_libera(d1.val);
        g_libera(d2.val);
        execerror("division entre cero", (char *)0);
    }
    Grande *r = g_div(d1.val, d2.val);
    g_libera(d1.val);
    g_libera(d2.val);
    d1.val = r;
    push(d1, 1);
}

void negate(void)
{
    Datum d;
    d = pop();
    Grande *r = g_negado(d.val);
    g_libera(d.val);
    d.val = r;
    push(d, 1);
}

void power(void)
{
    Datum d1, d2;
    d2 = pop();
    d1 = pop();
    Grande *r = g_pot(d1.val, d2.val);    /* NULL si hay error (ya aviso) */
    g_libera(d1.val);
    g_libera(d2.val);
    if (!r)
        execabort();
    d1.val = r;
    push(d1, 1);
}

void assign(void)                 /* asignar el valor superior al simbolo siguiente */
{
    Datum d1, d2;
    d1 = pop();                   /* el simbolo (lo metio varpush) */
    d2 = pop();                   /* el valor   (Grande propio)    */
    if (d1.sym->type != VAR && d1.sym->type != INDEF) {
        g_libera(d2.val);
        execerror("asignacion a algo que no es variable", d1.sym->name);
    }
    if (d1.sym->type == VAR)
        g_libera(d1.sym->u.val);  /* se descarta el valor anterior */
    d1.sym->u.val = d2.val;       /* la variable se queda con el Grande */
    d1.sym->type  = VAR;
    d2.val = g_copia(d2.val);     /* la pila recibe una copia (asgn tambien es expr) */
    push(d2, 1);
}

void drop(void)                   /* sacar el tope de la pila y liberarlo (sin imprimir) */
{
    Datum d;
    d = pop();
    g_libera(d.val);
}

void print(void)                  /* sacar el valor superior de la pila e imprimirlo */
{
    Datum d;
    d = pop();
    putchar('\t');
    g_imprime(d.val);
    putchar('\n');
    g_libera(d.val);
}

void bltin(void)                  /* evaluar un predefinido en el tope de la pila */
{
    Datum d;
    Grande *(*f)(const Grande *) = (Grande *(*)(const Grande *))(*pc++);
    d = pop();
    Grande *r = f(d.val);         /* NULL si hay error (ya aviso) */
    g_libera(d.val);
    if (!r)
        execabort();
    d.val = r;
    push(d, 1);
}

Inst *code(Inst f)                /* instalar una instruccion u operando */
{
    Inst *oprogp = progp;
    if (progp >= &prog[NPROG])
        execerror("programa demasiado grande", (char *)0);
    *progp++ = f;
    return oprogp;
}

void execute(Inst *p)             /* ejecucion con la maquina */
{
    for (pc = p; *pc != STOP; )
        (*pc++)();
}
