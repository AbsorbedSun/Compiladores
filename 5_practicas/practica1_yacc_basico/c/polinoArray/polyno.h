#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAX_GRADO 100

struct termino {
   double coefi;
   //int coefi;
   unsigned expo;
   unsigned band;
} ;
typedef struct termino Termino;

typedef struct {
    //double coeficientes[MAX_GRADO + 1]; 
    double *coeficientes;
    int grado;                           
} Polinomio;

Polinomio **divideWithRemainder(Polinomio *p1, Polinomio *p2);
void imprimirPolinomio(Polinomio p);

Polinomio *inicializarPolinomio() ;
Polinomio *creaPolinomio(int n, double eltos[]) ;

Polinomio *multiply(Polinomio *p1, Polinomio *p2) ;
Polinomio *evaluarPolinomio(Polinomio *p, double x) ;

Polinomio *operBig(Polinomio *p1, Polinomio *p2,
   Polinomio *(*op)(Polinomio *p1, Polinomio *p2) ) ;
Polinomio *add(Polinomio *p1, Polinomio *p2) ;
Polinomio *subtract(Polinomio *p1, Polinomio *p2) ;
Polinomio *sumaB(Polinomio *p1, Polinomio *p2) ;
Polinomio *restaB(Polinomio *p1, Polinomio *p2) ;
Polinomio *multB(Polinomio *p1, Polinomio *p2) ;
Polinomio *divide(Polinomio *p1, Polinomio *p2) ;
Polinomio *modulo(Polinomio *p1, Polinomio *p2) ;
Polinomio **divideWithRemainder(Polinomio *p1, Polinomio *p2); 
Polinomio *derivative(Polinomio *p) ;
double evaluate(Polinomio *p, double x) ;
// Imprime un polinomio de forma legible
void imprimirPolinomio(Polinomio p) ;
// Imprime un polinomio de forma legible
void imprimirPolinomio_(Polinomio p) ;
// Lee un polinomio desde la entrada estándar
void leerPolinomio(Polinomio *p) ;
    

