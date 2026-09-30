%{
#include <stdio.h>
#include <stdlib.h>
#include "polinomio_calc.h"

//gcc lex.yy.c polinomio_cal.c y.tab.c
// ./a.out (33x0+34x3+56x7)+(22x0+44x2+77x10)
int yylex(void);
int yyerror(const char*);
NodoL *cab;
%}
%union {
   NodoL *val;
   Termino *term;
   Polinomio *polino;
}
%token <term> TERMINO
%type <term> termino
%type <val> terminos
%type <polino> expr poli
%left '+' '-'
%left '*' '/'
%nonassoc '(' ')'
%%
input: /* vacio */ 
     | input line
     ;
line: '\n'
    | expr '\n' { imprimePolinomio($1); }
    ;
poli: '[' terminos ']'       {  
                               $$=creaPolinomio(0, cab, 1);
                             }
    | '['']'                {                
                              $$=(Polinomio *)NULL;
                            }
    ;
terminos: termino            { 
                             cab=NULL; 
                             insertaOrdA($1, & cab, cmpTermino);
                             }
    | termino terminos       { 
                             insertaOrdA($1, & cab, cmpTermino); 
                             }
    ;
termino: TERMINO {  }
    ;
expr : poli 
     | expr '+' expr { $$ = sumaPolinomio($1, $3);
                       simplifica($$);
                     }
     | expr '-' expr { $$ = restaPolinomio($1, $3);
                       simplifica($$);
                     }
     | expr '*' expr { 
                       $$ = multiplicaPolinomio($1, $3); 
                       simplifica($$);
                     }
     | expr '/' expr { $$ = dividePolinomio($1, $3); 
                       simplifica($$);
                     }
     | expr '%' expr { $$ = moduloPolinomio($1, $3); 
                       simplifica($$);
                     }
     | expr '$' expr { $$ = inverso($1, $3); 
                       simplifica($$);
                       //puts("INVINV");
                     }
     ;      
%%
int main() { return yyparse(); }
int yyerror(const char* s) { 
  printf("%s\n", s); 
  return 0; 
}


