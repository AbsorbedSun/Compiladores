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

Termino *creaTermino(double coefi, int expo) {
	Termino *nvo;
	nvo = (Termino *)malloc(sizeof(Termino));
	nvo->coefi = coefi;
	nvo->expo = expo;
	return nvo;
}
void imprimeTermino(void *dato, int fin) {
	Termino *t = (Termino *)dato;
	if (fin == 0) printf("%+.4g x^%d ", t->coefi, t->expo);
	else if (fin == 1) printf("%+.4g x^%d\n", t->coefi, t->expo);
	else printf("%.4g x^%d ", t->coefi, t->expo);
}
Polinomio *inicializarPolinomio() {
    Polinomio *p=malloc(sizeof(Polinomio));
    p->grado = 0;
    p->coeficientes = malloc(sizeof(double)*MAX_GRADO);  
    for(int i = 0; i <= MAX_GRADO; i++) {
        p->coeficientes[i] = 0.0;
    }
    return p;
}

Polinomio *creaPolinomio(int n, double eltos[]) {
   Polinomio *p=(Polinomio *)malloc(sizeof(Polinomio));
   p->coeficientes = malloc(sizeof(double)*n);  
   p->grado = n;
   for (int i = 0; i < n+1; i++)
      p->coeficientes[i] = eltos[i];
   return p;
}

Polinomio *multiply(Polinomio *p1, Polinomio *p2) {
    int n = p1->grado+p2->grado;
    double *coef = malloc(sizeof(double)*(n+1));  
    for(int i = 0; i <= p1->grado; i++) {
        for(int j = 0; j <= p2->grado; j++) {
       coef[i+j] += p1->coeficientes[i]*p2->coeficientes[j];
      }
    }
    return creaPolinomio(n, coef);
  }

Polinomio *multiply___(Polinomio *p1, Polinomio *p2) {
    int n = p1->grado+p2->grado;
    double *coef = malloc(sizeof(double)*(n+1));  
    for(int i = 0; i<=n; i++) {
      coef[i] = 0;
      for(int k = 0; k<=i; k++) {
        coef[i] += p2->coeficientes[k]*p1->coeficientes[i-k];
      }
    }
    coef[n] = 0;
    for(int k = 0; k<=n; k++) {
      coef[n] += p2->coeficientes[k]*p1->coeficientes[n-k];
  printf("ene00 <%.4g %.4g p=%.4g %d %d>\n", 
      p1->coeficientes[k], p2->coeficientes[n-k],
      p2->coeficientes[k]*p1->coeficientes[n-k], k, n-k);
      printf("ene11 %.4g k=%d n-k=%d\n", coef[n], k, n-k);
    }
    puts("multiplica");
    for (int j = 0; j < n+1; ++j) 
               printf("%.4g j=%d\n", coef[j], j);
                    printf("\n");
    return creaPolinomio(n+1, coef);
  }

Polinomio *evaluarPolinomio(Polinomio *p, double x) {
    double *digi=
       (double*)malloc(sizeof(double)*(p->grado+1));  
    double *digio=
       (double*)malloc(sizeof(double)*(p->grado+1)); 
    double resultado = 0.0;
    int carry=0, resi=0;
    carry=0;
    for(int i = 0; i <= p->grado; i++) {
        resi= ((int)p->coeficientes[i]+ carry)% (int)x;
        carry=((int)p->coeficientes[i]+ carry) / (int)x;
        digi[p->grado+1-i]=resi;
        digio[i]=resi;
        resultado = resultado * x + p->coeficientes[i];
    }
    digi[0]=carry;
    digio[p->grado+1]=carry;
    printf("grado = <%04d> ",  p->grado+1);  
    for (int i =0; i <= p->grado+1; i++) {
        printf("%04d ", (int)digi[i]);  
    }
    //imprimirPolinomio_(*creaPolinomio(p->grado+1, digio));
    return creaPolinomio(p->grado+1, digio);
}

Polinomio *operBig(Polinomio *p1, Polinomio *p2,
   Polinomio *(*op)(Polinomio *p1, Polinomio *p2) ) {
   return evaluarPolinomio(op(p1,p2), 10000);
}

Polinomio *add(Polinomio *p1, Polinomio *p2) {
    int n = MAX(p1->grado, p2->grado)+1;
    double *coef = malloc(sizeof(double)*(n));  
    for(int i = 0; i<n; i++) {
      coef[i] = p1->coeficientes[i]+p2->coeficientes[i];
    }
    return creaPolinomio(n, coef);
}

Polinomio *subtract(Polinomio *p1, Polinomio *p2) {
    int n = MAX(p1->grado, p2->grado)+1;
    double *coef = malloc(sizeof(double)*(n));  
    for(int i = 0; i<n; i++) {
      coef[i] = p1->coeficientes[i]-p2->coeficientes[i];
    }
    return creaPolinomio(n, coef);
}

Polinomio *sumaB(Polinomio *p1, Polinomio *p2) {
   return operBig(p1, p2, add);
}

Polinomio *restaB(Polinomio *p1, Polinomio *p2) {
   return operBig(p1, p2, subtract);
}

Polinomio *multB(Polinomio *p1, Polinomio *p2) {
   return operBig(p1, p2, multiply);
}

Polinomio *divide(Polinomio *p1, Polinomio *p2) {
    return divideWithRemainder(p1, p2)[0];
}
Polinomio *modulo(Polinomio *p1, Polinomio *p2) {
    return divideWithRemainder(p1, p2)[1];
}
  
Polinomio **divideWithRemainder(Polinomio *p1, Polinomio *p2) {
    Polinomio **answer =
     (Polinomio**)malloc(2*sizeof(Polinomio *));
    int m = p1->grado;
    int n = p2->grado;
    if(m < n) {
      double *q = (double*)malloc(sizeof(double));
      *q=0;
      answer[0] = creaPolinomio(0, q);
      answer[1] = p2;
      return answer;
    }
    double *quotient = (double*)malloc(
                       sizeof(double)*(m-n+1));
    double *coef = (double*)malloc(
                       sizeof(double)*(m+1));
    for(int k = 0; k <= m; k++) {
       coef[k] = p1->coeficientes[k];
    }
    double norm = 1/p2->coeficientes[n];
    for(int k = m-n; k>=0; k--) {
      quotient[k] = coef[n+k]*norm;
      for(int j = n+k-1; j>=k; j--) {
        coef[j] -= quotient[k]*p2->coeficientes[j-k];
      }
    }
    double *remainder = (double*)malloc(sizeof(double)*(n));
    for(int k = 0; k<n; k++) {
      remainder[k] = coef[k];
    }
    answer[0] = creaPolinomio(m-n+1, quotient);
    answer[1] = creaPolinomio(n, remainder);
    return answer;
}
  
Polinomio *derivative(Polinomio *p) {
    int n = p->grado;
    if(n==0) {
      double *coef = (double*)malloc(sizeof(double));
      *coef=0;
      return creaPolinomio(n, coef);
    }
    double *coef = (double*)malloc(sizeof(double)*(n));
    for(int i = 1; i<=n; i++) {
      coef[i-1] = p->coeficientes[i]*i;
    }
    return creaPolinomio(n, coef);
}

double evaluate(Polinomio *p, double x) {
    int n = p->grado;
    double answer = p->coeficientes[--n];
    while(n>0) {
      answer = answer*x+p->coeficientes[--n];
    }
    return answer;
}

// Imprime un polinomio de forma legible
void imprimirPolinomio(Polinomio p) {
    int primerTermino = 1;
    
    printf("II grado = <%04d> ",  p.grado); 
    for(int i = p.grado; i >= 0; i--) {
        if(p.coeficientes[i] != 0) {
            if(!primerTermino) {
                if(p.coeficientes[i] > 0)
                    printf(" + ");
                else
                    printf(" - ");
            }
            
            if(primerTermino && p.coeficientes[i] < 0) {
                printf("-");
            }
            
            double coeficiente = fabs(p.coeficientes[i]);
            
            if(i == 0 || coeficiente != 1.0) {
                // Mostrar coeficiente sin decimales si es entero
                if(coeficiente == (int)coeficiente)
                    printf("%d", (int)coeficiente);
                else
                    printf("%.2f", coeficiente);
            }
            
            if(i > 0) {
                printf("x");
                if(i > 1)
                    printf("^%d", i);
            }
            
            primerTermino = 0;
        }
    }
    if(primerTermino) {
        printf("0");
    }
    
    printf("\n");
}

// Imprime un polinomio de forma legible
void imprimirPolinomio_(Polinomio p) {
    int primerTermino = 1;
 
    for(int i = p.grado; i >= 0; i--) {
            double coeficiente = fabs(p.coeficientes[i]);
            
            if(i == 0 || coeficiente != 1.0) {
                // Mostrar coeficiente sin decimales si es entero
                if(coeficiente == (int)coeficiente)
                    printf(" %04d", (int)coeficiente);
                else
                    printf(" %.2f", coeficiente);
            }
            
            primerTermino = 0;
    }
   
    if(primerTermino) {
        printf("0");
    }
    
    printf("\n");
}



// Lee un polinomio desde la entrada estándar
void leerPolinomio(Polinomio *p) {
    //inicializarPolinomio(p);
    
    printf("Ingrese el grado del polinomio (max %d): ", MAX_GRADO);
    scanf("%d", &p->grado);
    
    if(p->grado < 0 || p->grado > MAX_GRADO) {
        printf("Grado no valido. Se usara grado 0.\n");
        p->grado = 0;
    }
    
    printf("Ingrese los coeficientes desde el termino independiente hasta x^%d:\n", p->grado);
    for(int i = 0; i <= p->grado; i++) {
        printf("Coeficiente de x^%d: ", i);
        scanf("%lf", &p->coeficientes[i]);
    }
    
    // Ajustar el grado real (eliminar coeficientes cero al final)
    while(p->grado > 0 && p->coeficientes[p->grado] == 0) {
        p->grado--;
    }
}
/*
void main(){
   Polinomio *p1, *p2, *resultado, *resto;
   int opcion, potencia;
   double val;
   
   p1=inicializarPolinomio();
   p2=inicializarPolinomio();
   printf("\nIngrese el primer polinomio:\n");
   leerPolinomio(p1);
   printf("\nIngrese el segundo polinomio:\n");
   leerPolinomio(p2);  
   resultado=inicializarPolinomio();
do {
        printf("\n=== MENU DE OPERACIONES ===\n");
        printf("1. Imprimir polinomios\n");
        //printf("2. Verificar igualdad\n");
        printf("2. Sumar polinomios\n");
        printf("3. Restar polinomios\n");
        printf("4. Multiplicar polinomios\n");
        printf("5. Dividir polinomios\n");
        //printf("7. Elevar polinomio a potencia\n");
        //printf("8. Calcular MCD de polinomios\n");
        printf("6. evaluar polinomio\n");
        printf("9. Ingresar nuevos polinomios\n");  
        printf("0. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        
        switch(opcion) {
            case 1:
                printf("\nPolinomio 1: ");
                imprimirPolinomio(*p1);
                printf("Polinomio 2: ");
                imprimirPolinomio(*p2);
                break;              
            case 2:
                resultado = add(p1, p2);
                printf("\nSuma: ");
                imprimirPolinomio(*p1);
                printf(" + ");
                imprimirPolinomio(*p2);
                printf(" = ");
                imprimirPolinomio(*resultado);
                break;          
            case 3:
                resultado = subtract(p1, p2);
                printf("\nResta: ");
                imprimirPolinomio(*p1);
                printf(" - ");
                imprimirPolinomio(*p2);
                printf(" = ");
                imprimirPolinomio(*resultado);
                break;            
            case 4:
                resultado = multiply(p1, p2);
                printf("\nMultiplicacion: ");
                imprimirPolinomio(*p1);
                printf(" * ");
                imprimirPolinomio(*p2);
                printf(" = ");
                imprimirPolinomio(*resultado);
                break;        
            case 5:
                if(p2->grado == 0 && p2->coeficientes[0] == 0) {
                    printf("\nError: No se puede dividir por cero.\n");
                } else {
                    resultado = divide(p1, p2);
                    printf("\nDivision: ");
                    imprimirPolinomio(*p1);
                    printf(" / ");
                    imprimirPolinomio(*p2);
                    printf(" = ");
                    imprimirPolinomio(*resultado);
                    //printf("Resto: ");
                    //imprimirPolinomio(*resto);
                }
                break;
            case 6:
                p1=inicializarPolinomio();
                p2=inicializarPolinomio();
                printf("\nIngrese el primer polinomio:\n");
                leerPolinomio(p1);
                printf("\nIngrese el segundo polinomio:\n");
                leerPolinomio(p2);       
                //resultado = multiplicarPolinomios(p1, p2);
                //imprimirPolinomio(*resultado);
                imprimirPolinomio_(*multB(p1,p2));
                //val=evaluarPolinomio(&resultado, 10000);
                //printf("eval= <%f> ", val);
                break;    
            case 0:
                printf("\nSaliendo del programa...\n");
                break;
            case 9:
                p1=inicializarPolinomio();
                p2=inicializarPolinomio();
                printf("\nIngrese el primer polinomio:\n");
                leerPolinomio(p1);
                printf("\nIngrese el segundo polinomio:\n");
                leerPolinomio(p2);
                break;  
            case 70:
                
                break;
                
            case 80:
                
                break;  
            default:
                printf("\nOpcion no valida. Intente nuevamente.\n");
        }
    } while(opcion != 0);
    
}*/



