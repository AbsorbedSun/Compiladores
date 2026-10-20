%{
/* hoc3.y -- calculadora de numeros grandes (+ - * ^, parentesis) con
 *           variables y funciones predefinidas, al estilo hoc3.
 *           El analizador lexico esta en lex.l (generado con lex/flex).
 *
 * Lo que se TOMO de hoc3:  union con Symbol*, tokens VAR/BLTIN/INDEF,
 *   regla asgn, '=' como operador, tabla de simbolos (lookup/install),
 *   warning() e init().
 * Lo que se CONSERVO de la calculadora: tipo Grande, %destructor,
 *   recuperacion de errores con YYERROR.
 * Lo que se QUITO de hoc3: double, '/', setjmp/longjmp (execerror),
 *   SIGFPE, lineno y progname.
 */
#include <stdio.h>
#include <stdlib.h>
#include "hoc.h"

int  yylex(void);                 /* lo genera lex a partir de lex.l */
void yyerror(const char *s);
%}

%union {
    Grande *num;                  /* NUMERO, expr, asgn */
    Symbol *sym;                  /* VAR, BLTIN, INDEF  (apunta a la tabla) */
}

%token <num> NUMERO
%token <sym> VAR BLTIN INDEF
%type  <num> expr asgn

/* Si el parser descarta un valor durante la recuperacion de errores,
   se libera su memoria. (Si tu yacc no soporta %destructor, borra estas
   3 lineas: solo se pierde la liberacion de memoria al haber errores.) */
%destructor { g_libera($$); } <num>

%right '='
%left  '+' '-'
%left  '*'
%left  UMENOS                     /* menos unario: -2^2 = -(2^2) */
%right '^'

/* NOTA sobre YYERROR: al ejecutarlo, byacc destruye despues (con %destructor)
   los valores de la regla que fallo, pero bison no. Como ya los liberamos a
   mano, se ponen en NULL antes de YYERROR (g_libera(NULL) no hace nada); asi
   no hay doble liberacion con ninguno de los dos. */

%%
list: /* vacio */
    | list '\n'
    | list asgn '\n'   { g_libera($2); }                 /* asignar no imprime */
    | list expr '\n'   { g_imprime($2); putchar('\n'); g_libera($2); }
    | list error '\n'  { yyerrok; }
    ;

asgn: VAR '=' expr {
          if ($1->type == VAR)
              g_libera($1->u.val);        /* la variable ya tenia valor: se descarta */
          $1->u.val = $3;                 /* la variable se queda con el valor de expr */
          $1->type  = VAR;
          $$ = g_copia($3);               /* asgn tambien es expr: devuelve una copia */
      }
    ;

expr: NUMERO
    | VAR {
          if ($1->type == INDEF) {
              warning("variable no definida", $1->name);
              YYERROR;
          }
          $$ = g_copia($1->u.val);        /* copia: la variable conserva el suyo */
      }
    | asgn
    | BLTIN '(' expr ')' {
          $$ = ($1->u.ptr)($3);           /* llama a abs() o fact() */
          g_libera($3);
          if (!$$) { $3 = NULL; YYERROR; }   /* ver nota sobre YYERROR abajo */
      }
    | expr '+' expr { $$ = g_suma($1, $3);  g_libera($1); g_libera($3); }
    | expr '-' expr { $$ = g_resta($1, $3); g_libera($1); g_libera($3); }
    | expr '*' expr { $$ = g_mult($1, $3);  g_libera($1); g_libera($3); }
    | expr '^' expr { $$ = g_pot($1, $3);   g_libera($1); g_libera($3);
                      if (!$$) { $1 = $3 = NULL; YYERROR; } }
    | '-' expr %prec UMENOS { $$ = g_negado($2); g_libera($2); }
    | '(' expr ')'  { $$ = $2; }
    ;
%%

void warning(const char *s, const char *t)
{
    fprintf(stderr, "Error: %s", s);
    if (t)
        fprintf(stderr, ": %s", t);
    fputc('\n', stderr);
}

void yyerror(const char *s)       /* llamada por yyparse ante un error */
{
    warning(s, NULL);
}

int main(void)
{
    init();                           /* instala abs y fact en la tabla de simbolos */
    yyparse();
    return 0;
}
