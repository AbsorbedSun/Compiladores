%{
/* hoc5.y -- calculadora de numeros grandes con decisiones (if/else) y ciclos
 *           (while), ejecutada en una maquina de pila (hoc5).
 *
 * Lo que se TOMO de hoc5: tokens IF, ELSE, WHILE, PRINT; los simbolos no
 *   terminales stmt, stmtlist, cond, while, if y end; las producciones de
 *   if/else, while, bloques { }, print; los operadores de comparacion
 *   (> >= < <= == !=) y logicos (&& || !); follow() en yylex().
 * Lo que se CONSERVO de la practica 4: tipo Grande, tokens NUMERO/VAR/BLTIN/
 *   INDEF, yylex para numeros de cualquier tamano, '/', abs y fact.
 * Lo que se CORRIGIO respecto al hoc5 entregado:
 *   - en la declaracion de precedencia aparecia '_' en lugar de '-'
 *   - tras un error de sintaxis se llama a initcode(): sin eso el codigo a
 *     medias de un if/while (con sus STOP sin rellenar) se ejecutaba junto con
 *     la linea siguiente y el programa terminaba con una violacion de segmento
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
int  follow(int expect, int ifyes, int ifno);
void yyerror(const char *s);
void fpecatch(int e);
%}

%union {
    Symbol *sym;                  /* NUMERO, VAR, BLTIN, INDEF, PRINT, IF, ELSE, WHILE (apunta a la tabla) */
    Inst   *inst;                 /* direccion en prog[] */
}

%token <sym>  NUMERO PRINT VAR BLTIN INDEF WHILE IF ELSE
%type  <inst> stmt asgn expr stmtlist cond while if end

%right '='
%left  OR
%left  AND
%left  GT GE LT LE EQ NE
%left  '+' '-'
%left  '*' '/'
%left  UMENOS NOT                 /* menos unario: -2^2 = -(2^2) */
%right '^'

%%
list:   /* vacio */
    | list '\n'
    | list asgn '\n'   { code2(drop, STOP); return 1; }     /* asignar no imprime */
    | list stmt '\n'   { code(STOP); return 1; }
    | list expr '\n'   { code2(print, STOP); return 1; }
    | list error '\n'  { yyerrok; initcode(); }   /* descartar el codigo a medias de la linea erronea */
    ;
asgn: VAR '=' expr     { $$ = $3; code3(varpush, (Inst)$1, assign); }
    ;
stmt: expr             { $$ = $1; code(drop); }             /* descarta el valor (y lo libera) */
    | PRINT expr       { $$ = $2; code(prexpr); }
    | while cond stmt end {
            ($1)[1] = (Inst)$3;   /* cuerpo de la iteracion */
            ($1)[2] = (Inst)$4;   /* terminar si la condicion no se cumple */
        }
    | if cond stmt end {          /* if sin else */
            ($1)[1] = (Inst)$3;   /* parte then */
            ($1)[3] = (Inst)$4;   /* terminar si la condicion no se cumple */
        }
    | if cond stmt end ELSE stmt end {    /* if con parte else */
            ($1)[1] = (Inst)$3;   /* parte then */
            ($1)[2] = (Inst)$6;   /* parte else */
            ($1)[3] = (Inst)$7;   /* terminar */
        }
    | '{' stmtlist '}' { $$ = $2; }
    ;
cond: '(' expr ')'     { code2(tobool, STOP); $$ = $2; }    /* la condicion deja un dval */
    ;
while: WHILE           { $$ = code3(whilecode, STOP, STOP); }
    ;
if:   IF               { $$ = code(ifcode); code3(STOP, STOP, STOP); }
    ;
end:  /* nada */       { code(STOP); $$ = progp; }
    ;
stmtlist: /* nada */   { $$ = progp; }
    | stmtlist '\n'    { $$ = $1; }
    | stmtlist stmt    { $$ = $1; }
    ;
expr: NUMERO           { $$ = code2(constpush, (Inst)$1); }
    | VAR              { $$ = code3(varpush, (Inst)$1, eval); }
    | asgn             { $$ = $1; }
    | BLTIN '(' expr ')' { $$ = $3; code2(bltin, (Inst)$1->u.ptr); }
    | '(' expr ')'     { $$ = $2; }
    | expr '+' expr    { code(add); }
    | expr '-' expr    { code(sub); }
    | expr '*' expr    { code(mul); }
    | expr '/' expr    { code(div_); }
    | expr '^' expr    { code(power); }
    | '-' expr %prec UMENOS { $$ = $2; code(negate); }
    | expr GT expr     { code(gt); }
    | expr GE expr     { code(ge); }
    | expr LT expr     { code(lt); }
    | expr LE expr     { code(le); }
    | expr EQ expr     { code(eq); }
    | expr NE expr     { code(ne); }
    | expr AND expr    { code(and_); }
    | expr OR expr     { code(or_); }
    | NOT expr         { $$ = $2; code(not_); }
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

    switch (c) {                  /* operadores de comparacion y logicos (hoc5) */
    case '>':  return follow('=', GE, GT);
    case '<':  return follow('=', LE, LT);
    case '=':  return follow('=', EQ, '=');
    case '!':  return follow('=', NE, NOT);
    case '|':  return follow('|', OR, '|');
    case '&':  return follow('&', AND, '&');
    case '\n': salto = 1; return '\n';
    default:   return c;          /* + - * / ^ ( ) { } ; lo demas lo rechaza el parser */
    }
}

int follow(int expect, int ifyes, int ifno)   /* buscar >=, <=, ==, !=, ||, && */
{
    int c = getchar();
    if (c == expect)
        return ifyes;
    ungetc(c, stdin);
    return ifno;
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
