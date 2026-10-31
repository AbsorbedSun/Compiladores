# Practica 05 - Decisiones (if) y ciclos (while) en la calculadora de numeros grandes

## Compilar y probar
    make
    ./calc < prueba_if.txt
    ./calc < prueba_while.txt
    ./calc < prueba_errores.txt
    ./calc < pruebas.txt          # regresion de la Practica 4 (misma salida)
    make clean

Requiere yacc, byacc o bison -y, y gcc. Probado con GNU Bison 3.8.2 y gcc 13, y con
-fsanitize=address,undefined (sin errores ni fugas).

## Que se realizo
Se partio de la Practica 4 (que ya incluia lo de la Practica 1 con Grande) y se le agrego
lo de HOC5. Como se eligio la opcion de numeros grandes, "Complejos" del enunciado = Grande.

- hoc.h:    Datum agrega `double dval` (resultado de comparaciones y logicos); prototipos de
            gt, lt, eq, ge, le, ne, and_, or_, not_, tobool, prexpr, ifcode, whilecode;
            g_cmp() y g_desde_ulong().
- math.c:   g_cmp(): comparacion con signo de dos Grande (-1, 0, 1).
- code.c:   gt(), lt(), eq(), ge(), le(), ne() comparan Grande y dejan 1.0/0.0 en dval.
            ifcode() y whilecode() leen la condicion en dval. and_(), or_(), not_(), tobool(),
            prexpr(). drop() (pop1 de hoc5) libera el Grande si lo es.
            popk()/popg()/popbool(): sacan el tope sabiendo si es Grande o dval (esval[]).
- hoc5.y:   tokens IF, ELSE, WHILE, PRINT; GT GE LT LE EQ NE AND OR NOT; tipos <inst> para
            stmt asgn expr stmtlist cond while if end; producciones de if/else, while, bloques
            { }, print, comparaciones y logicos; $$ asignado donde lo pide HOC5; yylex()
            tokeniza > >= < <= == != && || ! con follow().
- init.c:   instala las palabras clave if, else, while, print (ademas de abs y fact).
- symbol.c: sin cambios.   Makefile: ahora genera el parser a partir de hoc5.y.

## Lenguaje (ademas de lo de la Practica 4)
    if (cond) stmt            if (cond) stmt else stmt
    while (cond) stmt         { stmt  stmt ... }        print expr
    > >= < <= == !=           && || !
- cond puede ser una comparacion o cualquier expresion: un Grande distinto de 0 es verdadero.
- Las comparaciones devuelven 1 o 0; si se mezclan con aritmetica o se asignan se convierten
  a Grande:  y = x > 3  deja y = 1  y  (x > 3) + 1  vale 2.
- Un bloque lleva '{' al final de la linea del if/while; el else va en la misma linea que
  el '}' o que el stmt anterior:  if (c) {  ...  } else {  ...  }
- print imprime el valor y salta de linea. Una expresion suelta se imprime con tabulador.

## Notas
- bison/byacc avisan de 5 conflictos shift/reduce; todos vienen de HOC y se resuelven con
  shift, que es lo deseado: asgn/expr y expr/stmt antes de '\n' (asignar no imprime), else
  colgante (el else se une al if mas cercano) y '-' binario/unario al inicio de una stmt.
- Correcciones respecto al hoc5 entregado: '_' por '-' en la precedencia, y initcode() tras
  un error de sintaxis (sin eso el codigo a medias de un if/while se ejecutaba con la
  linea siguiente y el programa terminaba con violacion de segmento).
