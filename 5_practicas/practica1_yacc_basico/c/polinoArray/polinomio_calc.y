%{
#include <stdio.h>
#include <stdlib.h>
#include "polyno.h"

#define TAM 1000
//gcc lex.yy.c polinomio_cal.c y.tab.c
// ./a.out (33x0+34x3+56x7)+(22x0+44x2+77x10)
int yylex(void);
int yyerror(const char*);
double coef[TAM];
int cta=0;
//NodoL *cab;
%}
%union {
   int val;
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
    | expr '\n' { imprimirPolinomio(*$1);}
    ;
poli: '[' terminos ']'       { 
             /*for (int j = 0; j < cta+1; ++j) 
                  printf("%.4g j=%d\n", coef[j],j);
                printf("\n");*/
                               $$=creaPolinomio(cta, coef );
                               for (int j = 0; j < TAM; ++j) 
                                  coef[j]=0;
                             }
    | '['']'                {                
                              $$=(Polinomio *)NULL;
                            }
    ;
terminos: termino            { 
        //printf("term %+.4g x^%d\n", $1->coefi, $1->expo);
                   //exit(0);
                             coef[$1->expo]=$1->coefi; 
                             cta=$1->expo;
                             }
    | termino terminos       {     
      //printf("terms %+.4g x^%d\n", $1->coefi, $1->expo);
                             if( $1->expo > cta)
                               cta=$1->expo;
                             coef[$1->expo]=$1->coefi;
                             }
    ;
termino: TERMINO { 
                   $$ = $1;  
           //printf("term %+.4g x^%d\n", $1->coefi, $1->expo);
                   //exit(0);         
                 }
    ;
expr : poli 
     | expr '+' expr { $$ = add($1, $3);
                       //simplifica($$);
                     }
     | expr '-' expr { $$ = subtract($1, $3);
                       //simplifica($$);
                     }
     | expr '*' expr { 
                       //puts("multi");
                       //imprimirPolinomio(*$1);
                       //imprimirPolinomio(*$3);
                       $$ = multiply($1, $3); 
                       //simplifica($$);
                     }
     | expr '/' expr { $$ = divide($1, $3); 
                       //simplifica($$);
                     }
     | expr '%' expr { $$ = modulo($1, $3); 
                       //simplifica($$);
                     }
     ;      
%%
int main() { return yyparse(); }
int yyerror(const char* s) { 
  printf("%s\n", s); 
  return 0; 
}


