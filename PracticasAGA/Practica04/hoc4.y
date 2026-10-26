%{
/* hoc4.y -- calculadora de numeros grandes (+ - * / ^, parentesis) con
 *           variables y predefinidos, ejecutada en una maquina de pila (hoc4).
 *
 * Lo que se TOMO de hoc4: macros code2/code3, %union con Inst *, la gramatica
 *   que GENERA CODIGO en lugar de evaluar, main() con setjmp/initcode/execute,
 *   execerror(), fpecatch(), warning() con progname y lineno.
 * Lo que se CONSERVO de la practica 3: tipo Grande, tokens NUMERO/VAR/BLTIN/
 *   INDEF, yylex para numeros de cualquier tamano, predefinidos abs y fact.
 * Lo que se AGREGO: operador '/' (division entera de Grande).
 * Lo que se QUITO: %type/%destructor y YYERROR (el parser ya no maneja
 *   Grande: los valores viven en la pila de la maquina), constantes PI, E...
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <setjmp.h>
#include "hoc.h"

#define code2(c1, c2)     code(c1); code(c2)
#define code3(c1, c2, c3) code(c1); code(c2); code(c3)

int  yylex(void);
void yyerror(const char *s);
void fpecatch(int e);
%}

%union {
    Symbol *sym;                  /* NUMERO, VAR, BLTIN, INDEF (apunta a la tabla) */
    Inst   *inst;                 /* direccion en prog[] */
}

%token <sym> NUMERO VAR BLTIN INDEF
%right '='
%left  '+' '-'
%left  '*' '/'
%left  UMENOS                     /* menos unario: -2^2 = -(2^2) */
%right '^'

%%
list:   /* vacio */
    | list '\n'
    | list asgn '\n'   { code2(drop, STOP); return 1; }     /* asignar no imprime */
    | list expr '\n'   { code2(print, STOP); return 1; }
    | list error '\n'  { yyerrok; }
    ;
asgn: VAR '=' expr     { code3(varpush, (Inst)$1, assign); }
    ;
expr: NUMERO           { code2(constpush, (Inst)$1); }
    | VAR              { code3(varpush, (Inst)$1, eval); }
    | asgn
    | BLTIN '(' expr ')' { code2(bltin, (Inst)$1->u.ptr); }
    | '(' expr ')'
    | expr '+' expr    { code(add); }
    | expr '-' expr    { code(sub); }
    | expr '*' expr    { code(mul); }
    | expr '/' expr    { code(div_); }
    | expr '^' expr    { code(power); }
    | '-' expr %prec UMENOS { code(negate); }
    ;
%%

/* ================= programa principal ================= */

jmp_buf begin;

char *progname;
int  lineno = 1;

int main(int argc, char *argv[])
{
    (void)argc;
    progname = argv[0];
    init();                       /* instala abs y fact en la tabla de simbolos */
    setjmp(begin);
    signal(SIGFPE, fpecatch);
    for (initcode(); yyparse(); initcode())
        execute(prog);
    return 0;
}

void execerror(const char *s, const char *t)   /* error en ejecucion: avisa y regresa */
{
    warning(s, t);
    longjmp(begin, 0);
}

void execabort(void)              /* igual, pero el mensaje ya lo dio quien detecto el error */
{
    longjmp(begin, 0);
}

void fpecatch(int e)
{
    (void)e;
    execerror("excepcion de punto flotante", (char *)0);
}

/* ================= analizador lexico ================= */

int yylex(void)
{
    static int previo = '\n';     /* para cerrar la ultima linea si falta '\n' */
    static int salto  = 0;        /* 1 si el token anterior fue '\n' (se cuenta la linea al seguir) */
    int c;

    if (salto) { lineno++; salto = 0; }
    while ((c = getchar()) == ' ' || c == '\t' || c == '\r')
        ;
    if (c == EOF) {
        if (previo != '\n') { previo = '\n'; return '\n'; }
        return 0;
    }
    previo = c;

    if (isdigit(c)) {             /* numero de cualquier tamano */
        size_t n = 0, cap = 64;
        char *s = emalloc(cap);
        Grande *g;
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
        g = g_desde_cadena(s);
        free(s);
        yylval.sym = install("", NUMERO, g);    /* como hoc4: la constante vive en la tabla */
        return NUMERO;
    }

    if (isalpha(c)) {             /* identificador: VAR, BLTIN o INDEF */
        Symbol *s;
        char sbuf[256], *p = sbuf;
        do {
            if (p < sbuf + sizeof sbuf - 1)     /* no desbordar el buffer */
                *p++ = (char)c;
        } while ((c = getchar()) != EOF && isalnum(c));
        ungetc(c, stdin);
        *p = '\0';
        if ((s = lookup(sbuf)) == (Symbol *)NULL)
            s = install(sbuf, INDEF, NULL);     /* primera vez que se ve: indefinida */
        yylval.sym = s;
        return s->type == INDEF ? VAR : s->type;    /* VAR o BLTIN */
    }

    if (c == '\n')
        salto = 1;
    return c;                     /* + - * / ^ = ( ) \n; lo demas lo rechaza el parser */
}

void warning(const char *s, const char *t)
{
    fprintf(stderr, "%s: %s", progname, s);
    if (t)
        fprintf(stderr, " %s", t);
    fprintf(stderr, " cerca de la linea %d\n", lineno);
}

void yyerror(const char *s)       /* llamada por yyparse ante un error */
{
    warning(s, (char *)0);
}
