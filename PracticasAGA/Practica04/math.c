/* math.c -- aritmetica de numeros grandes
 *
 * En hoc3 este archivo tenia las funciones de <math.h> con errcheck().
 * Aqui ocupa su lugar el tipo Grande con todas sus operaciones (antes
 * estaban al final del .y de la calculadora).
 *
 * CAMBIOS respecto a la calculadora original:
 *   - reserva()/calloc() -> emalloc() de symbol.c
 *   - yyerror() -> warning() (asi no depende del parser)
 *   + g_copia() ahora es publica (el parser la usa para leer/asignar variables)
 *   + g_a_ulong() y g_desde_ulong(): conversiones auxiliares
 *   + g_abs() y g_fact(): funciones predefinidas
 *
 * CAMBIOS para la Practica 4 (hoc4):
 *   + g_es_cero() y g_div(): division entera (trunca hacia cero), porque
 *     hoc4 tiene el operador '/' y la maquina de pila usa div_()
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "hoc.h"

/* ================= tipo Grande ================= */

/* Crea un Grande con n limbs, todos en cero. */
static Grande *g_nuevo(size_t n)
{
    Grande *g = emalloc(sizeof *g);
    g->neg = 0;
    g->n   = n;
    g->d   = emalloc(n * sizeof *g->d);
    memset(g->d, 0, n * sizeof *g->d);
    return g;
}

void g_libera(Grande *g)
{
    if (g) { free(g->d); free(g); }
}

/* Quita limbs cero de la izquierda y normaliza el signo del cero. */
static Grande *g_recorta(Grande *g)
{
    while (g->n > 1 && g->d[g->n - 1] == 0) g->n--;
    if (g->n == 1 && g->d[0] == 0) g->neg = 0;
    return g;
}

Grande *g_copia(const Grande *a)
{
    Grande *r = g_nuevo(a->n);
    memcpy(r->d, a->d, a->n * sizeof *r->d);
    r->neg = a->neg;
    return r;
}

/* Convierte una cadena de digitos decimales (sin signo) a Grande. */
Grande *g_desde_cadena(const char *s)
{
    size_t len = strlen(s);
    Grande *g = g_nuevo((len + 8) / 9);
    for (size_t i = 0; i < g->n; i++) {
        size_t fin    = len - 9 * i;
        size_t inicio = fin >= 9 ? fin - 9 : 0;
        uint32_t v = 0;
        for (size_t k = inicio; k < fin; k++) v = v * 10 + (s[k] - '0');
        g->d[i] = v;
    }
    return g_recorta(g);
}

/* unsigned long -> Grande (3 limbs de 10^9 alcanzan para 64 bits). */
static Grande *g_desde_ulong(unsigned long v)
{
    Grande *g = g_nuevo(3);
    for (size_t i = 0; v; i++, v /= BASE) g->d[i] = (uint32_t)(v % BASE);
    return g_recorta(g);
}

/* Grande -> unsigned long. Devuelve 0 si no cabe (o es negativo). */
static int g_a_ulong(const Grande *g, unsigned long *out)
{
    unsigned long e = 0;
    if (g->neg) return 0;
    for (size_t i = g->n; i-- > 0; ) {
        if (e > (ULONG_MAX - g->d[i]) / BASE) return 0;
        e = e * BASE + g->d[i];
    }
    *out = e;
    return 1;
}

void g_imprime(const Grande *g)
{
    if (g->neg) putchar('-');
    printf("%u", g->d[g->n - 1]);
    for (size_t i = g->n - 1; i-- > 0; ) printf("%09u", g->d[i]);
}

/* ---------- operaciones sobre magnitudes (ignoran el signo) ---------- */

static int mag_cmp(const Grande *a, const Grande *b)
{
    if (a->n != b->n) return a->n > b->n ? 1 : -1;
    for (size_t i = a->n; i-- > 0; )
        if (a->d[i] != b->d[i]) return a->d[i] > b->d[i] ? 1 : -1;
    return 0;
}

static Grande *mag_suma(const Grande *a, const Grande *b)
{
    size_t max = a->n > b->n ? a->n : b->n;
    Grande *r = g_nuevo(max + 1);
    uint64_t acarreo = 0;
    for (size_t i = 0; i < max; i++) {
        uint64_t s = acarreo;
        if (i < a->n) s += a->d[i];
        if (i < b->n) s += b->d[i];
        r->d[i] = (uint32_t)(s % BASE);
        acarreo = s / BASE;
    }
    r->d[max] = (uint32_t)acarreo;
    return r;
}

/* Requiere |a| >= |b|. */
static Grande *mag_resta(const Grande *a, const Grande *b)
{
    Grande *r = g_nuevo(a->n);
    int64_t prestamo = 0;
    for (size_t i = 0; i < a->n; i++) {
        int64_t s = (int64_t)a->d[i] - prestamo - (i < b->n ? b->d[i] : 0);
        if (s < 0) { s += BASE; prestamo = 1; } else prestamo = 0;
        r->d[i] = (uint32_t)s;
    }
    return r;
}

/* ---------- operaciones con signo ---------- */

int g_es_cero(const Grande *a)
{
    return a->n == 1 && a->d[0] == 0;
}

Grande *g_negado(const Grande *a)
{
    Grande *r = g_copia(a);
    r->neg = !a->neg;
    return g_recorta(r);              /* -0 se normaliza a 0 */
}

Grande *g_suma(const Grande *a, const Grande *b)
{
    Grande *r;
    if (a->neg == b->neg) {
        r = mag_suma(a, b);
        r->neg = a->neg;
    } else {
        int c = mag_cmp(a, b);
        if (c == 0)     r = g_nuevo(1);                           /* a + (-a) = 0 */
        else if (c > 0) { r = mag_resta(a, b); r->neg = a->neg; }
        else            { r = mag_resta(b, a); r->neg = b->neg; }
    }
    return g_recorta(r);
}

Grande *g_resta(const Grande *a, const Grande *b)
{
    Grande tmp = *b;                  /* vista de b con el signo invertido */
    tmp.neg = !b->neg;                /* (comparte d; no se libera)        */
    return g_suma(a, &tmp);
}

Grande *g_mult(const Grande *a, const Grande *b)
{
    Grande *r = g_nuevo(a->n + b->n);
    for (size_t i = 0; i < a->n; i++) {
        uint64_t acarreo = 0;
        for (size_t j = 0; j < b->n; j++) {
            uint64_t cur = r->d[i + j] + (uint64_t)a->d[i] * b->d[j] + acarreo;
            r->d[i + j] = (uint32_t)(cur % BASE);
            acarreo     = cur / BASE;
        }
        r->d[i + b->n] += (uint32_t)acarreo;
    }
    r->neg = a->neg != b->neg;
    return g_recorta(r);
}

/* ---------- division entera ---------- */

/* Requiere a recortado. Devuelve |a| * m (m < BASE), recortado. */
static Grande *mag_mult_peq(const Grande *a, uint32_t m)
{
    Grande *r = g_nuevo(a->n + 1);
    uint64_t acarreo = 0;
    for (size_t i = 0; i < a->n; i++) {
        uint64_t cur = (uint64_t)a->d[i] * m + acarreo;
        r->d[i] = (uint32_t)(cur % BASE);
        acarreo = cur / BASE;
    }
    r->d[a->n] = (uint32_t)acarreo;
    return g_recorta(r);
}

/* Devuelve (r * BASE + limb), recortado: corre r un limb a la izquierda. */
static Grande *mag_desplaza(const Grande *r, uint32_t limb)
{
    Grande *s = g_nuevo(r->n + 1);
    s->d[0] = limb;
    for (size_t i = 0; i < r->n; i++) s->d[i + 1] = r->d[i];
    return g_recorta(s);
}

/* a / b con division larga en base 10^9. El resultado se trunca hacia cero
   (-7 / 2 = -3). Si b es cero avisa y devuelve NULL.
   Para cada limb del dividendo (de arriba a abajo) se baja el limb al residuo
   y se busca por biseccion el mayor digito q (0 <= q < BASE) con b*q <= residuo. */
Grande *g_div(const Grande *a, const Grande *b)
{
    if (g_es_cero(b)) {
        warning("division entre cero", NULL);
        return NULL;
    }
    Grande *q = g_nuevo(a->n);
    Grande *r = g_nuevo(1);                    /* residuo = 0 */
    for (size_t i = a->n; i-- > 0; ) {
        Grande *t = mag_desplaza(r, a->d[i]);
        g_libera(r);
        r = t;
        uint32_t lo = 0, hi = BASE - 1;
        while (lo < hi) {
            uint32_t mid = lo + (hi - lo + 1) / 2;
            Grande *p = mag_mult_peq(b, mid);
            if (mag_cmp(p, r) <= 0) lo = mid; else hi = mid - 1;
            g_libera(p);
        }
        q->d[i] = lo;
        if (lo) {
            Grande *p = mag_mult_peq(b, lo);
            t = g_recorta(mag_resta(r, p));
            g_libera(p);
            g_libera(r);
            r = t;
        }
    }
    g_libera(r);
    q->neg = a->neg != b->neg;
    return g_recorta(q);
}

/* base^exp por cuadrados repetidos.
   El exponente debe ser >= 0 y caber en unsigned long; si no, devuelve NULL. */
Grande *g_pot(const Grande *base, const Grande *exp)
{
    unsigned long e;
    if (exp->neg) {
        warning("exponente negativo no soportado (el resultado no es entero)", NULL);
        return NULL;
    }
    if (!g_a_ulong(exp, &e)) {
        warning("exponente demasiado grande", NULL);
        return NULL;
    }

    Grande *res = g_nuevo(1);
    res->d[0] = 1;
    Grande *b = g_copia(base);
    for (; e; e >>= 1) {
        if (e & 1) { Grande *t = g_mult(res, b); g_libera(res); res = t; }
        if (e >> 1) { Grande *t = g_mult(b, b);  g_libera(b);   b   = t; }
    }
    g_libera(b);
    return res;
}

/* ---------- predefinidos ---------- */

/* abs(x): valor absoluto */
Grande *g_abs(const Grande *a)
{
    Grande *r = g_copia(a);
    r->neg = 0;
    return r;
}

/* fact(n): n! ; n debe ser >= 0 y caber en unsigned long. NULL si hay error. */
Grande *g_fact(const Grande *a)
{
    unsigned long n;
    if (a->neg) {
        warning("factorial de un numero negativo", NULL);
        return NULL;
    }
    if (!g_a_ulong(a, &n)) {
        warning("argumento demasiado grande", "fact");
        return NULL;
    }
    Grande *res = g_nuevo(1);
    res->d[0] = 1;
    for (unsigned long i = 2; i <= n; i++) {
        Grande *f = g_desde_ulong(i);
        Grande *t = g_mult(res, f);
        g_libera(f);
        g_libera(res);
        res = t;
    }
    return res;
}
