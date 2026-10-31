/* code.c -- maquina de pila de hoc5 (hoc4 + if/while), con numeros grandes
 *
 * CAMBIOS para la Practica 5 (hoc5), respecto a la Practica 4:
 *   + gt(), lt(), eq(), ge(), le(), ne(): comparan dos Grande con g_cmp() y
 *     dejan el resultado (1.0 o 0.0) en la parte dval de un Datum. Ese Datum
 *     NO es un Grande, asi que se mete a la pila marcado con es_grande = 0.
 *   + and_(), or_(), not_(): operadores logicos; aceptan como operando un
 *     dval (resultado de una comparacion) o un Grande (0 = falso, otro = verdadero)
 *   + tobool(): instruccion que genera la gramatica al final de cada condicion
 *     para que lo que quede en la pila sea siempre un dval
 *   + ifcode() y whilecode(): leen la condicion en la parte dval del Datum
 *   + prexpr(): ejecuta "print expr" (imprime un Grande o un dval)
 *   + drop() sustituye a pop1() de hoc5: descarta el tope y libera el Grande
 *     si lo es (en hoc5 solo bajaba el apuntador de la pila)
 *   + popk()/popg()/popbool(): sacan el tope sabiendo de que tipo es (esval[]).
 *     La aritmetica usa popg(), de modo que  (3 > 2) + 1  tambien funciona.
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

static Datum popk(int *es_grande)  /* sacar el tope y decir si es un Grande propio */
{
    if (stackp <= stack)
        execerror("pila vacia", (char *)0);
    --stackp;
    *es_grande = esval[stackp - stack];
    return *stackp;
}

Datum pop(void)                   /* sacar y retornar el elemento del tope */
{
    int es;
    return popk(&es);
}

static void pushd(double v)       /* meter un resultado logico (dval) en la pila */
{
    Datum d;
    d.dval = v;
    push(d, 0);                   /* no es un Grande: no hay nada que liberar */
}

static Grande *popg(void)         /* sacar el tope COMO Grande (propio de quien lo llama) */
{
    int es;
    Datum d = popk(&es);
    if (!es)                      /* era un dval: verdadero -> 1, falso -> 0 */
        d.val = g_desde_ulong(d.dval != 0.0);
    return d.val;
}

static int popbool(void)          /* sacar el tope como valor de verdad */
{
    int es, t;
    Datum d = popk(&es);
    if (es) {                     /* Grande: es verdadero si es distinto de cero */
        t = !g_es_cero(d.val);
        g_libera(d.val);
    } else
        t = d.dval != 0.0;
    return t;
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
    Datum d1;
    Grande *a, *b;
    b = popg();
    a = popg();
    d1.val = g_suma(a, b);
    g_libera(a);
    g_libera(b);
    push(d1, 1);
}

void sub(void)
{
    Datum d1;
    Grande *a, *b;
    b = popg();
    a = popg();
    d1.val = g_resta(a, b);
    g_libera(a);
    g_libera(b);
    push(d1, 1);
}

void mul(void)
{
    Datum d1;
    Grande *a, *b;
    b = popg();
    a = popg();
    d1.val = g_mult(a, b);
    g_libera(a);
    g_libera(b);
    push(d1, 1);
}

void div_(void)                   /* division entera (trunca hacia cero) */
{
    Datum d1;
    Grande *a, *b;
    b = popg();
    a = popg();
    if (g_es_cero(b)) {
        g_libera(a);
        g_libera(b);
        execerror("division entre cero", (char *)0);
    }
    d1.val = g_div(a, b);
    g_libera(a);
    g_libera(b);
    push(d1, 1);
}

void negate(void)
{
    Datum d;
    Grande *a = popg();
    d.val = g_negado(a);
    g_libera(a);
    push(d, 1);
}

void power(void)
{
    Datum d1;
    Grande *a, *b, *r;
    b = popg();
    a = popg();
    r = g_pot(a, b);              /* NULL si hay error (ya aviso) */
    g_libera(a);
    g_libera(b);
    if (!r)
        execabort();
    d1.val = r;
    push(d1, 1);
}

void assign(void)                 /* asignar el valor superior al simbolo siguiente */
{
    Datum d1, d2;
    d1 = pop();                   /* el simbolo (lo metio varpush) */
    d2.val = popg();              /* el valor como Grande propio (un dval se vuelve 0 o 1) */
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

void drop(void)                   /* sacar el tope de la pila y descartarlo (pop1 de hoc5) */
{
    int es;
    Datum d = popk(&es);
    if (es)                       /* si es un Grande hay que liberarlo; un dval no */
        g_libera(d.val);
}

static void imprime_tope(void)    /* saca el tope y lo imprime: Grande o resultado logico */
{
    int es;
    Datum d = popk(&es);
    if (es) {
        g_imprime(d.val);
        g_libera(d.val);
    } else
        putchar(d.dval != 0.0 ? '1' : '0');
}

void print(void)                  /* expresion suelta: sacar el tope e imprimirlo */
{
    putchar('\t');
    imprime_tope();
    putchar('\n');
}

void prexpr(void)                 /* "print expr": imprime el valor y salta de linea */
{
    imprime_tope();
    putchar('\n');
}

void bltin(void)                  /* evaluar un predefinido en el tope de la pila */
{
    Datum d;
    Grande *(*f)(const Grande *) = (Grande *(*)(const Grande *))(*pc++);
    Grande *a = popg();
    Grande *r = f(a);             /* NULL si hay error (ya aviso) */
    g_libera(a);
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

/* ================= Practica 5: comparaciones =================
 * Cada una saca dos Grande (d2 es el de arriba), los compara con g_cmp(),
 * los libera y mete en la pila un Datum cuya parte dval vale 1.0 o 0.0.
 */
void gt(void)                     /* a > b */
{
    Grande *a, *b;
    b = popg();
    a = popg();
    int c = g_cmp(a, b);
    g_libera(a);
    g_libera(b);
    pushd((double)(c > 0));
}

void lt(void)                     /* a < b */
{
    Grande *a, *b;
    b = popg();
    a = popg();
    int c = g_cmp(a, b);
    g_libera(a);
    g_libera(b);
    pushd((double)(c < 0));
}

void ge(void)                     /* a >= b */
{
    Grande *a, *b;
    b = popg();
    a = popg();
    int c = g_cmp(a, b);
    g_libera(a);
    g_libera(b);
    pushd((double)(c >= 0));
}

void le(void)                     /* a <= b */
{
    Grande *a, *b;
    b = popg();
    a = popg();
    int c = g_cmp(a, b);
    g_libera(a);
    g_libera(b);
    pushd((double)(c <= 0));
}

void eq(void)                     /* a == b */
{
    Grande *a, *b;
    b = popg();
    a = popg();
    int c = g_cmp(a, b);
    g_libera(a);
    g_libera(b);
    pushd((double)(c == 0));
}

void ne(void)                     /* a != b */
{
    Grande *a, *b;
    b = popg();
    a = popg();
    int c = g_cmp(a, b);
    g_libera(a);
    g_libera(b);
    pushd((double)(c != 0));
}

/* ================= Practica 5: operadores logicos =================
 * (como en hoc5, evaluan siempre los dos operandos: no hay corto circuito)
 */
void and_(void)
{
    int b = popbool();
    int a = popbool();
    pushd((double)(a && b));
}

void or_(void)
{
    int b = popbool();
    int a = popbool();
    pushd((double)(a || b));
}

void not_(void)
{
    int a = popbool();
    pushd((double)(!a));
}

void tobool(void)                 /* deja en el tope un dval: 1.0 si era verdadero, 0.0 si no */
{
    int a = popbool();
    pushd((double)a);
}

/* ================= Practica 5: control de flujo =================
 * La gramatica genera, para cada if/while, un bloque de la forma:
 *
 *   whilecode:  whilecode | cuerpo | siguiente | cond... STOP | cuerpo... STOP
 *   ifcode:     ifcode | then | else | siguiente | cond... STOP | then... STOP | else... STOP
 *
 * Al ejecutarse whilecode()/ifcode(), pc ya apunta al primer campo (savepc).
 * La condicion (que termina en tobool) deja un Datum con dval = 1.0 o 0.0.
 */
void whilecode(void)
{
    Datum d;
    Inst *savepc = pc;            /* savepc[0] = cuerpo, savepc[1] = siguiente proposicion */

    execute(savepc + 2);          /* condicion */
    d = pop();
    while (d.dval) {
        execute(*((Inst **)(savepc)));    /* cuerpo */
        execute(savepc + 2);              /* condicion otra vez */
        d = pop();
    }
    pc = *((Inst **)(savepc + 1));        /* siguiente proposicion */
}

void ifcode(void)
{
    Datum d;
    Inst *savepc = pc;            /* savepc[0] = parte then, [1] = parte else, [2] = siguiente */

    execute(savepc + 3);          /* condicion */
    d = pop();
    if (d.dval)
        execute(*((Inst **)(savepc)));
    else if (*((Inst **)(savepc + 1)))    /* hay parte else? */
        execute(*((Inst **)(savepc + 1)));
    pc = *((Inst **)(savepc + 2));        /* siguiente proposicion */
}
