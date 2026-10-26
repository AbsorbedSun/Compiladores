# Practica 04 - Calculadora de numeros grandes en la maquina de pila de hoc4

## Compilar y probar
    make
    ./calc < pruebas.txt
    make clean

Requiere yacc o byacc (tambien compila con bison -y) y gcc.
Probado con byacc 2.0 20221106 y con GNU Bison, gcc 13, y con
-fsanitize=address,undefined (sin errores ni fugas).

## Que se realizo
Se copiaron los archivos de hoc4 a la carpeta practica4, se copiaron encima los de la
Practica 3 (calculadora de numeros grandes + tabla de simbolos) y se modificaron:

- hoc.h:    Datum.val pasa de double a Grande *; se agregan drop(), g_div(), g_es_cero(),
            execabort() y los prototipos de la maquina de pila.
- code.c:   add, sub, mul, div_, negate, power, bltin, varpush, constpush, eval, assign y
            print trabajan con Grande. La pila es duena de los Grande que contiene;
            un arreglo paralelo (esval[]) permite liberarlos si hay un error a medias.
- hoc4.y:   macros code2/code3, %union { Symbol *sym; Inst *inst; }, tokens NUMERO/VAR/
            BLTIN/INDEF, gramatica que GENERA CODIGO, operador '/', yylex() y main() de hoc4
            (setjmp/initcode/execute, execerror, fpecatch, warning con progname y lineno).
- math.c:   se agrega la division entera g_div() (division larga en base 10^9) y g_es_cero().
- symbol.c, init.c: sin cambios respecto a la Practica 3 (predefinidos abs y fact).
- Makefile: compila y.tab.c code.c symbol.c init.c math.c.

## Lenguaje
    + - * / ^  menos unario, parentesis, variables (x = 5, y = x = 10), abs(x), fact(n)
- '/' es division ENTERA (trunca hacia cero): 7/2 = 3, -7/2 = -3.
- Errores en ejecucion (variable no definida, division entre cero, fact de un negativo,
  exponente negativo o enorme) avisan con "calc: mensaje cerca de la linea N" y se
  continua con la siguiente linea.

## Notas
- byacc/bison avisan de 1 conflicto shift/reduce (list asgn '\n' vs expr: asgn); viene de
  hoc, se resuelve con shift y es lo deseado (asignar no imprime).
- Las constantes numericas quedan instaladas en la tabla de simbolos (como en hoc4).
