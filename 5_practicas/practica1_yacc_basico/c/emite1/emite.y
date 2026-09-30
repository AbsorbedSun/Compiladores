%{
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "emite.h"

#define YYSTYPE char *

#include "y.tab.h"
int yylex(void);
void yyerror(char *s);
void warning(char *s, char *t);
void execerror(char *s, char *t);

FILE *fo;
int lineno;
%}
%token NUMBER 

%right '='
%left  '+' '-'
%left  '*'
%%
inicio : list {
                emite("mov eax, 1\n");
                emite("xor ebx, ebx\n");
                emite("int 0x80\n");
                rutimpre(); 
		genDataSec();
         }
list:  {
                emite(";nasm -f elf compi.asm\n");
                emite(";ld -melf_i386 compi.o -o compi\n");
                emite(";./compi\n");
                emite("BITS 32\n");
                emite("global _start\n");
                emite("section .text\n");
                emite("_start:\n");
        }
	| list stmt 
	;
stmt : expr
        | 'p' expr { 
            genPrint();
        }
     ;
expr: NUMBER { 
                genNumero($1); 
                emite("push eax; number\n");
        }
	| expr '+' expr { 
            genSuma(); 
            emite("push eax; suma\n"); 
                        }
	| '(' expr ')' { $$ = $2; }
	;
%%
void genDataSec(){
   int i;
   char s[100];

   emite("\nsection .data\n");
   /*for(i=0;i< tmp; i++){
      sprintf(s,"temp%d dw 0\n", i+1);
      emite(s);
   }*/
}
void genPrint(){
   emite("mov ebx, 10\n");
   //emite("push eax\n");
   emite("call print\n");
   emite("pop eax\n");
}
void genNumero(char *numero){
   char s[100];
   strcpy(s,"mov eax, ");
   strcat(s, numero);
   strcat(s, "\n");
   emite(s);
   free(numero);
}
void genSuma(){           
   emite("pop eax\n");
   emite("pop ecx\n");
   emite("add eax, ecx\n");
}
int yylex() { 
   int c; 
   etiq1:
   while((c=getchar()) == ' ' || c == '\t') 
   	; 
   if(c == EOF) 
      return 0; 
   if(c == '.' || isdigit(c)) { 
      int val;
      ungetc(c, stdin); 
      scanf("%d",&val);
      yylval=(char *)malloc(500);
      sprintf(yylval,"%d",val);
      return NUMBER; 
   } 
   if(c == '\n'){
      lineno++; goto etiq1; 
   }
   return c; 
 } 
void execerror(char *s, char *t){ warning(s, t); }
void yyerror (char *s)  { warning(s, (char *) 0); }
void warning(char *s, char *t){
	fprintf(stderr, "%s", /*progname,*/ s);
	if (t && *t)
		fprintf(stderr, " %s", t);
	fprintf(stderr, " near line %d\n", lineno);
}
void emite(char *s){
     puts(s);
     fputs(s,fo);
}
void main() { 
   fo=fopen("compi.asm","w"); 
   yyparse(); 
   fclose(fo);
}
void rutimpre(){
   emite("print:\n");
   emite("mov ecx, esp\n");
   emite("sub esp, 36\n");   
   emite("mov edi, 1\n");
   emite("dec ecx\n");
   emite("mov [ecx], byte 10\n"); 
   emite("print_loop:\n");
   emite("xor edx, edx\n");
   emite("div ebx\n");
   emite("cmp dl, 9\n");    
   emite("jg use_letter\n");
   emite("add dl, '0'\n");
   emite("jmp after_use_letter\n");
   emite("use_letter:\n");
   emite("add dl, 'W'\n");  
   emite("after_use_letter:\n");
   emite("dec ecx\n");
   emite("inc edi\n");
   emite("mov [ecx],dl\n");
   emite("test eax, eax\n");
   emite("jnz print_loop\n");
   emite("mov eax, 4\n");    
   emite("mov ebx, 1\n");    
   emite("mov edx, edi\n");  
   emite("int 0x80\n");
   emite("add esp, 36\n");   
   emite("ret\n");
}



