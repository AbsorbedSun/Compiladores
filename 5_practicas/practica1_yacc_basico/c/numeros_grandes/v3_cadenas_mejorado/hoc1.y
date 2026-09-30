%{
/* Calculadora de numeros grandes: + * ^ y parentesis.
   Los numeros son cadenas de digitos decimales (sin limite de tamano). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int   yylex(void);
void  yyerror(const char *s);
char *suma(const char *a, const char *b);
char *multiplicacion(const char *a, const char *b);
char *exponenciacion(const char *base, const char *exp);
%}

%union { char *str; }
%token <str> NUMERO
%type  <str> expr

%left  '+'
%left  '*'
%right '^'

%%
entrada: /* vacio */
       | entrada linea
       ;

linea: '\n'
     | expr '\n'  { printf("%s\n", $1); free($1); }
     | error '\n' { yyerrok; }
     ;

expr: NUMERO
    | expr '+' expr { $$ = suma($1, $3);           free($1); free($3); }
    | expr '*' expr { $$ = multiplicacion($1, $3); free($1); free($3); }
    | expr '^' expr { $$ = exponenciacion($1, $3); free($1); free($3); }
    | '(' expr ')'  { $$ = $2; }
    ;
%%

/* ---------- utilidades ---------- */

static void *reserva(size_t n)
{
    void *p = malloc(n);
    if (!p) { fprintf(stderr, "Error: memoria insuficiente\n"); exit(EXIT_FAILURE); }
    return p;
}

/* Quita ceros a la izquierda (deja al menos un digito). Modifica y devuelve s. */
static char *normaliza(char *s)
{
    char *p = s;
    while (p[0] == '0' && p[1]) p++;
    memmove(s, p, strlen(p) + 1);
    return s;
}

/* ---------- analizador lexico ---------- */

int yylex(void)
{
    int c;
    while ((c = getchar()) == ' ' || c == '\t' || c == '\r')
        ;
    if (c == EOF) return 0;

    if (isdigit(c)) {
        size_t n = 0, cap = 64;
        char *s = reserva(cap);
        do {
            if (n + 2 > cap) { cap *= 2; s = realloc(s, cap); if (!s) exit(EXIT_FAILURE); }
            s[n++] = c;
            c = getchar();
        } while (isdigit(c));
        ungetc(c, stdin);
        s[n] = '\0';
        yylval.str = normaliza(s);
        return NUMERO;
    }
    return c;   /* + * ^ ( ) \n; cualquier otro caracter lo rechaza el parser */
}

/* ---------- aritmetica sobre cadenas de digitos ---------- */

char *suma(const char *a, const char *b)
{
    size_t la = strlen(a), lb = strlen(b), max = la > lb ? la : lb;
    char *r = reserva(max + 2);
    int acarreo = 0;
    size_t k = max + 1;               /* posicion de escritura (de derecha a izquierda) */
    r[k] = '\0';
    for (size_t i = 0; i < max || acarreo; i++) {
        int s = acarreo;
        if (i < la) s += a[la - 1 - i] - '0';
        if (i < lb) s += b[lb - 1 - i] - '0';
        r[--k] = '0' + s % 10;
        acarreo = s / 10;
    }
    memmove(r, r + k, max + 2 - k);   /* incluye el '\0' */
    return r;
}

char *multiplicacion(const char *a, const char *b)
{
    size_t la = strlen(a), lb = strlen(b);
    int *t = calloc(la + lb, sizeof *t);
    if (!t) { fprintf(stderr, "Error: memoria insuficiente\n"); exit(EXIT_FAILURE); }

    for (size_t i = la; i-- > 0; )
        for (size_t j = lb; j-- > 0; ) {
            int total = (a[i] - '0') * (b[j] - '0') + t[i + j + 1];
            t[i + j + 1] = total % 10;
            t[i + j]    += total / 10;
        }

    char *r = reserva(la + lb + 1);
    size_t i = 0, k = 0;
    while (i < la + lb - 1 && t[i] == 0) i++;      /* deja al menos un digito */
    while (i < la + lb) r[k++] = '0' + t[i++];
    r[k] = '\0';
    free(t);
    return r;
}

/* base^exp por cuadrados repetidos; el exponente debe caber en unsigned long */
char *exponenciacion(const char *base, const char *exp)
{
    unsigned long e = strtoul(exp, NULL, 10);
    char *res = strcpy(reserva(2), "1");
    char *b   = strcpy(reserva(strlen(base) + 1), base);

    for (; e; e >>= 1) {
        if (e & 1) { char *t = multiplicacion(res, b); free(res); res = t; }
        if (e >> 1) { char *t = multiplicacion(b, b);  free(b);   b   = t; }
    }
    free(b);
    return res;
}

void yyerror(const char *s) { fprintf(stderr, "Error: %s\n", s); }

int main(void)
{
    yyparse();
    return 0;
}
