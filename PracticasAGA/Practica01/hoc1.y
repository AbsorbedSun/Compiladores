%{
/* Calculadora de numeros grandes: + - * ^, menos unario y parentesis.
 *
 * Los numeros son del tipo Grande: signo + magnitud en base 10^9
 * (arreglo de "limbs" de 32 bits, el menos significativo primero).
 * No hay limite de tamano salvo la memoria disponible.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <limits.h>

#define BASE 1000000000u          /* 10^9: 9 digitos decimales por limb */

typedef struct {
    int       neg;                /* 1 si es negativo (el cero nunca es negativo) */
    size_t    n;                  /* numero de limbs en uso (n >= 1)              */
    uint32_t *d;                  /* d[0] = menos significativo; d[n-1] != 0
                                     salvo que el numero sea 0                    */
} Grande;

int      yylex(void);
void     yyerror(const char *s);

Grande  *g_desde_cadena(const char *s);
void     g_imprime(const Grande *g);
void     g_libera(Grande *g);
Grande  *g_negado(const Grande *a);
Grande  *g_suma(const Grande *a, const Grande *b);
Grande  *g_resta(const Grande *a, const Grande *b);
Grande  *g_mult(const Grande *a, const Grande *b);
Grande  *g_pot(const Grande *base, const Grande *exp);   /* NULL si hay error */
%}

%union { Grande *num; }
%token <num> NUMERO
%type  <num> expr

/* Si el parser descarta un valor durante la recuperacion de errores,
   se libera su memoria. */
%destructor { g_libera($$); } <num>

%left  '+' '-'
%left  '*'
%precedence UMENOS                    /* menos unario: -2^2 = -(2^2) */
%right '^'

%%
entrada: /* vacio */
       | entrada linea
       ;

linea: '\n'
     | expr '\n'  { g_imprime($1); putchar('\n'); g_libera($1); }
     | error '\n' { yyerrok; }
     ;

expr: NUMERO
    | expr '+' expr { $$ = g_suma($1, $3);  g_libera($1); g_libera($3); }
    | expr '-' expr { $$ = g_resta($1, $3); g_libera($1); g_libera($3); }
    | expr '*' expr { $$ = g_mult($1, $3);  g_libera($1); g_libera($3); }
    | expr '^' expr { $$ = g_pot($1, $3);   g_libera($1); g_libera($3);
                      if (!$$) YYERROR; }
    | '-' expr %prec UMENOS { $$ = g_negado($2); g_libera($2); }
    | '(' expr ')'  { $$ = $2; }
    ;
%%

/* ================= utilidades ================= */

static void *reserva(size_t n)
{
    void *p = malloc(n ? n : 1);
    if (!p) { fprintf(stderr, "Error: memoria insuficiente\n"); exit(EXIT_FAILURE); }
    return p;
}

/* ================= tipo Grande ================= */

/* Crea un Grande con n limbs, todos en cero. */
static Grande *g_nuevo(size_t n)
{
    Grande *g = reserva(sizeof *g);
    g->neg = 0;
    g->n   = n;
    g->d   = calloc(n, sizeof *g->d);
    if (!g->d) { fprintf(stderr, "Error: memoria insuficiente\n"); exit(EXIT_FAILURE); }
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

static Grande *g_copia(const Grande *a)
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

/* base^exp por cuadrados repetidos.
   El exponente debe ser >= 0 y caber en unsigned long; si no, devuelve NULL. */
Grande *g_pot(const Grande *base, const Grande *exp)
{
    if (exp->neg) {
        yyerror("exponente negativo no soportado (el resultado no es entero)");
        return NULL;
    }
    unsigned long e = 0;
    for (size_t i = exp->n; i-- > 0; ) {
        if (e > (ULONG_MAX - exp->d[i]) / BASE) {
            yyerror("exponente demasiado grande");
            return NULL;
        }
        e = e * BASE + exp->d[i];
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

/* ================= analizador lexico ================= */

int yylex(void)
{
    static int previo = '\n';         /* para cerrar la ultima linea si falta '\n' */
    int c;
    while ((c = getchar()) == ' ' || c == '\t' || c == '\r')
        ;
    if (c == EOF) {
        if (previo != '\n') { previo = '\n'; return '\n'; }
        return 0;
    }
    previo = c;

    if (isdigit(c)) {
        size_t n = 0, cap = 64;
        char *s = reserva(cap);
        do {
            if (n + 2 > cap) {
                cap *= 2;
                s = realloc(s, cap);
                if (!s) { fprintf(stderr, "Error: memoria insuficiente\n"); exit(EXIT_FAILURE); }
            }
            s[n++] = (char)c;
            c = getchar();
        } while (isdigit(c));
        ungetc(c, stdin);
        s[n] = '\0';
        yylval.num = g_desde_cadena(s);
        free(s);
        return NUMERO;
    }
    return c;   /* + - * ^ ( ) \n; cualquier otro caracter lo rechaza el parser */
}

void yyerror(const char *s) { fprintf(stderr, "Error: %s\n", s); }

int main(void)
{
    yyparse();
    return 0;
}
