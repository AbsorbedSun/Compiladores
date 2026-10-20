%{
/* lex.l -- analizador lexico (generado con lex/flex)
 *
 * Devuelve al parser:
 *   NUMERO  secuencia de digitos de cualquier longitud (valor: Grande *)
 *   VAR     identificador que es variable o que aun no esta definido
 *   BLTIN   identificador que es funcion predefinida (abs, fact)
 *   '\n' y cualquier otro caracter tal cual (+ - * ^ = ( ) ...); el parser
 *   rechaza los que no pertenecen al lenguaje.
 *
 * Los identificadores se buscan en la tabla de simbolos con lookup(); si es
 * la primera vez que aparecen se instalan como INDEF (igual que en hoc3).
 */
#include <stdio.h>
#include <stdlib.h>
#include "hoc.h"
#include "y.tab.h"                /* codigos de NUMERO, VAR, BLTIN, INDEF y yylval */

static int previo = '\n';         /* ultimo token devuelto: sirve para cerrar la
                                     ultima linea si el archivo no termina en '\n' */
%}

%option noyywrap nounput noinput

DIGITO      [0-9]
LETRA       [a-zA-Z]

%%

[ \t\r]+                { /* se ignoran espacios, tabuladores y '\r' (archivos CRLF) */ }

{DIGITO}+               {
                            yylval.num = g_desde_cadena(yytext);   /* sin limite de tamano */
                            previo = NUMERO;
                            return NUMERO;
                        }

{LETRA}({LETRA}|{DIGITO})* {
                            Symbol *s;
                            if ((s = lookup(yytext)) == NULL)
                                s = install(yytext, INDEF, NULL);  /* primera vez que se ve */
                            yylval.sym = s;
                            previo = (s->type == INDEF) ? VAR : s->type;
                            return previo;                          /* VAR o BLTIN */
                        }

\n                      { previo = '\n'; return '\n'; }

<<EOF>>                 {
                            if (previo != '\n') { previo = '\n'; return '\n'; }
                            return 0;                               /* fin de la entrada */
                        }

.                       { previo = yytext[0]; return yytext[0]; }

%%
