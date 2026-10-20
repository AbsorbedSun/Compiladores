# Practica 03 - Calculadora de numeros grandes + tabla de simbolos (hoc3) con lex

## Compilar y probar
    make
    ./hoc3                 # modo interactivo
    ./hoc3 < pruebas.txt   # casos de prueba
    make clean

Requiere gcc, yacc o byacc, y lex/flex (se necesita flex por la regla <<EOF>> de lex.l).
Probado con byacc 2.0 (20221106) y flex 2.6.4.

## Archivos
- hoc.h     Tipo Grande (signo + magnitud en base 10^9) y Symbol; prototipos.
- hoc3.y    Gramatica yacc: tokens VAR/BLTIN/INDEF, regla asgn, '=', warning(), main().
- lex.l     Analizador lexico (lex/flex): NUMERO, identificadores (lookup/install), '\n'.
- symbol.c  Tabla de simbolos (lista ligada): lookup, install, emalloc.
- init.c    init(): instala los predefinidos abs y fact.
- math.c    Aritmetica de Grande: suma, resta, multiplicacion, potencia, abs, fact.
- Makefile  yacc -d hoc3.y ; lex lex.l ; gcc.
- pruebas.txt  Casos de prueba.

## Que cambia respecto a la version anterior (yylex a mano)
- yylex() salio de hoc3.y y ahora esta en lex.l, generado con lex/flex.
- lex.l ignora tambien '\r' (archivos con saltos de linea CRLF).
- lex.l cierra la ultima linea con <<EOF>> si el archivo no termina en '\n'.
- Los identificadores ya no tienen limite de longitud (antes 255).
- El Makefile compila por objetos (-Wall -g) y el ejecutable se llama hoc3.

## Capacidades
- Enteros de tamano ilimitado: + - * ^, menos unario, parentesis, resta con negativos.
- Variables: x = 5   y = x = 10   x = x + 1   (una asignacion sola no imprime)
- Predefinidos: abs(x), fact(n)
- Errores: variable no definida, exponente negativo, factorial de negativo, sintaxis;
  tras un error el programa continua con la siguiente linea.

## Notas
- %precedence (solo bison) se cambio por %left UMENOS para compatibilidad con byacc.
- byacc avisa de 1 conflicto shift/reduce (list asgn '\n' vs expr: asgn); viene de hoc3,
  se resuelve con shift y es lo deseado (asignar no imprime).
- Con YYERROR los valores se ponen en NULL tras liberarlos para evitar doble free en byacc.
