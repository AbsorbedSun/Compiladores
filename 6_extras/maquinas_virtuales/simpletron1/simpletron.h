typedef struct Symbol { /* entrada de la tabla de símbolos */
	char   *name;
	short   type;   /* VAR, BLTIN, UNDEF */
	union {
		double val;	       /* si es VAR */
		double  (*ptr)(double);      /* sí es BLTIN */
	} u;
	struct Symbol   *next;  /* para ligarse a otro */ 
} Symbol;

//Symbol *install(char *s,int t, double d), *lookup(char *s);

typedef union Datum {   /* tipo de la pila del intérprete */
double  val;
Symbol  *sym; 
} Datum; 

extern Datum pop();
typedef void (*Inst)(void);  /* instrucción de máquina */ 

#define STOP    (Inst) 0

extern	void eval();
extern	void add();
extern	void sub(); 
extern	void mul(); 
extern	void div_(); 
extern	void negate(); 
extern	void power();
extern	void assign(); 
extern	void bltin(); 
extern	void varpush(); 
extern	void constpush();
extern	void print();
extern	void pop1();
