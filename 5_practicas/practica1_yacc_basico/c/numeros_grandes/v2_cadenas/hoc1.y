%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *suma(char *a, char *b);
char *multiplicacion(char *a, char *b);
char *exponenciacion(char *base, char *exponente);

int yylex(void);
int yyparse(void);
void yyerror(const char *s);
%}

%union {
    char *str;
}

%token <str> NUMERO

%type <str> expresion

%left '+' '-'
%left '*'
%right '^'

%%

entrada:
      expresion '\n'
      {
          printf("%s\n", $1);
          free($1);
      }
    ;

expresion:
      NUMERO
      {
          $$ = $1;
      }

    | expresion '+' expresion
      {
          $$ = suma($1, $3);
          free($1);
          free($3);
      }

    | expresion '*' expresion
      {
          $$ = multiplicacion($1, $3);
          free($1);
          free($3);
      }

    | expresion '^' expresion
      {
          $$ = exponenciacion($1, $3);
          free($1);
          free($3);
      }

    | '(' expresion ')'
      {
          $$ = $2;
      }
    ;

%%

/*
   ANALIZADOR LÉXICO*/

int yylex(void)
{
    int c;
    char buffer[1024];
    int i = 0;

    /* Ignorar espacios */
    do {
        c = getchar();
    } while (c == ' ' || c == '\t');

    /* Fin de archivo */
    if (c == EOF)
        return 0;

    /* Número */
    if (c >= '0' && c <= '9') {

        do {
            if (i < 1023)
                buffer[i++] = c;

            c = getchar();

        } while (c >= '0' && c <= '9');

        buffer[i] = '\0';

        ungetc(c, stdin);

        yylval.str = malloc(strlen(buffer) + 1);

        if (yylval.str == NULL) {
            fprintf(stderr, "Error: memoria insuficiente\n");
            exit(EXIT_FAILURE);
        }

        strcpy(yylval.str, buffer);

        return NUMERO;
    }

    /* Operadores y paréntesis */
    if (c == '+' ||
        c == '*' ||
        c == '^' ||
        c == '(' ||
        c == ')' ||
        c == '\n') {

        return c;
    }

    fprintf(stderr, "Caracter no válido: %c\n", c);

    return yylex();
}


/*
   SUMA*/

char *suma(char *a, char *b)
{
    int lenA = strlen(a);
    int lenB = strlen(b);

    int max = (lenA > lenB ? lenA : lenB);

    char *resultado = malloc(max + 2);

    int i = lenA - 1;
    int j = lenB - 1;
    int k = max;
    int acarreo = 0;

    resultado[k + 1] = '\0';

    while (i >= 0 || j >= 0 || acarreo) {

        int da = (i >= 0) ? a[i] - '0' : 0;
        int db = (j >= 0) ? b[j] - '0' : 0;

        int suma = da + db + acarreo;

        resultado[k--] = (suma % 10) + '0';
        acarreo = suma / 10;

        i--;
        j--;
    }

    /* Mover resultado para eliminar espacio sobrante */
    memmove(resultado, resultado + k + 1, max - k + 1);

    return resultado;
}


/*
   MULTIPLICACIÓN*/

char *multiplicacion(char *a, char *b)
{
    int lenA = strlen(a);
    int lenB = strlen(b);

    if ((lenA == 1 && a[0] == '0') ||
        (lenB == 1 && b[0] == '0')) {

        char *cero = malloc(2);
        strcpy(cero, "0");
        return cero;
    }

    int *temp = calloc(lenA + lenB, sizeof(int));

    if (temp == NULL) {
        fprintf(stderr, "Error: memoria insuficiente\n");
        exit(EXIT_FAILURE);
    }

    for (int i = lenA - 1; i >= 0; i--) {

        for (int j = lenB - 1; j >= 0; j--) {

            int producto =
                (a[i] - '0') *
                (b[j] - '0');

            int pos1 = i + j;
            int pos2 = i + j + 1;

            int total = producto + temp[pos2];

            temp[pos2] = total % 10;
            temp[pos1] += total / 10;
        }
    }

    char *resultado = malloc(lenA + lenB + 1);

    int i = 0;

    while (i < lenA + lenB && temp[i] == 0)
        i++;

    int k = 0;

    while (i < lenA + lenB)
        resultado[k++] = temp[i++] + '0';

    resultado[k] = '\0';

    free(temp);

    return resultado;
}


/*
   EXPONENCIACIÓN*/

char *exponenciacion(char *base, char *exponente)
{
    /*
       Esta función necesita trabajar con exponentes grandes.
       Para una primera versión podemos convertir el exponente
       a unsigned long long.
    */

    unsigned long long exp =
        strtoull(exponente, NULL, 10);

    char *resultado = malloc(2);

    strcpy(resultado, "1");

    char *b = malloc(strlen(base) + 1);
    strcpy(b, base);

    while (exp > 0) {

        if (exp % 2 == 1) {

            char *temp = multiplicacion(resultado, b);

            free(resultado);

            resultado = temp;
        }

        exp /= 2;

        if (exp > 0) {

            char *temp = multiplicacion(b, b);

            free(b);

            b = temp;
        }
    }

    free(b);

    return resultado;
}


/*
   ERROR*/

void yyerror(const char *s)
{
    fprintf(stderr, "Error: %s\n", s);
}


/*
   MAIN*/

int main(void)
{
    printf("Calculadora de numeros grandes\n");
    printf("Escribe una expresion:\n");

    yyparse();

    return 0;
}
