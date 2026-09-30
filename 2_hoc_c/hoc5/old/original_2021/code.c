#include "hoc.h" 
#include "y.tab.h"
#define NSTACK  256
static  Datum  stack[NSTACK];  /* la pila */
static  Datum   *stackp;       /* siguiente lugar libre en la pila */
#define NPROG   2000
Inst    prog[NPROG];    /* la máquina */
Inst    *progp;         /* siguiente lugar libre para la generación de código */
Inst    *pc;	/* contador de programa durante la ejecución */

void initcode()      /* inicialización para la generación de código */ {
   stackp = stack;
   progp = prog;
}

void push(Datum d){	/*  meter d en la pila  */
if (stackp >= &stack[NSTACK])
execerror("stack overflow", (char *) 0);
*stackp++ = d;
}

void pop1( ){     /* sacar y retornar de la pila el elemento del tope */
   if (stackp <= stack)
      execerror("stack underflow", (char *) 0);
   --stackp;
}

Datum pop( )     /* sacar y retornar de la pila el elemento del tope */
{
if (stackp <= stack)
execerror("stack underflow", (char *) 0);
return  *--stackp;
}


void constpush( )	/* meter una constante a la pila  */
{
Datum d;
d.val  =  ((Symbol  *)*pc++)->u.val;
push(d);
}

void varpush()	/* meter una variable a la pila   */
{
Datum d;
d.sym  =  (Symbol   *)(*pc++);
push(d);
}

void whilecode() {
   Datum d;
   Inst  *savepc  = pc;	/*  cuerpo de la iteración  */
   execute(savepc+2);     /*   condición   */ 
   d  =  pop(); 
   while   (d.val)   {
      execute(*((Inst  **)(savepc)));     /*  cuerpo  */
      execute(savepc+2);
      d  = pop(); 
   } 
   pc  =  *((Inst  **)(savepc+1));     /*   siguiente proposición   */
}
void ifcode(){
   Datum d;
   Inst  *savepc  = pc;	/* parte then */
   execute(savepc+3);	/*  condición   */
   d  =  pop(); 
   if (d.val)
      execute(*((Inst   **)(savepc))); 
   else  if   (*((Inst  **)(savepc+1)))   /*  ¿parte else?   */
      execute(*(( Inst  **) (savepc+1)));
   pc  =  *((Inst  **)(savepc+2));	/*  siguiente proposición   */ 
}

void eval( )	/*  evaluar una variable en la pila   */
{
Datum  d;
d   =  pop();
if   (d.sym->type   ==   UNDEF)
execerror("undefined variable",   
d.sym->name); 
d.val   =  d.sym->u.val; push(d);
}

void add( )	/*   sumar los dos elementos superiores de la pila   */
{
Datum d1,   d2; 
d2  =  pop(); 
d1   =  pop(); 
d1.val  +=  d2.val; push(d1); 
}

void sub(){
Datum d1,  d2; 
d2  = pop(); 
d1  = pop(); 
d1.val  -= d2.val; 
push(d1);
}
void mul(){
Datum d1, d2;
d2 = pop(); 
d1 = pop(); 
d1.val *= d2.val; 
push(d1);
}
void div( ){
Datum d1, d2;
d2 = pop();
if (d2.val == 0.0)
execerror("division by zero", (char *)0);
d1 = pop(); 
d1.val /= d2.val; 
push(d1);
}
void negate(){
Datum d; 
d = pop(); 
d.val  =  -d.val; 
push(d);
}
void power(){
Datum d1, d2;
extern double Pow();
d2 = pop();
d1 = pop();
d1.val = Pow(d1.val, d2.val);
push(d1);
}
void assign( )        /* asignar el valor superior al siguientevalor */ 
{
Datum d1, d2;
d1 = pop();
d2 = pop();
if (d1.sym->type != VAR && d1.sym->type != UNDEF) 
execerror("assignment to non-variable", d1.sym->name);
d1.sym->u.val = d2.val;
d1.sym->type = VAR;
push(d2); 
} 
void prexpr() {      
   Datum d;
//puts("prexpr");
d = pop();
printf("%.8g ", d.val);
}
void print( ){  /* sacar el valor superior de la pila e imprimirlo */ 
 Datum d;
 d = pop();
 printf("\t%.8g\n",   d.val);
}
void bltin( )/*  evaluar un predefinido en el tope de la pila  */
{
Datum d;
d  =  pop();
d.val  =   (*(double   (*)(double))(*pc++))(d.val);
push(d);
}
Inst *code(Inst f){ /*   instalar una instrucción u operando   */
   //puts("code1");
   Inst *oprogp = progp;
   if (progp > &prog [ NPROG -1 ])
     execerror("program too big", (char *) 0);
   *progp++ = f;
   return oprogp;
}
void execute(Inst *p){	/*   ejecución con la máquina   */
for  (pc  =  p;   *pc != STOP; ) 
	(*pc++)();
}

void gt() {
Datum d1,  d2;
d2 = pop();
d1 = pop();
d1.val  =   (double)(d1.val  > d2.val);
push(d1);
}

void lt(){
Datum d1,  d2;
d2 = pop();
d1 = pop();
d1.val  =   (double)(d1.val  < d2.val);
push(d1);
}

void ge( ) {
Datum d1,  d2;
d2  = pop();
d1  = pop();
d1.val   =   (double)(d1.val  >= d2.val);
push(d1) ;
}

void le() {
Datum d1,  d2;
d2   =  pop();
d1   =  pop();
d1.val  =   (double)(d1.val   <=  d2.val);
push(d1);
}

void eq( ) {
Datum d1,  d2;
d2  = pop();
d1  = pop();
d1.val  =   (double) (d1.val  ==  d2.val);
push(d1);
}

void ne(){
Datum d1, d2;
d2 = pop();
d1 = pop();
d1.val = (double)(d1.val != d2.val);
push(d1);
}

void and(){
Datum d1,   d2;
d2   = pop();
d1   = pop();
d1.val = (double)(d1.val   !=   0.0 && d2.val   !=  0.0);
push(d1);
}

void or()
{
Datum d1, d2;
d2 = pop();
d1 = pop();
d1.val = (double)(d1.val != 0.0 || d2.val != 0.0);
push(d1);
}

void not( )
{
Datum d;
d = pop();
d.val = (double)(d.val == 0.0);
push(d);
}

