typedef struct Symbol { /* entrada en la tabla de símbolos */
char    *name;
short   type;   /* VAR, BLTIN, UNDEF */
union {
double val;	/* si es VAR */
double  (*ptr)(double);       /* si es BLTIN */
} u;
struct Symbol   *next;  /* para ligarse a otro */ } Symbol;
Symbol  *install(), *lookup();

typedef union Datum {   /* tipo de la pila del intérprete */
double  val;
Symbol  *sym; } Datum; 

extern Datum pop();

typedef void (*Inst)(void);  /* instrucción de maquina */ 

#define STOP    (Inst) 0

extern	Inst prog[ ] , *progp, *code();

extern	void eval();
extern	void add();
extern	void sub(); 
extern	void mul(); 
extern	void div(); 
extern	void negate(); 
extern	void power();
extern	void assign(); 
extern	void bltin(); 
extern	void varpush(); 
extern	void constpush();
extern	void print();
extern	void pop1();

extern	void prexpr();
extern	void gt();
extern	void lt(); 
extern	void eq(); 
extern	void ge(); 
extern	void le(); 
extern	void ne(); 
extern	void and(); 
extern	void or();
extern	void not();
extern	void ifcode(); 
extern	void whilecode();
void execute(Inst *p);
