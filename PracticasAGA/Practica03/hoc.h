/* hoc.h -- definiciones compartidas por hoc3.y, symbol.c, init.c y math.c
 *
 * CAMBIOS respecto a hoc3 original:
 *   + se agrega el tipo Grande (antes vivia dentro del .y de la calculadora)
 *   + Symbol.u.val  pasa de  double      a  Grande *
 *   + Symbol.u.ptr  pasa de  double (*)(double)  a  Grande *(*)(const Grande *)
 *   + install() recibe un Grande * en lugar de un double
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

/* ---------- Tabla de simbolos (lista ligada, como en hoc3) ---------- */
typedef struct Symbol {           /* entrada de la tabla de simbolos */
    char   *name;
    short   type;                 /* VAR, BLTIN, INDEF */
    union {
        Grande *val;                          /* si es VAR   (el simbolo es su dueno) */
        Grande *(*ptr)(const Grande *);       /* si es BLTIN (devuelve NULL si hay error) */
    } u;
    struct Symbol *next;          /* para ligarse al siguiente */
} Symbol;

/* symbol.c */
Symbol *install(char *s, int t, Grande *g), *lookup(char *s);
void   *emalloc(size_t n);

/* init.c */
void init(void);

/* hoc3.y */
void warning(const char *s, const char *t);

/* math.c: tipo Grande y sus operaciones */
Grande *g_desde_cadena(const char *s);
Grande *g_copia(const Grande *a);
void    g_libera(Grande *g);
void    g_imprime(const Grande *g);
Grande *g_negado(const Grande *a);
Grande *g_suma(const Grande *a, const Grande *b);
Grande *g_resta(const Grande *a, const Grande *b);
Grande *g_mult(const Grande *a, const Grande *b);
Grande *g_pot(const Grande *base, const Grande *exp);   /* NULL si hay error */
Grande *g_abs(const Grande *a);                         /* predefinido abs()  */
Grande *g_fact(const Grande *a);                        /* predefinido fact(); NULL si hay error */

#endif
