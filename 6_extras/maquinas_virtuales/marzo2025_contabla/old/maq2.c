
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
// stack will have fixed size
#define STACK_SIZE 512

#define PUSH(vm, v) vm->stack[++vm->sp] = v // push value on top of the stack
#define POP(vm)     vm->stack[vm->sp--]     // pop value from top of the stack
#define NCODE(vm)   vm->code[vm->pc++]      // get next bytecode

enum {

    ADD_I32 = 1,    // int add
    SUB_I32 = 2,    // int sub
    MUL_I32 = 3,    // int mul
    LT_I32 = 4,     // int less than
    EQ_I32 = 5,     // int equal
    JMP = 6,        // branch
    JMPT = 7,       // branch if true
    JMPF = 8,       // branch if false
    CONST_I32 = 9,  // push constant integer
    LOAD = 10,      // load from local
    GLOAD = 11,     // load from global
    STORE = 12,     // store in local
    GSTORE = 13,    // store in global memory
    PRINT = 14,     // print value on top of the stack
    POP = 15,       // throw away top of the stack
    HALT = 16,      // stop program
    CALL = 17,      // call procedure
    RET = 18,        // return from procedure
    NE_I32 = 19,
    GT_I32 = 20,
    LE_I32 = 21,
    GE_I32 = 22,
    OR = 23,
    AND = 24,
    NOT = 25
};

static struct {         
/* Constantes */ 
char *name; 
int cval;
} consts[] = {
    "ADD_I32", 1,    // int add
    "SUB_I32", 2,    // int sub
    "MUL_I32", 3,    // int mul
    "LT_I32", 4,     // int less than
    "EQ_I32", 5,     // int equal
    "JMP", 6,        // branch
    "JMPT", 7,       // branch if true
    "JMPF", 8,       // branch if false
    "CONST_I32", 9,  // push constant integer
    "LOAD", 10,      // load from local
    "GLOAD", 11,     // load from global
    "STORE", 12,     // store in local
    "GSTORE", 13,    // store in global memory
    "PRINT", 14,     // print value on top of the stack
    "POP", 15,       // throw away top of the stack
    "HALT", 16,      // stop program
    "CALL", 17,      // call procedure
    "RET", 18,        // return from procedure
    "NE_I32", 19,
    "GT_I32", 20,
    "LE_I32", 21,
    "GE_I32", 22,
    "OR", 23,
    "AND", 24,
    "NOT", 25
};

typedef struct {
    int* locals;    // local scoped data
    int* code;      // array od byte codes to be executed
    int* stack;     // virtual stack
    int pc;         // program counter (aka. IP - instruction pointer)
    int sp;         // stack pointer
    int fp;         // frame pointer (for local scope)
} VM;

/*
char *busca(int n){
int i;

   for (i = 0; consts[i].name; i++)
      if (i== n) 
         return i;
   return -1;
}*/

VM* newVM(int* code,    // pointer to table containing a bytecode to be executed
    int pc,             // address of instruction to be invoked as first one - entrypoint/main func
    int datasize) {      // total locals size required to perform a program operations
    VM* vm = (VM*)malloc(sizeof(VM));
    vm->code = code;
    vm->pc = pc;
    vm->fp = 0;
    vm->sp = -1;
    vm->locals = (int*)malloc(sizeof(int) * datasize);
    vm->stack = (int*)malloc(sizeof(int) * STACK_SIZE);

    return vm;
}

    void delVM(VM* vm){
        free(vm->locals);
        free(vm->stack);
        free(vm);
    } 

    void run(VM* vm){
    //-----int cta=0;
    do{
        int opcode = NCODE(vm);        // fetch
        printf("(opcode=%d,b=%s )\n", opcode, consts[opcode-1].name );
        int v, addr, offset, a, b, argc, rval;
        //----if(cta==120) exit(0);
        //----cta++;
        //puts(consts[opcode].name);
        switch (opcode) {   // decode
        case HALT: return;  // stop the program
        case CONST_I32:
            v = NCODE(vm);   // get next value from code ...
            PUSH(vm, v);     // ... and move it on top of the stack
            break;
        case ADD_I32:
            b = POP(vm);        // get second value from top of the stack ...
            a = POP(vm);        // ... then get first value from top of the stack ...
            PUSH(vm, a + b);    // ... add those two values and put result on top of the stack
             break;
        case SUB_I32:
             b = POP(vm);        // get second value from top of the stack ...
            a = POP(vm);        // ... then get first value from top of the stack ...
            PUSH(vm, a - b);    // ... add those two values and put result on top of the stack
             break;
        case MUL_I32: 
            b = POP(vm);        // get second value from top of the stack ...
            a = POP(vm);        // ... then get first value from top of the stack ...
            PUSH(vm, a * b);    // ... add those two values and put result on top of the stack
             break;
        case EQ_I32: 
             b = POP(vm);        // get second value from top of the stack ...
             a = POP(vm);        // ... then get first value from top of the stack ...
             printf("(a=%d,b=%d, %d)\n",a,b,  (a==b) ? 1 : 0);
             PUSH(vm, (a==b) ? 1 : 0); // ... compare those two values, and put result on top of the stack
             break;
        case NE_I32: 
             b = POP(vm);        // get second value from top of the stack ...
             a = POP(vm);        // ... then get first value from top of the stack ...
             printf("NE (a=%d,b=%d, %d)\n",a,b,  (a!=b) ? 1 : 0);
             PUSH(vm, (a!=b) ? 1 : 0); // ... compare those two values, and put result on top of the stack
             break;
        case LT_I32:
             b = POP(vm);        // get second value from top of the stack ...
             a = POP(vm);        // ... then get first value from top of the stack ...    
             printf("LT (a=%d,b=%d, %d)\n",a,b,  (a<b) ? 1 : 0);
             PUSH(vm, (a<b) ? 1 : 0); // ... compare those two values, and put result on top of the stack
             break;
        case LE_I32:
             b = POP(vm);        // get second value from top of the stack ...
             a = POP(vm);        // ... then get first value from top of the stack ...
             PUSH(vm, (a<=b) ? 1 : 0); // ... compare those two values, and put result on top of the stack
             break;
        case GT_I32: 
             b = POP(vm);        // get second value from top of the stack ...
             a = POP(vm);        // ... then get first value from top of the stack ...
             printf("(a=%d,b=%d, %d)\n",a,b,  (a > b) ? 1 : 0);
             PUSH(vm, (a > b) ? 1 : 0); // ... compare those two values, and put result on top of the stack
             break;
        case GE_I32: 
             b = POP(vm);        // get second value from top of the stack ...
             a = POP(vm);        // ... then get first value from top of the stack ...
             printf("(a=%d,b=%d, %d)\n",a,b,  (a >= b) ? 1 : 0);
             PUSH(vm, (a >= b) ? 1 : 0); // ... compare those two values, and put result on top of the stack
             break;
//==========================================================      
        case JMPF: 
//==========================================================  
             addr = NCODE(vm);  // get address pointer from code ...
             printf("(addr=%d,b=%s)\n",addr, consts[opcode-1].name);
             if(! POP(vm)) {      // ... pop value from top of the stack, and if it's true ...
                vm->pc = addr; // ... jump with program counter to provided address
             }
             break;  
        case JMP:
             vm->pc = NCODE(vm);  // unconditionaly jump with program counter to provided address
             break;
        case JMPT:
             addr = NCODE(vm);  // get address pointer from code ...
             if(POP(vm)) {      // ... pop value from top of the stack, and if it's true ...
                vm->pc = addr; // ... jump with program counter to provided address
             }
             break;    
        case STORE:                 // store local value or function arg
             v = POP(vm);            // get value from top of the stack ...
             offset = NCODE(vm);     // ... get the relative pointer address from code ...
             vm->locals[vm->fp+offset] = v;  // ... and store value at address received relatively to frame pointer
             break;
        case LOAD:                  // load local value or function arg
             offset = NCODE(vm);     // get next value from code to identify local variables offset start on the stack
             printf("(offset=%d, arg=%d,   %s)\n",
             offset, vm->stack[vm->fp+offset], consts[opcode-1].name);
             PUSH(vm, vm->stack[vm->fp+offset]); // ... put on the top of the stack variable stored relatively to frame pointer
             break;
        
        case GLOAD:
             addr = POP(vm);             // get pointer address from code ...
             v = vm->locals[addr];         // ... load value from memory of the provided addres ...
             printf("(kixals=%d, v=%d,   %s)\n",
             vm->locals[addr],v, consts[opcode-1].name);
             
             PUSH(vm, vm->stack[vm->fp+offset]); 
             PUSH(vm, v);                // ... and put that value on top of the stack
             break;
        case GSTORE:
            v = POP(vm);                // get value from top of the stack ...
            addr = NCODE(vm);           // ... get pointer address from code ...
            vm->locals[addr] = v;         // ... and store value at address received
            printf("(locals=%d, v=%d,   %s)\n",
             vm->locals[addr], v, consts[opcode-1].name);
            break;
        
        case CALL:
        // we expect all args to be on the stack
             addr = NCODE(vm); // get next instruction as an address of procedure jump ...
             argc = NCODE(vm); // ... and next one as number of arguments to load ...
             PUSH(vm, argc);   // ... save num args ...
             PUSH(vm, vm->fp); // ... save function pointer ...
             PUSH(vm, vm->pc); // ... save instruction pointer ...
             vm->fp = vm->sp;  // ... set new frame pointer ...
             vm->pc = addr;    // ... move instruction pointer to target procedure address
             break;
        case RET:
             rval = POP(vm);     // pop return value from top of the stack
             vm->sp = vm->fp;    // ... return from procedure address ...
             vm->pc = POP(vm);   // ... restore instruction pointer ...
             vm->fp = POP(vm);   // ... restore framepointer ...
             argc = POP(vm);     // ... hom many args procedure has ...
             vm->sp -= argc;     // ... discard all of the args left ...
             PUSH(vm, rval);     // ... leave return value on top of the stack
             break;
        case AND: 
             b = POP(vm);        // get second value from top of the stack ...
             a = POP(vm);        // ... then get first value from top of the stack ...
             printf("(a=%d,b=%d, %d)\n",a,b,  (a!= 0 && b!= 0) ? 1 : 0);
             PUSH(vm, (a!= 0 && b!= 0) ? 1 : 0); // ... compare those two values, and put result on top of the stack
             break;
        case OR: 
             b = POP(vm);        // get second value from top of the stack ...
             a = POP(vm);        // ... then get first value from top of the stack ...
             printf("(a=%d,b=%d, %d)\n",a,b,  (a!= 0 || b!= 0) ? 1 : 0);
             PUSH(vm, (a!= 0 || b!= 0) ? 1 : 0); // ... compare those two values, and put result on top of the stack
             break;
        case POP:
            --vm->sp;      // throw away value at top of the stack
            break;
        case PRINT:
            v = POP(vm);        // pop value from top of the stack ...
            printf("PRINT %d\n", v);  // ... and print it
            break;
        default:
            break;
        }

    }while(1);
}
    
    const int fib = 0;  // address of the fibonacci procedure
    int program1[] = {
    // int fib(n) {
    //     if(n == 0) return 0;
    LOAD, -3,       // 0 - load last function argument N
    CONST_I32, 0,   // 2 - put 0
    EQ_I32,         // 4 - check equality: N == 0
    JMPF, 10,       // 5 - if they are NOT equal, goto 10
    CONST_I32, 0,   // 7 - otherwise put 0
    RET,            // 9 - and return it
    //     if(n < 3) return 1;
    LOAD, -3,       // 10 - load last function argument N
    CONST_I32, 3,   // 12 - put 3
    LT_I32,         // 14 - check if 3 is less than N
    JMPF, 20,       // 15 - if 3 is NOT less than N, goto 20
    CONST_I32, 1,   // 17 - otherwise put 1
    RET,            // 19 - and return it
    //     else return fib(n-1) + fib(n-2);
    LOAD, -3,       // 20 - load last function argument N
    CONST_I32, 1,   // 22 - put 1
    SUB_I32,        // 24 - calculate: N-1, result is on the stack
    CALL, fib, 1,   // 25 - call fib function with 1 arg. from the stack
    LOAD, -3,       // 28 - load N again
    CONST_I32, 2,   // 30 - put 2
    SUB_I32,        // 32 - calculate: N-2, result is on the stack
    CALL, fib, 1,   // 33 - call fib function with 1 arg. from the stack
    ADD_I32,        // 36 - since 2 fibs pushed their ret values on the stack, just add them
    RET,            // 37 - return from procedure
    // entrypoint - main function
    CONST_I32, 10,   // 38 - put 6 
    CALL, fib, 1,   // 40 - call function: fib(arg) where arg = 6;
    PRINT,          // 43 - print result
    HALT            // 44 - stop program
};


const int facto = 0;  // address of the fibonacci procedure
    int program2[] = {
    // int fib(n) {
    //     if(n == 0) return 0;
    LOAD, -3,       // 0 - load last function argument N
    CONST_I32, 0,   // 2 - put 0
    EQ_I32,         // 4 - check equality: N == 0
    JMPF, 10,       // 5 - if they are NOT equal, goto 10
    CONST_I32, 1,   // 7 - otherwise put 1
    RET,            // 9 - and return it  
    LOAD, -3,       // 10 20 - load last function argument N
    CONST_I32, 1,   // 12 22 - put 1
    SUB_I32,        // 14 24 - calculate: N-1, result is on the stack
    CALL, facto, 1,   // 15 25 - call fib function with 1 arg. from the stack
    LOAD, -3,       // 18 28 - load N again
    MUL_I32,        // 20 30 - since 2 fibs pushed their ret values on the stack, just add them
    RET,            // 21 37 - return from procedure
    // entrypoint - main function
    CONST_I32, 7,   // 22  38 - put 7 
    CALL, facto, 1,   // 24 - call function: fib(arg) where arg = 6;
    PRINT,          // 25  43 - print result
    HALT            // 26  44 - stop program
};

const int inc = 0;  // address of the fibonacci procedure
    int program3[] = {
    // int fib(n) {
    //     if(n == 0) return 0;
    LOAD, -3,       // 0 - load last function argument N
    CONST_I32, 1,   // 2
    ADD_I32,        // 4
    RET,            // 5
    CONST_I32, 7,   // 6  38 - put 7 
    CALL, inc, 1, // 8 - call function: fib(arg) where arg = 6;
    PRINT,          // 9  43 - print result
    HALT            // 10  44 - stop program
};

const int suma = 0;  // address of the fibonacci procedure
    int program4[] = {
    // int fib(n) {
    //     if(n == 0) return 0;
    LOAD, -3,       // 0 - load last function argument N
    LOAD, -4,   // 2
    ADD_I32,        // 4
    RET,            // 5
    CONST_I32, 44,   // 6  38 - put 7 
    CONST_I32, 33,   // 8  38 - put 7 
    CALL, suma, 2, // 10 - call function: fib(arg) where arg = 6;
    PRINT,          // 11  43 - print result
    HALT            // 12  44 - stop program
};   

const int ciclo = 0;  // address of the fibonacci procedure
int program5[] = {
    CONST_I32, 0,   // 0 - put 0
    GSTORE, 0,       // 2   
    CONST_I32, 0,   // 4 - put 0
    GLOAD,         // 6  
    LOAD, -3,       //7 - load last function argument N       
    LT_I32,         // 9 - check equality: N == 0
    JMPF, 26,       // 10 - if they are NOT equal, goto 10
    CONST_I32, 0,   // 12
    GLOAD,        // 14
    PRINT,          // 15
    CONST_I32, 0,   // 16
    GLOAD,         // 18
    CONST_I32, 1,   // 19 - otherwise put 1
    ADD_I32,        // 21
    GSTORE, 0,       //22
    JMP, 4,      // 24
    
    RET,            // 26 - and return it  
    
    CONST_I32, 7,   // 27  38 - put 7 
    CALL, ciclo, 1,   // 29 - call function: fib(arg) where arg = 6;
    PRINT,          // 31 - print result
    HALT            // 32  44 - stop program*/
};
    
void main(){
   /*VM* vm = newVM(program1,   // program to execute
                   38,    // start address of main function
                   0);    // locals to be reserved, fib doesn't require them
    */
    /*VM* vm = newVM(program2,   // program to execute
                   22,    // start address of main function
                   0);    // locals to be reserved, fib doesn't require them
    */
    /*VM* vm = newVM(program4,   // program to execute
                   6,    // start address of main function
                   0);    // locals to be reserved, fib doesn't require them
   */
    VM* vm = newVM(program5,   // program to execute
                   27,    // start address of main function
                   10);    // locals to be reserved, fib doesn't require them
    run(vm);
}


    

