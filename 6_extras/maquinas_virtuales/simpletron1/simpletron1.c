#include <stdio.h>
#include <stdlib.h>
#include "simpletron.h"
#define NSTACK 1024
static  Datum  stack[NSTACK];  /* la pila */
static  Datum   *stackp;       /* siguiente lugar libre en la pila */

#define NPROG   20000
Inst    prog[NPROG];    /* la RAM de la máquina VP*/
Inst    *progp;         /* siguiente lugar libre para la generación de código */
Inst    *pc;	/* contador de programa durante la ejecución */

int memo[NPROG];
int operationCode;   
int operand;
int processing;

void displayMemory(int memory[],int size){
    int i;
    printf("\n\n****SML file Loaded in Memory****\n\n");
    for(i=0;i<size;i++){
        printf(" %+05d ",memory[i]);
        if((i+1)%10 == 0){
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
   Datum d1,   d2; 
   long int offset;
   offset = (long)pc[0];
   d2  =  pop(); 
   //d1  =  pop(); 
   d1.val = (long)prog[offset];
   //printf("add = %g %g %ld", d1.val, d2.val, offset);
   d1.val  +=  d2.val;
   push(d1); 
   pc=pc+1;
}
void sub(){
   Datum d1,  d2; 
   long int offset;
   puts("sub");
   offset = (long)pc[0];
   //printf("sub offset= (%ld)", offset);
   d2  = pop(); 
   //d1  = pop(); 
   d1.val = (long)prog[offset];
   //printf("sub = %g %g %ld ", d1.val, d2.val, offset);
   d2.val  -= d1.val; 
   //printf("resta = %g ", d2.val);
   push(d2);
   pc=pc+1;
}
void mul(){
   Datum d1, d2;
   long int offset;
   offset = (long)pc[0];
   d2 = pop(); 
   //d1 = pop(); 
   d1.val = (long)prog[offset];
   d1.val *= d2.val; 
   push(d1);
   pc=pc+1;
}
void div_( ){
   Datum d1, d2;
   long int offset;
   offset = (long)pc[0];
   d2 = pop();
   //if (d2.val == 0.0)
   //   execerror("division by zero", (char *)0);
   //d1 = pop(); 
   d1.val = (long)prog[offset];
   d1.val /= d2.val; 
   push(d1);
   pc=pc+1;
}
void store(){
   Datum d1;
   long int offset;
   d1= pop();
   //memory.add(operand, new Integer((int)d1));
   offset = (long)pc[0];
   //printf("store [%g] (%ld) \n", d1.val, offset);
   prog[offset] = (Inst)((long)d1.val);
   pc = pc + 1;
}
void load(){
   Datum d1;
   long int offset;
   puts("load");
   offset = (long)pc[0];   
   d1.val = (long)prog[offset];
   //printf("load [%g] (%ld) \n", d1.val, offset);
   push(d1);
   pc = pc + 1;
}
void read(){
   Datum d1;
   long int val;
   long int offset;
   printf("Opcode is 10, So Enter a number : ");
   scanf("%ld", &val);
   //d1.val = val;
   offset = (long)pc[0];
   prog[offset] = (Inst)val;
   //d1.val = prog[operand];
   d1.val = val;
   //push(d1);
   printf("read [%g] (%ld)\n", d1.val, offset);
   pc=pc+1;
}
void write(){
   Datum d;
   long int offset, val;
   printf("output: ");
   //d=pop();
   //puts("["+d.val+"]");
   offset = (long)pc[0];
   val = (long)prog[offset];
   printf("[%ld]\n", val);
   //push(d);
   pc = pc + 1;
}
void halt(){  }
void branch(){ 
   long int offset;
   puts("branch");
   offset = (long)pc[0];
   //printf("branch <%ld>", offset);
   pc = prog + offset;  
}
void branchneg(){
   Datum d1;
   long int offset;
   //puts("branchegn");
   offset = (long)pc[0];
   //printf("<%ld>", offset); 
   d1 = pop();  
   printf("branchneg [%g] (%ld)\n", d1.val, offset);
   if(d1.val < 0){
      //puts("IF branchneg");
      pc = prog + offset;
   } else { pc = pc + 1; }
}
void branchzero(){
   Datum d1;
   long int offset;
   d1 = pop();
   offset = (long)pc[0];
   if(d1.val == 0){
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
   processing = 1;
   pc1 = 0;
   initcode();
   while(pc1 < cta){
      //printf("111pc=%d ", pc1);
      int instructionRegister = memo[pc1];
      operationCode = instructionRegister / 100;
      operand = instructionRegister % 100;
      //printf("operationCode= %d %d", operationCode, operand);
      switch(operationCode){
            case 10 : code(read); code((Inst)((long)operand));
            break;
            case 11 : code(write); code((Inst)((long)operand));
            break;
            case 20 : code(load); code((Inst)((long)operand));
                      //printf("operationCode= (%d,%d)", operationCode, operand);
            break;
            case 21 : code(store); code((Inst)((long)operand));
            break;
            case 30 : code(add); code((Inst)((long)operand));
            break;
            case 31 : code(sub); code((Inst)((long)operand));
                   printf("operationCode= (%d,%d)", operationCode, operand);   
            break;
            case 32 : code(div_); code((Inst)((long)operand));
            break;
            case 33 : code(mul); code((Inst)((long)operand));
            break;
            case 40 : code(branch); code((Inst)((long)operand)); 
                      //printf("operationCode= %d %d", operationCode, operand);
            break;
            case 41 : code(branchneg); code((Inst)((long)operand));
            break;
            case 42 : code(branchzero); code((Inst)((long)operand));
            break;
            case 43 : code(STOP);
                      //processing = 0;
            break;
        }
        pc1=pc1+1; 
        //printf("222pc=%d ", pc);     
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
      //i=0;
   } else{
      printf("\nNo SML file provided to RUN\n");
      exit(0);
   }
   displayMemory(memo, 100);
   allcode(i);
   execute(prog);
}




