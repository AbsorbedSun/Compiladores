/* hoc.h -- definiciones compartidas por hoc4.y, code.c, symbol.c, init.c y math.c
 *
 * CAMBIOS respecto a la Practica 3 (hoc3):
 *   + Datum (tipo de la pila de la maquina): val pasa de double a Grande *
 *   + Inst, STOP, prog[] y las instrucciones de la maquina (de hoc4)
 *   + drop(): descarta el tope de la pila LIBERANDO su Grande
 *   + g_div() y g_es_cero(): division entera y prueba de cero
 *
 * CAMBIOS respecto a hoc4 original:
 *   - double -> Grande en Symbol, install() y Datum
 *   - el predefinido recibe y devuelve Grande * (NULL si hay error)
 */
#ifndef HOC_H
#define HOC_H

#include <stddef.h>
#include <stdint.h>

#define BASE 1000000000u          /* 10^9: 9 digitos decimales por limb */

/* ---------- Numero grande: signo + magnitud en base 10^9 ---------- */
typedef struct {
    int       neg;                /* 1 si es negativo (el cero nunca es negativo) */
    size_t    n;                  /* numero de limbs en uso (n >= 1)              */
    uint32_t *d;                  /* d[0] = menos significativo                   */
} Grande;

/* ---------- Tabla de simbolos (lista ligada, como en hoc3/hoc4) ---------- */
typedef struct Symbol {           /* entrada de la tabla de simbolos */
    char   *name;
    short   type;                 /* VAR, BLTIN, INDEF, NUMERO */
    union {
        Grande *val;                          /* si es VAR o NUMERO (el simbolo es su dueno) */
        Grande *(*ptr)(const Grande *);       /* si es BLTIN (devuelve NULL si hay error)    */
    } u;
    struct Symbol *next;          /* para ligarse al siguiente */
} Symbol;

/* symbol.c */
Symbol *install(char *s, int t, Grande *g), *lookup(char *s);
void   *emalloc(size_t n);

/* init.c */
void init(void);

/* hoc4.y */
void warning(const char *s, const char *t);
void execerror(const char *s, const char *t);   /* avisa y regresa al ciclo principal */
void execabort(void);                           /* regresa al ciclo principal sin mensaje */

/* ---------- Maquina de pila (code.c) ----------
 * Datum es lo que viaja por la pila:
 *   val -> un Grande * que pertenece a la pila (quien lo saca debe liberarlo)
 *   sym -> un Symbol * de la tabla de simbolos (no se libera)
 */
typedef union Datum {
    Grande *val;
    Symbol *sym;
} Datum;

extern Datum pop(void);
typedef void (*Inst)();           /* instruccion de maquina */

#define STOP    (Inst) 0
extern Inst prog[];
extern Inst *progp;               /* siguiente lugar libre para generar codigo */
extern void initcode(void);
extern Inst *code(Inst f);
extern void execute(Inst *p);
extern void eval(), add(), sub(), mul(), div_(), negate(), power();
extern void assign(), bltin(), varpush(), constpush(), print(), drop();

/* ---------- math.c: tipo Grande y sus operaciones ---------- */
Grande *g_desde_cadena(const char *s);
Grande *g_copia(const Grande *a);
void    g_libera(Grande *g);
void    g_imprime(const Grande *g);
int     g_es_cero(const Grande *a);
Grande *g_negado(const Grande *a);
Grande *g_suma(const Grande *a, const Grande *b);
Grande *g_resta(const Grande *a, const Grande *b);
Grande *g_mult(const Grande *a, const Grande *b);
Grande *g_div(const Grande *a, const Grande *b);        /* cociente entero; NULL si b = 0 */
Grande *g_pot(const Grande *base, const Grande *exp);   /* NULL si hay error */
Grande *g_abs(const Grande *a);                         /* predefinido abs()  */
Grande *g_fact(const Grande *a);                        /* predefinido fact(); NULL si hay error */

#endif
