# Practica 03 - Calculadora de numeros grandes + tabla de simbolos (hoc3)

## Compilar y probar
    make
    ./calc < pruebas.txt
    make clean

Requiere yacc o byacc y gcc. (Probado con byacc 2.0 20221106.)

## Que se realizo
Se tomo la calculadora de numeros grandes (+ - * ^, menos unario, parentesis)
y se unio a la estructura de hoc3 (hoc.h, hoc3.y, symbol.c, init.c, math.c),
modificando solo lo necesario:

- hoc.h:    tipo Grande; Symbol guarda Grande* (antes double); install recibe Grande*.
- symbol.c: tabla de simbolos como lista ligada (lookup, install, emalloc).
- init.c:   instala predefinidos abs y fact. Se quitaron constantes y funciones
            de double (sin, cos, log, PI, etc.).
- math.c:   aritmetica de Grande (suma, resta, mult, pot) + g_abs y g_fact.
- hoc3.y:   tokens VAR/BLTIN/INDEF, regla asgn, '=' , yylex con lookup/install,
            warning(); se conservan %destructor y YYERROR de la calculadora.
- Makefile: compila y.tab.c symbol.c init.c math.c.

## Nuevas capacidades
- Variables:        x = 5      y = x = 10      x = x + 1
- Variable no definida -> "Error: variable no definida: nombre"
- Funciones:        abs(-7)    fact(30)
- Todo con enteros de tamano ilimitado.

## Notas
- %precedence (solo bison) se cambio por %left UMENOS para compatibilidad con byacc.
- byacc avisa de 1 conflicto shift/reduce (list asgn '\n' vs expr: asgn); viene
  de hoc3, se resuelve con shift y es lo deseado (asignar no imprime).
- Con YYERROR los valores se ponen en NULL tras liberarlos para evitar doble free en byacc.
