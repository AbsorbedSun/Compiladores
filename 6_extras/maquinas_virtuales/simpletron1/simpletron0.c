#include <stdio.h>
#include <stdlib.h>
#include "simpletron.h"
#define NSTACK  256
static  Datum  stack[NSTACK];  /* la pila */
static  Datum   *stackp;       /* siguiente lugar libre en la pila */
int prog[100]={0};
int pc;
int operationCode;   
int operand;
int processing;

void displayMemory(int memory[],int size){

    int i;

    printf("\n\n****SML file Loaded in Memory****\n\n");
    for(i=0;i<size;i++){
        printf(" %+05d ",memory[i]);

        if((i+1)%10==0){

            printf("\n");

        }

    }
    printf("\n");
}
void initcode( ) {
   stackp = stack;
}

void push(Datum d){	/*  meter d en la pila  */
   //if (stackp >= &stack[NSTACK])
   //   execerror("stack overflow", (char *) 0);
   *stackp++ = d;
}
void pop1( ){     /* sacar y retornar de la pila el elemento del tope */
   //if (stackp <= stack)
   //   execerror("stack underflow", (char *) 0);
   --stackp;
}
Datum pop( ){     /* sacar y retornar de la pila el elemento del tope */
   //if (stackp <= stack)
   //   execerror("stack underflow", (char *) 0);
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
   d2  =  pop(); 
   d1  =  pop(); 
   printf("add = %g %g ", d1.val, d2.val);
   d1.val  +=  d2.val; 
   push(d1); 
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
void div_( ){
   Datum d1, d2;
   d2 = pop();
   //if (d2.val == 0.0)
   //   execerror("division by zero", (char *)0);
   d1 = pop(); 
   d1.val /= d2.val; 
   push(d1);
}
void load(){
   Datum d1;
   d1.val = prog[operand];
   push(d1);
      //pc = pc + 1;
}
void store(){
   Datum d1;
   d1= pop();
   //memory.add(operand, new Integer((int)d1));
   prog[operand] = d1.val;
      //pc = pc + 1;
}
void read(){
   Datum d1;
   int val;
   printf("Opcode is 10, So Enter a number : ");
   scanf("%d", &val);
   prog[operand] = val;
   //d1.val = prog[operand];
   d1.val = val;
   push(d1);
}
void write(){
   puts("output: ");
   Datum d;
   d=pop();
   //puts("["+d.val+"]");
   printf("[%g]", d.val);
   push(d);
}
void halt(){ processing = 0; }
void branch(){ pc = operand; }
void branchneg(){
   Datum d1;
   d1 = pop();
   if(d1.val < 0){
      pc = operand;
   } //else { pc = pc + 1; }
}
void branchzero(){
   Datum d1;
   d1 = pop();
   if(d1.val == 0){
      pc = operand;
   } //else { pc = pc + 1; }
}
void execute(){	/*   ejecución con la máquina   */
   //Datum d1;
   //int val;
   puts("Simpletron execution begins!");
   processing = 1;
   pc = 0;
   initcode();
   while(processing){
      //int instructionRegister = ((Integer)memory.get(pc)).intValue();
      printf("111pc=%d ", pc);
      int instructionRegister = prog[pc];
      operationCode = instructionRegister / 100;
      operand = instructionRegister % 100;
      //printf("operationCode= %d %d", operationCode, operand);
      switch(operationCode){
            case 10 : read();
            break;
            case 11 : write(); 
                //printf("res=%d ", prog[operand]);
            break;

            case 20 : load();

            break;

            case 21 : store();

            break;

            case 30 : add( );

            break;

            case 31 : sub();

            break;

            case 32 : div_( );

            break;

            case 33 : mul();

            break;

            case 40 : branch();
                      continue;
            break;

            case 41 : branchneg();
                      continue;
            break;

            case 42 : branchneg();
                      continue;
            break;
            case 43 : halt();
                      continue;
            break;

        }
        pc=pc+1; 
        //printf("222pc=%d ", pc);     
   }
        puts("Simpletron execution terminated!");
}

void main(int argCount, char *argArr[]){
   FILE *file;
   int i = 0;
    if(argCount==2){

        if((file=fopen(argArr[1],"r"))==NULL){

            printf("\nProvided SML File is Not Opening\n");

        }else{
            while(!feof(file)){
                fscanf(file,"%d",&prog[i]);
                i++;
            }
            fclose(file);
        }
        i=0;
    } else{
        printf("\nNo SML file provided to RUN\n");
        exit(0);
    }
   displayMemory(prog,100);
   execute();
}




