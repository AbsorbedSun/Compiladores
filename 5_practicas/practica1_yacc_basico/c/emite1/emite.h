
typedef struct Symbol {	/* symbol table entry */
	char	*name;
	short	type;	/* VAR, BLTIN, UNDEF */
	double val;		/* if VAR */
	struct Symbol	*next;	/* to link to another */
} Symbol;
void genDataSec();
void genPrint();
void genNumero();
void genSuma();
void emite(char *s);


