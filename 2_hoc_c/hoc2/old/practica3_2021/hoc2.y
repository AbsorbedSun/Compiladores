%{
#include <math.h>

double mem[26];//arreg. alamac. los valores de las vars.
%}
%union {
	double val; //val es el valor que se va almac. en alguna posi arreglo
	int index; //posicion en el arreglo
}
%token <val> NUMBER
%token <index> VAR
%type <val> expr
%right '='
%left '+' '-'
%left '*' '/'
%left UNARYMINUS
%% /* A continuación las reglas gramaticales y las acciones */
expr:      NUMBER           { $$ = $1;  }
	| VAR               { $$ = mem[$1]; }// yylval.index 0 .. 25
	| VAR '=' expr      { $$ = mem[$1] = $3; }
        | expr '+' expr     { $$ = $1+$3;  }
        | expr '-' expr     { $$ = $1-$3;  }
        | expr '*' expr     { $$ = $1*$3;  }
        | expr '/' expr     { 
		if($3==0.0)
			execerror("division por cero", "");
		$$ = $1/$3; }
        | '(' expr ')'     { $$ = $2;}
	| '-' expr %prec UNARYMINUS { $$= -$2; }
	;
%%

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <setjmp.h>

jmp_buf begin;

char *progname;
int lineno = 1;

void main (int argc, char *argv[]){
	progname=argv[0];
	setjmp(begin);
	signal(SIGFPE, fpecatch);
  	yyparse ();
}

void execerror(char *s, char *t){
	warning(s, t);
	longjmp(begin, 0);
}

void fpecatch()
{
	execerror("excepcion de punto flotante", (char *)0);
}

int yylex (){
  	int c;

  	while ((c = getchar ()) == ' ' || c == '\t')  
  		;
 	if (c == EOF)                            
    		return 0;
  	if (c == '.' || isdigit (c)) {//numeros 
      		ungetc (c, stdin);
      		scanf ("%lf", &yylval.val);
	      	return NUMBER;//tipo de token
    	}
	if(islower(c)){//letras minus
		yylval.index=c-'a';//lexema la posicion correspondiente a la var c en el arreglo
		return VAR;
	}
  	if(c == '\n')
		lineno++;
  	return c;                                
}

void yyerror (char *s)  /* Llamada por yyparse ante un error */
{
	warning(s, (char *) 0);
}

void warning(char *s, char *t)
{
	fprintf (stderr, "%s: %s", progname, s);
	if(t)
		fprintf (stderr, " %s", t);
	fprintf (stderr, "cerca de la linea %d\n", lineno);
}
