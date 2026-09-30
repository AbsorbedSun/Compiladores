#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "complejo_calc.h"

void demo_operations();

int main() { demo_operations(); }
int yyerror(const char* s) { 
  printf("%s\n", s); 
  return 0; 
}
Complejo *creaComplejo(int real, int img){
   Complejo *nvo = (Complejo*)malloc(sizeof(Complejo));
   nvo -> real = real;
   nvo -> img = img;
   return nvo;
}
Complejo *Complejo_add(Complejo *c1, Complejo *c2){
  return creaComplejo(c1->real + c2->real, c1->img + c2->img);
}
Complejo *Complejo_sub(Complejo *c1, Complejo *c2){
  return creaComplejo(c1->real - c2->real, c1->img - c2->img);
}
Complejo *Complejo_mul(Complejo *c1, Complejo *c2){
  return creaComplejo( c1->real*c2->real - c1->img*c2->img,
                       c1->img*c2->real + c1->real*c2->img);
}
Complejo *Complejo_div(Complejo *c1, Complejo *c2){
   double d = c2->real*c2->real + c2->img*c2->img;
   return creaComplejo( (c1->real*c2->real + c1->img*c2->img) / d,
                        (c1->img*c2->real - c1->real*c2->img) / d);
}

Complejo *Complejo_powPolar(Complejo *a, Complejo *b) {
    // a^b = exp(b * ln(a))
    // Calcular módulo y argumento de a
    double r = sqrt(a->real * a->real + a->img * a->img);
    double theta = atan2(a->img, a->real);
    
    // ln(a) = ln(r) + i*theta
    Complejo *ln_a = creaComplejo(log(r), theta);
    
    // b * ln(a)
    Complejo *product = Complejo_mul(b, ln_a);
    
    // exp(b * ln(a))
    double exp_real = exp(product->real);
    return creaComplejo(exp_real * cos(product->img),
                        exp_real * sin(product->img));
}

Complejo *Complejo_sqrtPolar(Complejo *a) {
    // sqrt(z) = sqrt(r) * (cos(theta/2) + i*sin(theta/2))
    double r = sqrt(a->real * a->real + a->img * a->img);
    double theta = atan2(a->img, a->real);
    return creaComplejo(sqrt(r)* cos(theta / 2),
                        sqrt(r)* sin(theta / 2));
}

Complejo *Complejo_pow(Complejo *base, int expo) {
    Complejo *resultado;  
    // Caso exponente 0
    if (expo == 0) return creaComplejo(1.0, 0.0);
    // Inicializar con la base
    resultado = base;
    for (int i = 1; i < expo; i++) {
        resultado = Complejo_mul(base, resultado);
    }
    return resultado;
}

//void raiz_cuadrada_formula_clasica(double real, double imag, double complex *raiz1, double complex *raiz2) {
Complejo **Complejo_sqrt(Complejo *a) {
    Complejo **raices = (Complejo **)malloc(2*sizeof(Complejo*));
    // Calculamos el módulo
    double modulo = sqrt(a->real * a->real + 
                         a->img * a->img);
    // Calculamos la parte real de la raíz usando la fórmula: sqrt((modulo + real)/2)
    double A = sqrt((modulo + a->real) / 2.0);
    
    // Calculamos la parte imaginaria usando: signo(imag) * sqrt((modulo - real)/2)
    double B = (a->img >= 0) ? 1.0 : -1.0;
    B *= sqrt((modulo - a->real) / 2.0);
    
    // Las dos raíces
    raices[0] = creaComplejo(A, B);
    raices[1] = creaComplejo(-A, -B);
    return raices;
}

Complejo *Complejo_log(Complejo *a) {
    // ln(z) = ln|z| + i*arg(z)
    double r = sqrt(a->real * a->real + a->img * a->img);
    double theta = atan2(a->img, a->real);    
    return creaComplejo(log(r), theta);
}

Complejo *Complejo_exp(Complejo *a) {
    // exp(z) = exp(real) * (cos(imag) + i*sin(imag))
    return creaComplejo(exp(a->real) * cos(a->img),
                        exp(a->real) * sin(a->img));
}

Complejo *Complejo_sin(Complejo *a) {
    // sin(z) = sin(real)*cosh(imag) + i*cos(real)*sinh(imag)
    return creaComplejo(sin(a->real) * cosh(a->img),
                        cos(a->real) * sinh(a->img));
}
Complejo *Complejo_cos(Complejo *a) {
     // cos(z) = cos(real)*cosh(imag) - i*sin(real)*sinh(imag)
    return creaComplejo(cos(a->real) * cosh(a->img),
                        -sin(a->real) * sinh(a->img));
}
Complejo *Complejo_tan(Complejo *a) {
    return Complejo_div(Complejo_sin(a),
                        Complejo_cos(a));
}
Complejo *Complejo_cot(Complejo *a) {
    return Complejo_div(Complejo_cos(a),
                        Complejo_sin(a));
}
Complejo *Complejo_sec(Complejo *a) {
     // sec(z) = 1 / cos(z)
    Complejo *one = creaComplejo(1,0);
    return Complejo_div(one,
                        Complejo_cos(a));
}
Complejo *Complejo_csc(Complejo *a) {
     // sec(z) = 1 / sin(z)
    Complejo *one = creaComplejo(1,0);
    return Complejo_div(one,
                        Complejo_sin(a));
}

void imprimirC(Complejo *c){
   if(c->img != 0)
      printf("%f%+fi\n", c->real, c->img);
   else
      printf("%f\n", c->real);
}

void print_complex(Complejo c) {
    if (isnan(c.real) || isnan(c.img)) {
        printf("Error: Resultado no definido\n");
        return;
    }
    
    // Formatear la parte real
    if (fabs(c.real) < 1e-10) c.real = 0;
    if (fabs(c.img) < 1e-10) c.img = 0;
    
    if (c.img == 0) {
        printf("%.6f", c.real);
    } else if (c.real == 0) {
        if (c.img == 1) {
            printf("i");
        } else if (c.img == -1) {
            printf("-i");
        } else {
            printf("%.6fi", c.img);
        }
    } else {
        if (c.img > 0) {
            if (c.img == 1) {
                printf("%.6f + i", c.real);
            } else {
                printf("%.6f + %.6fi", c.real, c.img);
            }
        } else {
            if (c.img == -1) {
                printf("%.6f - i", c.real);
            } else {
                printf("%.6f - %.6fi", c.real, fabs(c.img));
            }
        }
    }
}

void demo_operations() {
    printf("\n=== DEMOSTRACIÓN DE OPERACIONES ===\n");
    
    Complejo *a = creaComplejo(3,4);
    Complejo *b = creaComplejo(1,-2);
    
    //Complejo a=*a1;
    //Complejo b=*b1;
    printf("a = "); print_complex(*a); printf("\n");
    printf("b = "); print_complex(*b); printf("\n\n");
    
    printf("a + b = "); print_complex(*Complejo_add(a, b)); printf("\n");
    printf("a - b = "); print_complex(*Complejo_sub(a, b)); printf("\n");
    printf("a * b = "); print_complex(*Complejo_mul(a, b)); printf("\n");
    printf("a / b = "); print_complex(*Complejo_div(a, b)); printf("\n");
    printf("a ^ b = "); print_complex(*Complejo_powPolar(a, b)); printf("\n");
    printf("sqrt(a) = "); print_complex(*Complejo_sqrtPolar(a)); printf("\n");
    printf("exp(a) = "); print_complex(*Complejo_exp(a)); printf("\n");
    printf("sin(a) = "); print_complex(*Complejo_sin(a)); printf("\n");
    printf("cos(a) = "); print_complex(*Complejo_cos(a)); printf("\n");
    printf("\n");
}

