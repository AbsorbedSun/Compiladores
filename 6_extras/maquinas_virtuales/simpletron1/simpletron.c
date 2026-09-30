#include <stdio.h>
#include <stdlib.h>
#include "simpletron.h"

#define code2(c1, c2)   code(c1); code(c2);

#define READ         10
#define WRITE        11
#define LOAD         20
#define STORE        21
#define ADD          30
#define SUBTRACT     31
#define DIVIDE       32
#define MULTIPLY     33
#define INC          34
#define DEC          35
#define BRANCH       40
#define BRANCHNEG    41
#define BRANCHZERO   42
#define HALT         43
   
#define NSTACK 1024
Datum  stack[NSTACK];  /* la pila */
Datum   *stackp;       /* siguiente lugar libre en la pila */

#define NPROG   20000
Inst    prog[NPROG];    /* la RAM de la máquina VP*/
Inst    *progp;         /* siguiente lugar libre para la generación de código */
Inst    *pc;	/* contador de programa durante la ejecución */

int memo[NPROG];
int operationCode;   
int operand;

void displayMemory(int memory[], int size){
    int i;
    printf("\n\n****SML file Loaded in Memory****\n\n");
    for(i=0;i<size;i++){
        printf(" %+05d ",memory[i]);
        if((i+1) % 10 == 0){
            printf("\n");
        }
    }
    printf("\n");
}
void initcode( ) {
   stackp = stack;
   progp = prog;
}
void push(Datum d){	/*  meter d en la pila  */
   if (stackp >= &stack[NSTACK])
      puts("stack overflow");
   *stackp++ = d;
}
void pop1( ){     /* sacar y retornar de la pila el elemento del tope */
   if (stackp <= stack)
      puts("stack underflow");
   --stackp;
}
Datum pop( ){     /* sacar y retornar de la pila el elemento del tope */
   if (stackp <= stack)
      puts("stack underflow");
   return  *--stackp;
}
void inc(){
   Datum d1; 
   d1  =  pop(); 
   d1.val += 1; 
   push(d1);
}
void dec(){
   Datum d1; 
   d1  =  pop(); 
   d1.val -= 1; 
   push(d1);
}
void negate(){
   Datum d; 
   d = pop(); 
   d.val =  -d.val; 
   push(d);
}
/*void power(){
   Datum d1, d2;
   extern double Pow();
   d2 = pop();
   d1 = pop();
   d1.val = Pow(d1.val, d2.val);
   push(d1);
}*/
void add( ){
   Datum d; 
   long int val, offset;
   offset = (long)pc[0];
   d  =  pop(); 
   val = (long)prog[offset];
   //printf("add = %g %ld %ld", d.val, val, offset);
   d.val  +=  val;
   push(d); 
   pc=pc+1;
}
void sub(){
   Datum d; 
   long int val, offset;
   //puts("sub");
   offset = (long)pc[0];
   //printf("sub offset= (%ld)", offset);
   d  = pop(); 
   val = (long)prog[offset];
   //printf("sub = %g %ld %ld ", d.val, val, offset);
   d.val  -= val; 
   //printf("resta = %g ", d.val);
   push(d);
   pc=pc+1;
}
void mul(){
   Datum d;
   long int val, offset;
   offset = (long)pc[0];
   d = pop(); 
   val = (long)prog[offset];
   d.val *= val; 
   push(d);
   pc=pc+1;
}
void div_( ){
   Datum d;
   long int val, offset;
   offset = (long)pc[0];
   d = pop();
   //if (d2.val == 0.0)
   //   execerror("division by zero", (char *)0);
   val = (long)prog[offset];
   d.val /= val; 
   push(d);
   pc=pc+1;
}
void store(){
   Datum d;
   long int offset;
   offset = (long)pc[0];
   d = pop();
   //printf("store [%g] (%ld) \n", d.val, offset);
   prog[offset] = (Inst)((long)d.val);
   pc = pc + 1;
}
void load(){
   Datum d;
   long int offset;
   //puts("load");
   offset = (long)pc[0];   
   d.val = (long)prog[offset];
   //printf("load [%g] (%ld) \n", d.val, offset);
   push(d);
   pc = pc + 1;
}
void read(){
   long int val;
   long int offset;
   printf("Opcode is 10, So Enter a number : ");
   scanf("%ld", &val);
   offset = (long)pc[0];
   prog[offset] = (Inst)val;
   printf("read [%ld] (%ld)\n", val, offset);
   pc=pc+1;
}
void write(){
   long int offset, val;
   printf("output: ");
   offset = (long)pc[0];
   val = (long)prog[offset];
   printf("[%ld]\n", val);
   pc = pc + 1;
}
void halt(){  }
void branch(){ 
   long int offset;
   //puts("branch");
   offset = (long)pc[0];
   //printf("branch <%ld>", offset);
   pc = prog + offset;  
}
void branchneg(){
   Datum d;
   long int offset;
   //puts("branchegn");
   offset = (long)pc[0];
   //printf("<%ld>", offset); 
   d = pop();  
   printf("branchneg [%g] (%ld)\n", d.val, offset);
   if(d.val < 0){
      //puts("IF branchneg");
      pc = prog + offset;
   } else { pc = pc + 1; }
}
void branchzero(){
   Datum d;
   long int offset;
   d = pop();
   offset = (long)pc[0];
   if(d.val == 0){
      pc = prog + offset;
   } else { pc = pc + 1; }
}
Inst *code(Inst f){ /*   instalar una instrucción u operando   */
   Inst *oprogp = progp;
   if (progp > &prog [ NPROG -1 ])
      puts("program too big");
   *progp++ = f;
   return oprogp;
}
void execute(Inst *p){	/*   ejecución con la máquina   */
   for  (pc  =  p;   *pc != STOP; ) 
	(*pc++)();
}
void allcode(int cta){	/*   carga en la máquina   */
   int pc1;
   puts("Simpletron load begins!");
   pc1 = 0;
   initcode();
   while(pc1 < cta){
      //printf("111pc=%d ", pc1);
      int instructionRegister = memo[pc1];
      operationCode = instructionRegister / 100;
      operand = instructionRegister % 100;
      //printf("operationCode= %d %d", operationCode, operand);
      switch(operationCode){
            case READ: code2(read, (Inst)((long)operand));
            break;
            case WRITE: code2(write, (Inst)((long)operand));
            break;
            case LOAD: code2(load, (Inst)((long)operand));
            break;
            case STORE: code2(store, (Inst)((long)operand));
            break;
            case ADD: code2(add, (Inst)((long)operand));
            break;
            case SUBTRACT: code2(sub, (Inst)((long)operand)); 
            break;
            case DIVIDE: code2(div_, (Inst)((long)operand));
            break;
            case MULTIPLY: code2(mul, (Inst)((long)operand));
            break;
            case BRANCH: code2(branch, (Inst)((long)operand)); 
            break;
            case BRANCHNEG: code2(branchneg, (Inst)((long)operand));
            break;
            case BRANCHZERO: code2(branchzero, (Inst)((long)operand));
            break;
            case HALT : code(STOP);
            break;
        }
        pc1=pc1+1;     
   }
   puts("Simpletron load terminated!");
}
void cargaArchi(char *nombre, int *i){
   FILE *file;
   if((file=fopen(nombre,"r"))==NULL){
      printf("\nProvided SML File is Not Opening\n");
   } else{
      while(!feof(file)){
         fscanf(file,"%d", &memo[*i]);
         *i=*i+1;
      }
      fclose(file);
   }
}
void main(int argc, char *argv[]){ 
   int i = 0;
   if(argc==2){
      cargaArchi(argv[1], &i);
   } else{
      printf("\nNo SML file provided to RUN\n");
      exit(0);
   }
   displayMemory(memo, 100);
   allcode(i);
   execute(prog);
}




