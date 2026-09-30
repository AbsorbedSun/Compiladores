#include <sys/stat.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct nodoL {
   void *info;
   struct nodoL *sig;
};
typedef struct nodoL NodoL;

struct hashEle {
   char *clave;
   void *valor;
   int tipo;
};
typedef struct hashEle HashEle;

struct aches {
   char *clave;
   int valori;
};
typedef struct aches Aches;

enum { MULTIPLIER=1 };

struct  object  {
   int tipo;
};
typedef struct object Object;

struct term {
   Object o;
   char functor[256]; //cambiar por apun
   int arity; 
   struct term **args;
   //int varnum; //=1;
   bool bound; 
   int varid;
   bool deref; 
   struct term *ref;
   bool occurcheck; // = false;
   bool prettyprint; // = true;
   bool internalparse; // = false;
};

typedef struct term Term;

struct termList {
        Term *term;
        struct termList *next;
	struct termList **definer; //= null;  
	int numclauses;//=0; 	
};
typedef struct termList TermList;

struct engine {
   //Stack stack; 
   NodoL *stack; 
   //Hashtable db; 
   NodoL **db; //hashtabchar *more(Engine *e, bool b)
   TermList *goal;
   Term *call;
   //bool trace = false;
   bool trace;
   //long time;
   TermList *failgoal;
};
typedef struct engine Engine;

struct parseString {
   char *str;
   int posn, start, varnum;
   //Hashtable vardict;
   NodoL **vardict;
};
typedef struct parseString ParseString;

struct  choicePoint  {
   Object o;
   int clausenum;
   TermList *goal;
   //int tipo;
};
typedef struct choicePoint ChoicePoint;

struct OnceMark {
   Object o;
   //int tipo;
};
typedef struct OnceMark OnceMark;

#define HASH_SIZE 1000

enum { CHOICEPOINT, ONCEMARK, TERM };

NodoL *creaPila(NodoL **stack);
int estaVacia(NodoL *stack);
NodoL *push(NodoL **stack, void *dato);
void *top(NodoL *stack);
NodoL *pop(NodoL **stack);



void creaHashTab(NodoL ***htab,int n);
NodoL *get(NodoL **htab,char *clave);
void put(NodoL **htab,char *clave, void *valor);

Term *creaTerm();
Term * creaTerm1(int i);
Term *creaTerm2(char *s,int a);
Term *creaTerm3(ParseString *ps);
void bind(Term *t1, Term *t2) ;
/** Unbinds a term -- ie. resets it to a variable */
void unbind(Term *t) ;

void setarg(Term *t, int pos,Term *val);

/** Retrieves an argument of a term */
Term *getarg(Term *t, int pos);

char *getfunctor(Term *t) ;
int getarity(Term *t) ;
bool occurs1(Term *t,int var);
bool occurs(Term *t,int var);
bool unify(Term *t1, Term *t2, NodoL **stack); 

Term *getvar(Term *l[], int v) ;
Term *refresh(Term *t1, Term *l[]);
char *toStringTerm(Term *t);

TermList *creaTermList1(Term *t1, TermList *n) ;
TermList *creaTermList2(Term *t1, TermList *n,TermList *d);
TermList  *creaTermList3(ParseString *ps);
char *toStringTL(TermList *t);
void resolve(TermList *tl, NodoL **db);

void run(Engine *e);
char *run1(Engine *e, bool embed);
char *more0(Engine *e, bool b);

char *toStringPS(ParseString *ps);
ParseString *creaParseString(char *s);
char current(ParseString *ps);
bool empty(ParseString *ps);
void advance(ParseString *ps);
char *getname(ParseString *ps) ;
char *getnum(ParseString *ps) ;
Term *getvarPS(ParseString *ps) ;
void parseerror(char *s);
void skipspace();
void skipcomment();
void nextclause(ParseString *ps);
NodoL **consult(char *s, NodoL **db);
void resolvePS(NodoL **db);

ChoicePoint *creaChoicePoint(int cn,TermList *g);
char *toStringCP(ChoicePoint *cp);

void error(char * caller, char* mesg);
void fatalerror(char *caller, char* mesg);
void result(char * s);
void diagnostic(char * s);
void trace(char * s);
void prologprint(char * s);

char *buls[]={"false", "true"};

ChoicePoint *creaChoicePoint(int cn, TermList *g) {
   ChoicePoint *cp=(ChoicePoint*)malloc(sizeof(ChoicePoint));
   cp->o.tipo = CHOICEPOINT;
   cp->clausenum = cn; 
   cp->goal = g;
   return cp;
}

char * toStringCP(ChoicePoint *cp) {
   char *msj;
   msj=(char *)malloc(256);
   sprintf(msj, "<< %d : %p >>", cp->clausenum, cp->goal);
   return (msj);
}

void error(char *caller, char * mesg) {
        printf(
	"FATAL ERROR: in %s  : %s\n", caller, mesg);
}
// fatal error ...
void fatalerror(char *caller, char * mesg) {
   printf(
	"FATAL ERROR: in %s  : %s\n", caller, mesg);
   exit(1);
}
void result(char * s) {
   printf("%s\n", s);
   printf("W JO %s\n", s);
}
void diagnostic(char * s) {
   printf("*** %s\n", s);
}
void trace(char *s) {
   printf("%s\n", s);
}
void prologprint(char * s) {
   puts(s);
}

NodoL *creaNodoL(void *info, NodoL *sig ){
   NodoL *nvo;
   nvo=(NodoL *)malloc(sizeof(NodoL));
   if(!nvo){
	puts("no hay memoria para crear NodoL");
        return (NodoL *)NULL;
   }
   nvo->info=info;
   nvo->sig=sig;
   return nvo;
}

NodoL *remueve(NodoL **cab)
{
   NodoL *p;
   if(!*cab){
      puts("remueve: lista vacia");
      return (NodoL *)NULL;
   }
   p=*cab;
   *cab=p->sig;
   p->sig=(NodoL *)NULL;
   return p;
}

NodoL *remueve_(NodoL **cab,  NodoL *p)
{
   NodoL *q,*r;
   if(!*cab){
      puts("remueve: lista vacia");
      return (NodoL *)NULL;
   }
   if(p==*cab){
	*cab=p->sig;
	p->sig=(NodoL *)NULL;  
        return *cab;  
   }
   for(q=(*cab) , r=q->sig; r ;q=q->sig,r=r->sig){
	if(p==r){
             q->sig=r->sig;
             r->sig=(NodoL *)NULL;
             return *cab;
	}
   }
   return *cab;  
}

void imprime(NodoL *inicio, void (*f)(void *)){
   NodoL *p;
   
    if(!inicio){
      puts("imprime:lista vacia");
      return ;
   }
   for(p=inicio;p;p=p->sig)
      (*f)(p->info);
}

NodoL *creaPila(NodoL **stack){
	*stack=(NodoL *)NULL;
	return *stack;
}

int estaVacia(NodoL *stack){
	return !stack;
}

void impNodo(void *dato){ 
   Term *t;
   ChoicePoint *cp;
   if (((Object *)dato)->tipo == TERM) {
	t = (Term*) dato;
	printf("IMPila term <%s>\n", toStringTerm(t));		
   } else if (((Object *)dato)->tipo == CHOICEPOINT) {
	cp = (ChoicePoint*) dato;
	printf("IMPila cp <%s>\n",  toStringTL(cp->goal) );
   }
}

NodoL *push(NodoL **stack, void *dato){
   Term *t;
   ChoicePoint *cp;
   //  printf("PUSH tipo <%p, (%s)>", dato, );
   //printf("PUSH tipo <%p, %d>", dato, ((Object *)dato)->tipo );	
   if (((Object *)dato)->tipo == TERM) {
	t = (Term*) dato;
	printf("PUSH term <%s>\n", toStringTerm(t));
	//if(!strcmp("_0",toStringTerm( t  )))
	//   exit(0);		
   } else if (((Object *)dato)->tipo == CHOICEPOINT) {
	cp = (ChoicePoint*) dato;
	//printf("PUSH cp <%s>", toStringTerm(cp->goal));
	printf("----PUSH cp <%p, %s, %s>\n", cp, 
	    toStringTL(cp->goal), 
	    toStringTerm(cp->goal->term) );
	
   }
   puts("imprime1");
   imprime(*stack, impNodo);
   *stack=creaNodoL(dato, *stack );  
   puts("imprime2 "); 
   imprime(*stack, impNodo);
   
   //exit(0);
   return *stack; 
}

void *top(NodoL *stack){
	return stack->info;
}

NodoL *pop(NodoL **stack){
   NodoL *p;
   void *dato;
   Term *t;
   ChoicePoint *cp;
   
   p=remueve(stack);
   dato=p->info;
   if (((Object *)dato)->tipo == TERM) {
	t = (Term*) dato;
	printf("POP term <%s>\n", toStringTerm(t));
	//if(!strcmp("_0",toStringTerm(t)))
	//   exit(0);		
   } else if (((Object *)dato)->tipo == CHOICEPOINT) {
	cp = (ChoicePoint*) dato;
	//printf("PUSH cp <%s>", toStringTerm(cp->goal));
	printf("POP cp <%p, %s, %s>\n", cp, 
	    toStringTL(cp->goal), 
	    toStringTerm(cp->goal->term) );
		   
   }
   //return remueve(stack);
   imprime(*stack, impNodo);
   return p;	
}

NodoL *cab=NULL;
NodoL *cab1=NULL;


int hashsize;
int iiih;
//NodoL **vardict;
Aches *aches[1000];
Aches *creaAche(char *clave, int valori ){
   Aches *nvo;
   nvo=(Aches *)malloc(sizeof(Aches));
   if(!nvo){
	puts("no hay memoria para crear NodoL");
        return (Aches *)NULL;
   }
   nvo->clave=strdup(clave);
   nvo->valori=valori;
   return nvo;
}
HashEle *creaHashEle(char *clave, void *valor, int tipo){
   HashEle *nvo;
   nvo=(HashEle *)malloc(sizeof(HashEle));
   if(!nvo){
	puts("no hay memoria para crear NodoL");
        return (HashEle *)NULL;
   }
   nvo->clave=strdup(clave);
   nvo->valor=valor;
   nvo->tipo=tipo;
   return nvo;
}
void creaHashTab(NodoL ***htab,int n){
   int i;
   hashsize=n;
   *htab=(NodoL **)malloc(sizeof(NodoL*)*n);
   for(i=0; i < n; i++)
	(*htab)[i]=(NodoL *)NULL;
}
/*unsigned hash(char *s){
    unsigned hashval;
    for (hashval = 0; *s ; s++)
	hashval = *s + MULTIPLIER * hashval;
    return hashval % hashsize;
}*/

// Función hash mejorada
unsigned int hash(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash % HASH_SIZE;
}
int ctact=0;
NodoL *get(NodoL **htab,char *clave){
   NodoL *np;
   int h;
   h=hash(clave);
   printf("GET clave = <<%s, %d>> ", clave, h); 
   //if(!strcmp("fail/0-1", clave)) exit(0); 
   for(np = htab[hash(clave)];np; np = np->sig){
	char *key=((HashEle *)np->info)->clave;
	void *valor=((HashEle *)np->info)->valor;
	int tipo=((HashEle *)np->info)->tipo;
        //void *valor=((HashEle *)np->info)->valor;  
        //imprimeEmp(valor);
        printf("get clave= <%s, %s, %d> ", clave, key, tipo);
        if (strcmp(clave, key) == 0){
            //puts("se encontro");
	    //imprimeEmp(valor);
	    
	    if(tipo==1) 
       printf("TTTipo==1 get clave = <<%s, %s>> ",
       clave, toStringTerm(valor)); 
    if(tipo==2) 
     printf("TTTipo==2 get clave = <<%s, %s, nc=%d, h=%d>> ",
  clave, toStringTL(valor), ((TermList*)valor)->numclauses, h);
            return np;    
	}
   } 
   ctact++;
   //if(!strcmp("fail/0-1", clave)) exit(0);
   //if(ctact==75) exit(0);
   return NULL;
}
void putH(NodoL **htab,char *clave, void *valor, int tipo){
    NodoL *np;
    int h=0; 
    printf("putH clave = <<%s>> ", clave);  
    
    get(htab, clave);//tabla hash con dupli abajo
    
    h=hash(clave);
    
    aches[iiih]=creaAche(clave, h);
    iiih++;
    
    if(tipo==1) 
       printf("TTTipo==1 putHH clave = <<%s, %s>> ",
       clave, toStringTerm(valor)); 
    if(tipo==2) 
     printf("TTTipo==2 putHH clave = <<%s, %s, nc=%d, h=%d>> ",
  clave, toStringTL(valor), ((TermList*)valor)->numclauses, h); 
    
    htab[h]=creaNodoL(creaHashEle(clave, valor, tipo), htab[h]);
    
}

int varnumT=1;
Term *creaTerm() { 
   Term *t = (Term*)malloc(sizeof(Term));
   t->prettyprint = true; 
   t->o.tipo = TERM; 
   t->varid = varnumT++;
   printf("creaTerm VARNUMT = <%d>\n", varnumT);
   t->bound = false;
   t->occurcheck = false;
   t->deref = false;
   //t->internalparse = false;
   return t;
}

Term * creaTerm1(int i) { 
   Term *t = (Term*)malloc(sizeof(Term));
   t->prettyprint = true; 
   t->o.tipo = TERM; 
   t->varid = i;
   printf("creaTerm1 VARNUMPS = <%d>\n", t->varid);
   t->bound = false;
   t->deref = false;
   t->occurcheck = false;
   ///t->internalparse = false;
   return t;
}

Term *creaTerm2(char *s,int a) {
   Term *t = (Term*)malloc(sizeof(Term));
   //t->o.tipo = TERM; 
   strcpy(t->functor, s); 
   t->prettyprint = true; 
   t->arity = a;
   t->bound = true; 
   t->deref = false;
   t->occurcheck = false;
   t->internalparse = false;  
   t->args = (Term**)malloc(sizeof(Term*)*t->arity);
   return t;
}

Term *copyTerm(Term *t) {
   Term *t1 = (Term*)malloc(sizeof(Term));
   t1->prettyprint = t->prettyprint; 
   t1->o.tipo = TERM; 
   strcpy(t1->functor, t->functor); 
   t1->arity = t->arity;
   t1->bound = t->bound; 
   t1->ref   = t->ref;
   t1->deref = t->deref;
   t1->occurcheck = t->occurcheck;
   t1->internalparse = t->internalparse;  
   t1->args = (Term**)malloc(sizeof(Term*)*t->arity);
   for (int j = 0 ; j < t->arity ; j++)
   	      t1->args[j] = t->args[j];
   return t1;
}

void bind(Term *t1, Term *t2) {
   printf("Entra BIND =<<b=%d, dr=%d, t1=%s, t2=%s>>\n", 
    t1->bound, t1->deref, 
    toStringTerm(t1), toStringTerm(t2));
   if (t1 == t2) 
      return;  
   if (!t1->bound) { 
     t1->bound = true; t1->deref = true;       
     t1->ref = t2;
     printf(
 "BIND if =<<b=%d, dr=%d, t1=(%s), t2=(%s),t1->ref=%s>>\n", 
     t1->bound, t1->deref,
     toStringTerm(t1), toStringTerm(t2),
     toStringTerm(t1->ref));
    } else {
       error(toStringTerm(t1), "Can't bind nonvar!");
       //error("Term.bind(" + t1 + ")" ,
       //			     "Can't bind nonvar!");
    }
}

/** Unbinds a term -- ie. resets it to a variable */
void unbind(Term *t) {
   t->bound = false; 
   t->ref= NULL;
}

void setarg(Term *t, int pos,Term *val) {
		// only to be used on bound terms
   if (t->bound & (!t->deref)) t->args[pos] = val;
   else  //error("Term.setarg(" + pos + "," + val + ")",
         error("Term.setarg( )",
			    "Can't setarg on variables!");
}

/** Retrieves an argument of a term */
Term *getarg(Term *t, int pos) {
		// should check if pos is valid
   if (t->bound) {
	if (t->deref) {return getarg(t->ref, pos);}
	else {return t->args[pos];}
   } else {
	fatalerror("Term.getarg", 
		"Error - lookup on unbound term!");
	return NULL; // dummy ... never reached 
   }
}

char *getfunctor(Term *t) {
   if (t->bound) {
      if (t->deref) {return getfunctor(t->ref);}
      else {
           //printf("getfunctor functor <%s> \n", t->functor);
             return t->functor;
           }
   } else {
        puts("cade vacia");
        return "PATOTE";
   }
}
int getarity(Term *t) {
   if (t->bound) {
	if (t->deref) {
	   return getarity(t->ref);
	}
	else return t->arity;
   } else return 0;
}

bool occurs1(Term *t,int var);
bool occurs(Term *t,int var) {
   if (t->varid == var) 
      return false; 
   else return occurs1(t, var);
}
bool occurs1(Term *t,int var) {
   if (t->bound) {
      if (t->deref) return occurs1(t->ref, var);
      else { // bound and not deref
         for (int i=0 ; i < t->arity ; i++) 
	    if (occurs1(t->args[i], var)) 
	        return true;
	 return false;
      }
   } else // unbound
   return (t->varid == var);
}
bool unify(Term *t1, Term *t2, 
   //Stack s) {
   NodoL **s){
   
   Term *ttt;
   //printf("UNIFYUNIFY term <<t= "+
   //	t+" , b = "+bound+" , tb ="+ t.bound+"\n>>");
 printf("UNIFYUNIFY <t2=%s , <b = %s,UNIFYUNIFY tb=%s >>\n", 
       //toStringTerm(t1), unify
       toStringTerm(t2),
       buls[t1->bound], buls[t2->bound]);
      imprime(*s, impNodo); 
   if (t1->bound & t1->deref) {
      puts("t1->bound & t1->deref");
      //return unify(t1, t2, s);
      return unify(t1->ref, t2, s);
   }
   if (t2->bound & t2->deref) {
      printf("t2->bound & t2->deref <%s, %s>\n",
      toStringTerm(t1), toStringTerm(t2) );
      return unify(t1, t2->ref, s);
   }
   
   if (t1->bound & t2->bound) { // bound and not deref
      printf("UNIFY %s == %s____\n",
               getfunctor(t1), getfunctor(t2));
      //exit(0);  
      if ( !strcmp(t1->functor, getfunctor(t2)) & 
	    (t1->arity == getarity(t2))){
	 for (int i=0; i< t1->arity ; i++) 
	    if (!unify(t1->args[i], getarg(t2, i), s)) 
		return false;
	  printf("UNIFYUNIFY true Cr \n");
	    return true;
	} 
	else  {
	  puts("UNIFYUNIFY false Cr");
	  return false; // functor/arity don't match ...
	}

     }  
     if (t1->bound) {
        printf(
     "UNIFYUNIFY OCcur22 =<<%d, t1=%s, t2=%s>>\n", 
         t1->occurcheck, toStringTerm(t1), 
         toStringTerm(t2));
		// return t.unify(this,s);
			// XXXX Added missing occur check
	if (t1->occurcheck) {
	   if (occurs(t1, t2->varid)) {
	        printf(
     "UNIFYUNIFY false OCcur22 =<<%d>>\n", t1->occurcheck);
	        return false;
	   }
	}  // XXXX
	   bind(t2, t1);  //t2.bind(this);
	   push(s, t2);
	   puts("UNIFY=============UNIFY push true");
	   imprime(*s, impNodo);
	   printf("UNIFYUNIFY push true Cr t2=<%s>\n",
	   toStringTerm(t2));
	   return true;
      } 
      		// Do occurcheck if turned on ...
      if (t2->occurcheck) {
	   if (occurs(t2, t1->varid)) return false;
      }    
      bind(t1, t2);
      printf(
     "UNIFYUNIFY salida =<<%d, %s,%s, %d, %d>>\n", 
         t1->occurcheck, toStringTerm(t1), 
         toStringTerm(t2), t1->o.tipo, t2->o.tipo);
      imprime(*s, impNodo);
      //push(&s, copyTerm(t1));
      ttt= copyTerm(t1);
      printf(
     "UNIFYUNIFY111 salida =<<%s>>\n", toStringTerm(ttt));
      //exit(0);
      push(s, ttt);
      //free(t1);
      puts("UNIFYUNIFY salida");
      imprime(*s, impNodo);
      //push(s, t1); // save for backtracking
      //exit(0);
      return true;
}

Term *getvar(Term *l[], int v) ;

Term *refresh(Term *t1, Term *l[]) {
   Term *t2;
   printf("ENTRA refresh b =[ %s , %s]\n", 
          buls[t1->bound], toStringTerm(t1));
   if (t1->bound) {
      if (t1->deref) 
         return refresh(t1->ref, l);
      else { // bound & not deref
	 t2 = creaTerm2(t1->functor, t1->arity);
	 // t.bound = true; t.deref = false; 
	 // t.functor = functor; t.arity = arity;
	 for (int i=0;i<t1->arity;i++) {
	    printf("refresh t1=<<%s, %d>> \n",
	     toStringTerm(t1->args[i]), i);
		t2->args[i] = refresh(t1->args[i], l);
            printf("refresh t2=<<%s, %d>> \n",
	     toStringTerm(t2->args[i]), i);
	 }
   printf("SALI refresh t2=<<%s>> \n", toStringTerm(t2));
	 return t2;
       }
   } else //unbound
   printf("refresh unbound t1=<<%s, varid=%d>> \n",
	     toStringTerm(t1), t1->varid);
   return getvar(l, t1->varid);
}
Term *getvar(Term *l[], int v) {
   if (l[v] == NULL) 
      l[v] = creaTerm();
   return l[v];
}
/** Displays a term in standard notation */
char *toStringTerm(Term *t) {
   char *s;
   s=(char *)malloc(1024);
   if (t->bound) {
      if (t->deref) 
         return toStringTerm(t->ref);
      else {
	 if (!strcmp(t->functor, "null") & t->arity==0 
	     & t->prettyprint)
	    return "[]";
	    if (!strcmp(t->functor, "cons") & t->arity==2 & 
	        t->prettyprint) {
	       Term *t1; 
	       strcpy(s, "[");
	       strcat(s, toStringTerm(t->args[0]));
	       //s = "[" + t->args[0];
	       t1 = t->args[1];
	       while (!strcmp(getfunctor(t1), "cons") &
			getarity(t1) == 2) {
		  strcat(s, ",");
		  strcat(s, toStringTerm(getarg(t1, 0)));
	          //s = s + "," + getarg(t1, 0);
		  t1 = getarg(t1, 1);  
	       }
	       if (!strcmp(getfunctor(t1),"null") &
					getarity(t1) == 0) 
		  strcat(s, "]");			
	          //s = s + "]";
	       else {
	          strcat(s, "|");
	          strcat(s,toStringTerm(t1));
	          strcat(s, "]");
	          //s = s + "|" + t1 + "]";
	       }
	       return s;
	     } else {
	        //s = t->functor;
	        strcpy(s, t->functor);
		if (t->arity > 0) {
		   strcat(s, "(");   
		   //s = s + "(";
		   for (int i=0; i < (t->arity - 1); i++){
		      strcat(s, toStringTerm(t->args[i]));
		      strcat(s, ",");
		     //s =s + toStringTerm(t->args[i]) + ",";
		   }
              strcat(s, toStringTerm(t->args[t->arity-1]));
		 strcat(s, ")");
	  //s = s + toStringTerm(t->args[t->arity-1]) + ")";
		}
	     }
	     return s;
      }
   } else {  
      char *sss=(char *)malloc(256);
      sprintf(sss,"_%d", t->varid);
      return sss;
   }
}

/** This constructor is the simplest way to construct a term. The term is 
given in standard notation. 
Example <tt>Term(new ParseString("p(1,a(X,b))"))</tt>
@see ParseString
*/
bool Terminternalparse;

char* substring(const char *src, unsigned int start, 
             unsigned int end);
int www=0;
Term *creaTerm3(ParseString *ps) /*throws Exception*/ {
   Term **ts; 
   int i=0; 
   Term *t;  
   char *s;
   Term *tt = (Term*)malloc(sizeof(Term));
   tt->o.tipo = TERM; 
   tt->prettyprint = true; 
   tt->occurcheck = false;
   //tt->internalparse=false; no
   //ts = new Term[300]; 
     
   printf("creaTer3 str=[%d, %c] ", 
   //printf("creaTer3 str=[%s, %d, %c] ", 
   //ps->str, 
   ps->posn, ps->str[ps->posn]);
   //, getname(ps));
   ts = (Term **)malloc(300*sizeof(Term *));
   if (islower(current(ps)) | 
      (Terminternalparse & current(ps) == '_') ){
      strcpy(tt->functor, getname(ps));
      printf("if creaTer3 tt->functor=[%s] ", tt->functor);
      
      tt->bound = true; tt->deref = false;
      if (current(ps) == '(') {
	  advance(ps); skipspace(ps);
	  ts[i++] = creaTerm3(ps);
	  skipspace(ps);
	  while (current(ps) == ',') {
		advance(ps); skipspace(ps);
		ts[i++] = creaTerm3(ps);
		skipspace(ps);
	  }
	  if (current(ps) != ')') 		      
	     /*ps.*/parseerror("Expecting: ``)''");
	  
	  advance(ps);
	  //args = new Term[i];
          tt->args = (Term**)malloc(sizeof(Term*)*i);
	  for (int j = 0 ; j < i ; j++){
	      tt->args[j] = ts[j];
	  printf("creaTer3 args = <%d,%d, %s>",
	  i, j, toStringTerm(tt->args[j]));
	  }   		
	   tt->arity = i;
      } else tt->arity = 0;
   } else
      if (isupper(current(ps))) {
         printf("VARVARVAR <%d, %d>\n", 
             tt->varid, ps->varnum);
         tt->bound = true; 
         tt->deref = true;
	 tt->ref = getvarPS(ps);
	 //exit(0);
      } else
	if (isdigit(current(ps))) {
	   puts("NUM");
	   strcpy(tt->functor, getnum(ps));
	   tt->arity = 0;
	   tt->bound = true ; 
	   tt->deref = false;
	} else
	   if (current(ps) =='[') {
	      advance(ps);
	      if (current(ps) == ']') {
		  advance(ps);
		  strcpy(tt->functor, "null"); 
		  tt->arity = 0;
		  tt->bound = true; tt->deref = false;
	       } else {
		  skipspace(ps);
		  ts[i++] = creaTerm3(ps);
		  skipspace(ps);
		  while (current(ps)==',') {
	              advance(ps); skipspace(ps);
		      ts[i++] = creaTerm3(ps);
		      skipspace(ps);
		  }
		  if (current(ps) == '|') {
			advance(ps); skipspace(ps);
			ts[i++] = creaTerm3(ps);
			skipspace(ps);
		  } else ts[i++] = creaTerm2("null",0);
		     if (current(ps) != ']') 
			/*ps.*/parseerror("Expecting ``]''");
		     advance(ps);
		     tt->bound = true; tt->deref = false;
		     strcpy(tt->functor,"cons"); 
		     tt->arity = 2;
		     //args = new Term[2];
		     tt->args = (Term**)malloc(sizeof(Term*)*2);
		     for (int j=i-2; j>0 ; j--) {
			t = creaTerm2("cons",2);
			setarg(t, 0, ts[j]);
			setarg(t, 1, ts[j+1]);
			ts[j] = t;
	  printf("creaTer3 lista args = <%d,%d, %s>",
	  i, j, toStringTerm(ts[j]));
		    }
		    tt->args[0] = ts[0]; 
		    tt->args[1] = ts[1];
	       }
	} else /*ps.*/parseerror(
  "Term should begin with a letter, a digit or ``[''");
        //puts("creaTerm3 antes return tt");
        s = substring(ps->str, ps->start, ps->posn);
   printf("creaTerm3  antes return tt s= <<%s>> ", s);
          for (int j = 0 ; j < tt->arity ; j++){
	     printf("creaTer3 args55 = <%d, %s, %d>",
	        j, toStringTerm(tt->args[j]), tt->bound);
	  } 
	  for (int j = 0 ; j < tt->arity ; j++){
	     printf("creaTer3 args ts1= <%d, %s, %s>",
	  j, toStringTerm(ts[j]), toStringTerm(tt->args[j]));
	  } 
        return tt;
}

TermList *creaTermList1(Term *t1, TermList *n) {
   TermList *t = (TermList*)malloc(sizeof(TermList));
   t->term = t1; 
   t->next =  n;
   //t->numclauses = 0;//ver
   return t;
}
TermList *creaTermList2(Term *t1, TermList *n,TermList *d) {

   printf("creaTermList2 <%s, d=%s, %d>\n", 
   toStringTerm(t1), toStringTL(d), d->numclauses);
   TermList *t = (TermList*)malloc(sizeof(TermList));
   t->term = t1; t->next = n;
   t->definer = d->definer;
   t->numclauses = d->numclauses;
   return t;
}

char *toStringTL(TermList *t) {
   char *s; 
   TermList *tl;
   
   s=malloc(4096);
   strcpy(s, "[");  strcat(s, toStringTerm(t->term)); 
   
   tl = t->next;
   int i=0;
   while (tl != NULL) {
     strcat(s, ", " );  strcat(s, toStringTerm(tl->term));
     tl = tl->next;
     //printf("22TOOOO toStringTL =<%s> ", s);
     i++;
   }
   if (t->definer != NULL) {
       char nume[512];
       sprintf(nume, "%d", t->numclauses);
       strcat(s, "( " );
       strcat(s, nume);
       strcat(s, " clauses)");
       //s = s + "( " + t->numclauses + " clauses)";
   }
   
   return strcat(s, "]");
}

/** Looks up which clauses define atoms once and for all */
void resolve(TermList *tl, //Hashtable
   NodoL **db) {
   char *index;
   printf("\ntl->definer222=<%p, %d, %p, %s>\n", 
   tl, tl->numclauses, tl->definer, toStringTL(tl)); 
   
   if (tl->definer == NULL) {
      tl->numclauses = 0;
     
      index = (char *)malloc(256);
      sprintf(index, "%s/%d-%d", getfunctor(tl->term), 
      getarity(tl->term), (1+tl->numclauses));
      
       while (get(db, index) != NULL) { 
          printf("00Resolve numclauses=<%d, %s> ",
                tl->numclauses , index);
          tl->numclauses++;
       
          sprintf(index, "%s/%d-%d", getfunctor(tl->term), 
      getarity(tl->term), (1+tl->numclauses));
      printf("11RESOLVE  numclauses (%d, index= %s) ", 
                          tl->numclauses, index);
          
          //exit(0);
       }
       
       tl->definer = (TermList**) malloc(
          sizeof(TermList*)*(tl->numclauses+1));	
      //tl->definer = new TermList[tl->numclauses+1]; // start numbering at 1
      
      printf("resolve LIM =<%d, %s> ", tl->numclauses,
      toStringTL(tl)
      );
      
      //exit(0);
      for (int i=1; i <= tl->numclauses ;i++) {
         index = (char *)malloc(256);
      sprintf(index, "%s/%d-%d", getfunctor(tl->term), 
      getarity(tl->term), (i));     
          tl->definer[i] = ((TermList*) 
	  ((HashEle *)get(db, index)->info)->valor);
         printf(
      "REsolveMapota  %s/%d-%d\n", getfunctor(tl->term), 
         getarity(tl->term), (i));
	printf("MAPOTAMAPOTA def[%d]=<<%s, %d, %s>> \n",
	   i, toStringTL(tl->definer[i]), tl->numclauses,
	   index);
	 
      }
      if(!strcmp("[wooden(_0), floats(_0)( 2 clauses)]",
       toStringTL(tl)))
         exit(0);
   printf("resolve fin for this=(%s, %s)",
   toStringTL(tl), toStringTerm(tl->term));
      if (tl->next != NULL) {
         puts("tl->next != NULL");
         resolve(tl->next, db);
      }
   }
}
/** Used for parsing  clauses. */

TermList  *creaTermList3(ParseString *ps) {
   TermList *tt = (TermList*)malloc(sizeof(TermList));
   Term **ts;//[]; 
   int i=0; 
   TermList **tsl;//[];
   ts =  (Term **)malloc(300*sizeof(Term *));
   //new Term[300]; // arbitrary
   tsl = (TermList **)malloc(300*sizeof(TermList *));
   //new TermList[300]; // arbitrary
   //exit(0);
   printf("555TermListPS antes creaTerm3(ps) ps->str= <<%s, %d, %d, %c>> \n",  
   ps->str, strlen(ps->str), ps->posn, ps->str[ps->posn]);
   ts[i++] = creaTerm3(ps); 
   
   skipspace(ps);
    
   printf("66TermListPS ANTES [:]  <<%d, %d, %c>> \n",
   //ps->str, 
   strlen(ps->str), ps->posn, ps->str[ps->posn]);
   //exit(0);
   if (current(ps) == ':') {
        
   printf("77TermListPS [:] = <<%d, %d, %c>> \n",  
   //ps->str, 
   strlen(ps->str), ps->posn, ps->str[ps->posn]);
      advance(ps);
      //exit(0);
      if (current(ps) != '-') {
	 /*ps.*/parseerror("Expecting ``-'' after ``:''");
      }
      advance(ps); skipspace(ps);
      //exit(0);
      ts[i++] = creaTerm3(ps);
      //exit(0);
      skipspace(ps);
      while (current(ps) == ',') {
         //exit(0);
         advance(ps); skipspace(ps);
         printf("11creaTermList i=<%d> ", i);
         //exit(0);
         Term *t3=creaTerm3(ps);
         
         printf("88creaTermList t3=<%s, %d> ",
         toStringTerm(t3), i);
	 ts[i++] = t3;
	 //exit(0);
	 skipspace(ps); 
	 printf("22creaTermList i=<%d> ", i);
	 //exit(0);
      }
      //exit(0);
      tsl[i] = NULL;
      //exit(0);
      for (int j = i - 1 ; j > 0 ; j--)
	 tsl[j] = creaTermList1(ts[j], tsl[j+1]);
      //exit(0);
      tt->term = ts[0];
      tt->next = tsl[1];
       printf("<%s>", toStringTL(tt));
   } else {
      tt->term = ts[0]; 
      tt->next = NULL;
   }
   //printf("33creaTermList  = <%p, %p, %s, %d, %d, %d >", 
   printf("33creaTermList  = <%p, %p, %d, %d, %d >", 
      tt, tt->term, 
      //ps->str, 
      ps->start, 
      ps->posn, strlen(ps->str) );
   //exit(0);
   if (current(ps) != '.')
      /*ps.*/parseerror("Expecting ``.''");		
   advance(ps);
   //exit(0);
   return tt;
}

bool empty(ParseString *ps);



char* substring(const char *src, unsigned int start, 
             unsigned int end){
    if (start > end || end >= strlen(src)) {
        return NULL; // Return NULL for invalid range
    }

    unsigned int subtext_len = end - start + 2;
    char *subtext = malloc(subtext_len);

    if (subtext == NULL) {
        return NULL; // Return NULL if memory allocation fails
    }

    strncpy(subtext, &src[start], subtext_len - 1);
    subtext[subtext_len - 1] = '\0';
    return subtext;
}

char *toString(ParseString *ps) {
   char *cad=(char *)malloc(512);
   
   sprintf(cad,
      "{ %s ^ %s | %p }\n", substring( ps->str, 0, ps->posn), 
        substring(
   ps->str, ps->posn, strlen(ps->str)) , ps->vardict);
   //ps->str, ps->posn, strlen(ps->str)) , ps->vardict);
   return  cad;
   
}
/** Initialise variables */
ParseString *creaParseString(char *s) {
   ParseString *ps = (ParseString*)malloc(sizeof(ParseString));
   //ps->str = s; 
   ps->str = strdup (s) ; 
   ps->posn = 0; 
   ps->start = 0; 
   ps->varnum = 0;
   creaHashTab(&ps->vardict, 1000);
   //creaHashTab(&vardict, 1000);
   //ps->vardict = new Hashtable();
   printf("\n\nCREAParseString ps->varnum = <%d>\n\n",
   ps->varnum);
   return ps;
}
/** Get the current character */
char current(ParseString *ps) {
   if (empty(ps)) 
      return '#'; //  can't be space
   else {
     return ps->str[ps->posn];
   }
}
/** Is the parsestring empty? */
bool empty(ParseString *ps) {
     
   return ps->posn == (strlen(ps->str));
}
/** Move a character forward */
void advance(ParseString *ps) {
   ps->posn ++;
 printf("Adva i=<%d,%d, carac=[%c], %d> \n", 
     ps->posn, strlen(ps->str),
     ps->str[ps->posn], (ps->posn >= strlen(ps->str)));
   if (ps->posn >= strlen(ps->str)){
     //printf(" i STR=<%d,%s> ", ps->posn, ps->str);
     printf(" i STR=<%d> ", ps->posn);
     ps->posn = strlen(ps->str);
   }
}
// all three get methods must be called before advance.
/** Recognise a name (sequence of alphanumerics) */
char *getname(ParseString *ps) {
   char *s;
   ps->start = ps->posn; ps->posn ++;
   while ( isdigit(current(ps)) |
	   islower(current(ps)) |
	   isupper(current(ps))) 
      ps->posn++;
   //printf("getname  STR= <<%s,%d,%d,%d,%d>> ",    
          //ps->str, 
   printf("getname  = <<%d,%d,%d,%d>> ", 
          ps->start, ps->posn, 
          strlen(ps->str), ps->posn >= strlen(ps->str) );
   //exit(0);
   //s = strdup(substring(ps->str, ps->start, ps->posn));
   s = substring(ps->str, ps->start, ps->posn-1);
   printf("SUB getname  s= <<%s>> ", s);
   //exit(0);
   if (ps->posn >= strlen(ps->str)) 
      ps->posn = strlen(ps->str) ; 
   return s;
}
/** Recognise a number */
char *getnum(ParseString *ps) {
   char *s;
   ps->start = ps->posn; ps->posn ++;
   while (isdigit(current(ps))) 
      ps->posn++;
   s = substring(ps->str, ps->start, ps->posn);
   printf("getnum  s= <<%s>> ", s);
   if (ps->posn >= strlen(ps->str)) 
      ps->posn = strlen(ps->str) ; 
   return s;
}
/** Get the Term corresponding to a name. If the name is new, then create a 
new variable */
int ctaH;
Term *getvarPS(ParseString *ps) {
   char *s; 
   Term *t;
   NodoL *n;
   int tipo;
   s = getname(ps);
   //---------n = get(vardict, s);
   n = get(ps->vardict, s);
   printf("getvarPS getname  s= <<%s, %p>> ", s, n);
   if( n ) {
       tipo =(((HashEle *)(n->info))->tipo);
       printf("getvarPS getname  s= <<%s, tipo=%d>> ", s, tipo);
   }
   //exit(0);
   //t = (Term *) get(ps->vardict, s);
   
   //t = (Term *) (((HashEle *)(get(vardict, s)->info))->valor);
   if(n) {
      //t = (Term *) (((HashEle *)(get(vardict, s)->info))->valor);
      t = (Term *) (((HashEle *)(get(ps->vardict, s)->info))->valor);
      printf(
"TRUE getvarPS TERM  t= <<%s, varid=(%d), %s, varnum=%d>> \n", 
      toStringTerm(t), t->varid, s, ps->varnum);
   }
   if (n == NULL) {
      puts("n == NULL");
      t = creaTerm1(ps->varnum++); // XXXX wrong varnum??
      printf("getvarPS t=<%p, varid=(%d), varnum=%d> \n", 
                                 t, t->varid, ps->varnum);
      //put(ps->vardict, s,t);
      printf("getvarPS TERM  t= <<%s, %d, %s>> ", 
      toStringTerm(t), ctaH, s);
      //putH(vardict, s,t, 1);
      putH(ps->vardict, s,t, 1);
      
   }
   //exit(0);
   return t;
}
/** Handle errors in one place */
void parseerror(char *s) /*throws Exception*/ {
   char cad[256];
   strcpy(cad, "Unexpected character : ");
   strcat(cad, s);
   diagnostic(cad);
   //throw new Exception();
}
/** Skip spaces. Also skips Prolog comments */
void skipspace(ParseString *ps) /*throws Exception*/ {
   while (current(ps) == ' ' || current(ps) == '\n' )
	advance(ps);
   skipcomment(ps);
}
void skipcomment(ParseString *ps) /*throws Exception*/ {
   if (current(ps) == '%') {
      while (current(ps) != '\n' & current(ps) != '#') 
         advance(ps);
      skipspace(ps);
   }
   if (current(ps) =='/') {
      advance(ps);
      if (current(ps) !='*') parseerror("expecting ``*''");
      else {
         advance(ps);
	 while (current(ps) != '*' & current(ps) != '#')
	    advance(ps);
	 advance(ps);
	 if (current(ps) !='/') 
	    parseerror("expecting ``/''");
         advance(ps);
      }
      skipspace(ps);
   }
}
/** This resets the variable dictionary. */
void nextclause(ParseString *ps) {
   // create new variables for next clause 
   creaHashTab(&ps->vardict, 1000);
   //-----creaHashTab(&vardict, 1000);
   //new Hashtable();
   
   ps->varnum = 0;
}
/*Hashtable*/ NodoL **consult(char *s,
                              //Hashtable db)
                              NodoL **db) 
		/*throws Exception*/ {
   printf("consult s= [<%s>]", s);
   ParseString *ps = creaParseString(s); //tc
   char *func, *prevfunc, *index;
   int clausenum, arity, prevarity;
   TermList *tls;
   skipspace(ps);
   prevfunc = ""; prevarity = -1;
   //exit(0);
   clausenum = 1;
   while (! empty(ps)) {
      //exit(0);
      //printf("000consult FUNC = <%s, %d, %d, %d >", 
      printf("000consult FUNC = <%d, %d, %d >", 
      //ps->str, 
      ps->start, ps->posn, 
      strlen(ps->str) );
      tls = creaTermList3(ps);
      
      printf("111consult tls = <%s, nc = (%d) >", 
      toStringTL(tls), tls->numclauses);
      //exit(0);
      printf("111consult FUNC = <%p, %p, %s, %d, %d, %d >", 
      tls, tls->term, ps->str, ps->start, ps->posn, 
      strlen(ps->str) );
      //, tls->term);
      //exit(0);
      func = getfunctor(tls->term);
      //exit(0);
      printf("consult func = <%s>", func);
      //exit(0);
      arity = getarity(tls->term);
      if (!strcmp(func, prevfunc) & arity == prevarity) 
         clausenum++;
      else {
	 clausenum = 1;
	 prevfunc = func;
	 prevarity = arity;
      }
      index = (char *)malloc(256);
      sprintf(index, "%s/%d-%d", func, arity, clausenum);
      //index = func + "/" + arity + "-" + clausenum;
      //println("22Mapota "+index);
      printf(
      "22Mapota: <<%s/%d-%d>>\n", func, arity, clausenum);
      /*if(!strcmp("[wooden(_0), floats(_0)]",
       toStringTL(tls)))
         exit(0);*/
      putH(db, index, tls, 2);
      
      skipspace(ps);
      nextclause(ps); // new set of vars
      //exit(0);
   }
   //------exit(0);
   return db;
} // consult

int cta46=0;
//void resolvePS(NodoL **db) {
void resolvePS(NodoL **db) {
   NodoL **vardict; 
   int j;
   vardict=db;
   j=0;
   for (int i=999; i>= 0 /*modi*/; i--){//721
     // puts("FORFORFOR resolvePS");
     
      NodoL *entry= vardict[i];
      if(entry != NULL){ 
         printf("FOR FORfor vardict <%d,%p, %d>\n", i, vardict[i],
         ((HashEle*)((NodoL*)vardict[i])->info)->tipo);
      if(((HashEle*)((NodoL*)vardict[i])->info)->tipo == 2){
      
      
         printf("0000resolvePS 2o for ( %s, %d, %d, %d)",   
    ((HashEle*)((NodoL*)vardict[i])->info)->clave, i, j, 
  ((TermList*)
  (((HashEle*)((NodoL*)vardict[i])->info)->valor))->numclauses);
  
    printf("FOR FORfor TL <%s>\n", 
  toStringTL(((TermList*)
  (((HashEle*)((NodoL*)vardict[i])->info)->valor))  ) );
     }
     
     if(((HashEle*)((NodoL*)vardict[i])->info)->tipo == 1){
         printf("1111resolvePS 2o for tipo 1 ( %s, %d, %d, %s)",   
    ((HashEle*)((NodoL*)vardict[i])->info)->clave, i, j, 
  ((Term*)
  (((HashEle*)((NodoL*)vardict[i])->info)->valor))->functor);
  //mal ((TermList*) (((NodoL*)vardict[i])->info))->numclauses);
    printf("FOR FORfor Term <%s>\n", 
  toStringTerm(((Term*)
  (((HashEle*)((NodoL*)vardict[i])->info)->valor))  ) );
     }
     //toStringTL((TermList*) (((NodoL*)vardict[i])->info)));
         j++;
         //exit(0);
      }
      while (entry != NULL){
         //exit(0);
  //--if(((HashEle*)((NodoL*)vardict[i])->info)->tipo == 2){
    if(((HashEle*)((NodoL*)entry)->info)->tipo == 2){
         printf("--PUTPUTPUT ( %s, %d, %d)",   
    //((HashEle*)((NodoL*)vardict[i])->info)->clave, i,
      ((HashEle*)((NodoL*)entry)->info)->clave, i,
      ((TermList*) 
    //((HashEle*)((NodoL*)vardict[i])->info)->valor)->numclauses);
      ((HashEle*)((NodoL*)entry)->info)->valor)->numclauses);
    //tll->numclauses);
    
        resolve(
     ((TermList*) 
   // ((HashEle*)((NodoL*)vardict[i])->info)->valor), db);
   ((HashEle*)((NodoL*)entry)->info)->valor), db);
         //(TermList*) (((NodoL*)vardict[i])->info), db);
    }
    
    if(((HashEle*)((NodoL*)vardict[i])->info)->tipo == 2)
         printf("==PUTPUTPUTTL ( %s, %d)",   
    toStringTL(((TermList*) 
    ((HashEle*)((NodoL*)vardict[i])->info)->valor))
    , i);
    if(((HashEle*)((NodoL*)vardict[i])->info)->tipo == 1)
         printf("==PUTPUTPUTTerm ( %s, %d)",   
    toStringTerm(((TermList*) 
    ((HashEle*)((NodoL*)vardict[i])->info)->valor))
    , i);
    printf("==555PUTPUTPUT ( %s, %d)",   
    ((HashEle*)((NodoL*)vardict[i])->info)->clave, i);
         //exit(0);       
         entry = entry->sig;
         
         if(!entry) puts("SIG NULO");
      } 
      
      //}       
   }
   //--exit(0); 
}

OnceMark *creaOnceMark() {
   OnceMark* om = (OnceMark*)malloc(sizeof( OnceMark));  
   ((Object*)om)->tipo = ONCEMARK;
   return om;
}

Engine *creaEngine(Term *g, NodoL **prog) {
   Term  *t, *t2;
   TermList *tl;
   Engine *eng=(Engine *)malloc(sizeof(Engine)); 
   //stack = new Stack(); 
   creaPila(&eng->stack);
   eng->goal = creaTermList1(g, NULL);
   eng->call = g;
   // enable underscore as first character of term
   Terminternalparse = true; 
   //try {
   eng->db = /*ParseString.*/
   
   consult(
       ("eq(X,X). fail :- eq(c,d). print(X) :- _print(X).\
if(X,Y,Z) :- once(wprologtest(X,R)) , wprologcase(R,Y,Z).\
wprologtest(X,yes) :- call(X).wprologtest(X,no). \
wprologcase(yes,X,Y) :- call(X). \
wprologcase(no,X,Y) :- call(Y).\
not(X) :- if(X,fail,true).or(X,Y) :- call(X). \
or(X,Y) :- call(Y).\
true. call(X) :- _call(X). \
nl :- _nl.once(X):-_onceenter , call(X) , _onceleave.") , prog);
		  puts("000creaEnginede vuelta de  consult");
		    //exit(0);
      /*ParseString.*/  resolvePS( eng->db);
      //exit(0);
   //} catch (Exception e) {
     //----IO.fatalerror("Engine.init","Can't parse default program!");
   //}
   Terminternalparse = false; 
   //exit(0);
   resolve(eng->goal, eng->db);//estaba comentado
   printf(
   //"resolve GOAL.numclauses = <%d>", 
   //   eng->goal->numclauses);
      "resolve GOAL.numclauses = <%d, %s>", 
      eng->goal->numclauses, toStringTerm(eng->goal->term));
   //exit(0);
   eng->failgoal = creaTermList1(creaTerm2("fail",0), NULL);
   resolve(eng->failgoal, eng->db);

   //setPriority(2);
   //time = System.currentTimeMillis();
   puts("llcreaEngine");
   return eng;
}
	
void dump(Engine *e, int clausenum) {
   char *msg = (char *)malloc(256);
      sprintf(msg, 
      "dump Goal:  %s clausenum = %d e->goal->numcla=%d \n", 
        toStringTL(e->goal) , clausenum, e->goal->numclauses);
   puts(msg);
   if (e->trace) {
      trace(msg);
   }
}
void run(Engine *e) { 
   char *r;
   r = run1(e, false); 
   result(r);
}
/** run does the actual work. */
char *run1(Engine *e, bool embed) { 
   bool found;
   char *func;
   int arity, clausenum;
   TermList *clause, *nextclause, *ts1, *ts2;
   Term *t, *tu;
   //Object o;dump
   void *o;
   ChoicePoint *cp;
   Term **vars;
   clausenum = 1;
   char *msj;
   
   if(e->goal)
     printf("_0000RUN1 <egoal %d,%d,%d, %s>\n", 
        e->goal->numclauses, clausenum,
        (e->goal->numclauses > clausenum ), 
        toStringTL(e->goal));
   while (true) {
     puts("INIC WHIlE");
     //exit(0);
      if (e->goal == NULL) {
        msj=(char *)malloc(1024);
        strcpy(msj, "Yes: ");
        strcat(msj, toStringTerm(e->call));
        //printf("Yes: %s\n", toStringTerm(e->call));
        return msj;
        
	return;
      }
      if (e->goal->term == NULL) {
         fatalerror("Engine.run","goal.term is null!");
      }
      func = getfunctor(e->goal->term);
      arity = getarity(e->goal->term); // is this needed?
      printf("RUNrun e->goal->numclauses= %d %s\n", 
      e->goal->numclauses, toStringTL(e->goal));
      dump(e, clausenum);
      //exit(0);
      if (func[0] != '_') { 
	// ie the goal is not a system predicate AAA
	// if there is an alternative clause push choicepoint:
	printf("111RUN1 <egoal %d,%d,%d, %s/%d %s>\n", 
        e->goal->numclauses, clausenum,
        //(e->goal->numclauses > clausenum  ));
        (clausenum > e->goal->numclauses), func, arity,
        toStringTL(e->goal));
        
         if (e->goal->numclauses > clausenum){
	  //stack.push(new ChoicePoint(clausenum +1, goal));
  printf("0077RUN PUsh clausenum %d, << %s, %s>>\n",
  clausenum, toStringTL(e->goal),
  toStringTerm(e->goal->term));  
  		  
  push(&e->stack, creaChoicePoint(clausenum +1, e->goal));
  
  printf("0077run vacia=<%d>\n", estaVacia(e->stack));
         }
  printf("0088RUN clausenum %d, << tl=%s, %s>>\n",
  clausenum, toStringTL(e->goal),  
  toStringTerm(e->goal->term));
	    if (clausenum > e->goal->numclauses) {
	       char msj[256];
	       sprintf(msj, "%s/%d  undefined!", func, arity);
	       clause = e->failgoal;
	       diagnostic(msj);
	       clause = creaTermList1(
	       creaTerm2("fail",0), e->goal);
	       
	       //new TermList(new Term("fail",0), goal);
	       resolve(clause, e->db);
	    } else {
    
    printf("else e->goal->definer[%d]=<%s, %s>\n", 
    clausenum,
    toStringTL(e->goal->definer[clausenum]), 
    toStringTerm((e->goal->definer[clausenum])->term)  );
    
    printf("else e->goal->definer=<%s,>\n", 
    //%s, %s, %s>\n", 
    toStringTL(e->goal->definer[1])//, 
    );
    
    printf("Mapota00 def=<<%s, %s>> \n",
	     toStringTerm(e->goal->term), 
	     toStringTL(e->goal));
	 clause = e->goal->definer[clausenum];
    printf("Mapota11 def=<<%s, %s>> \n",
	     toStringTerm(clause->term), 
	     toStringTL(clause));
		
            }
   printf(
"00run clausenum <%d> %s clause.term <%s>  goal.term <%s> \n", 
       clausenum,
         toStringTL(clause), toStringTerm(clause->term), 
       toStringTerm(e->goal->term) );  
	    clausenum = 1; // reset ...
		// check unification ...
	    //vars = new Term[300]; // XXX arbitrary limit
	    vars = (Term**) malloc(sizeof(Term*)*300);    
	     
      tu=refresh(clause->term, vars);
      printf(
	"00run TU < %s, gt= %s >\n", 
	toStringTerm(tu), toStringTerm(e->goal->term));
            if (
   //unify( refresh(clause->term, vars), 
   unify( tu, 
   e->goal->term, &e->stack)) {
                printf("00Yes: %p\n", clause );
		   clause = clause->next;
		printf("11Yes: %p\n", clause );
		// refresh clause -- need to also copy definer 
		
		puts("1111run clause ");
	        if (clause != NULL) {
	   printf("ANTNEXT___3322run <%s, (%s)>\n", 
    toStringTL(clause), toStringTerm(clause->term));
		      ts1 = creaTermList2(
	                     refresh(clause->term, vars), 
			       NULL, clause);
   printf("2222run <%s>\n", toStringTL(ts1));
		      ts2 = ts1;
		      clause = clause->next;
		      while (clause != NULL ) {
			 ts1->next = creaTermList2(
		           refresh(clause->term, vars),
			 NULL , clause);
			 ts1 = ts1->next;
	printf("3333run <%s, %s>\n", toStringTL(ts1), 
	toStringTerm(ts1->term));
			 clause = clause->next;
		      }
	// splice together refreshed clause and other goals
		      ts1->next = e->goal->next;
		      e->goal = ts2;
	// For GC purposes drop references to data that is not 
	// needed 
		      t = NULL; ts1 = NULL; 
		      ts2 = NULL; vars = NULL; 
		      clause = NULL; nextclause = NULL; 
		      func = NULL;
		} else { // matching against fact ...
		   e->goal = e->goal->next;
    //printf("4444run <%s, %s>\n", toStringTL(e->goal), 
    //	toStringTerm(e->goal->term)); 
		}
		puts("if ex  ");
		
	    } else { // unify failed - backtrack ...
		e->goal = e->goal->next;
     printf("666600run vacia=<%s>\n",
      buls[estaVacia(e->stack)] );
     if(e->goal)
     printf("666611run goal=<%s>\n", toStringTL(e->goal));
	//<%s>\n", toStringTL(e->goal));	
		found = false;
		while (! estaVacia(e->stack)) {
		           //o = stack.pop();
                    o = pop(&e->stack)->info;
              printf("__66run POP vacia=<%s>\n", 
               buls[estaVacia(e->stack)]);	     
              // (! ((Object *)o)->tipo == ONCEMARK ) 
                    //if (o instanceof Term) { 
		    if (((Object *)o)->tipo == TERM) {
			t = (Term*) o;
              printf("6666run POP t=<%s> vacia=<%s>\n", 
              toStringTerm(t), buls[estaVacia(e->stack)]);	
			unbind(t);
		    } else if (
		    ((Object *)o)->tipo == CHOICEPOINT) {
			cp = (ChoicePoint*) o;
			e->goal = cp->goal;
	printf("7777run POP <%s, %s>\n", 
	toStringTL(e->goal), 
	toStringTerm(e->goal->term)); 		
			clausenum = cp->clausenum;
			found = true;
			break;
		    } 
		  // else if integer .. iterative deepening 
		    // not implemented yet
	        }
	        printf("6666run fin while <%s, found=<%d>>\n", 
	        buls[estaVacia(e->stack)], found);
	  // stack is empty and we have not found a choice 
	        // point ... fail ...
	        if (!found) {
	             printf("NONONO <%s, found=<%d>>\n", 
	        buls[estaVacia(e->stack)], found);
				    return "No."; // IO.result("No.");
				    // this.suspend();
				    // return;
	        }
	   }
	 //exit(0);  
	} // AAA
	
	else if (!strcmp(func,"_print") & arity==1) {
		if (embed) 		    
     puts(toStringTerm(getarg(e->goal->term, 0)));
		else 			
prologprint(toStringTerm(getarg(e->goal->term, 0)));
		e->goal = e->goal->next;
	}
	else if (!strcmp(func,"_nl") & arity==0) {
		if (embed) 
			puts("");
		else 
			prologprint("\n");
		e->goal = e->goal->next;
	}
	else if (!strcmp(func,"_call") & arity==1) {
	// System.out.println(goal.term);
	// System.out.println(goal.term.getarg(0));
		TermList *templist = 
	creaTermList1(getarg(e->goal->term, 0), NULL);
		resolve(templist, e->db);
		templist->next = e->goal->next;
		e->goal = templist;
	}
	// The next two together implement once/1
	else if (!strcmp(func,"_onceenter") & arity==0) {
	        
		//stack.push(new OnceMark());
		push(&e->stack, creaOnceMark());
		e->goal = e->goal->next;
	}
	//else if (func.equals("_onceleave") & arity==0) {
	else if (!strcmp(func,"_onceleave") & arity==0) {
	// find mark, remove mark and all choicepoints above it.
	 NodoL * tempstack;
	 creaPila(&tempstack);
	 //Stack tempstack = new Stack();
	 
	 //o = stack.pop(stack);
	 o=pop(&e->stack)->info;
	 while (! ((Object *)o)->tipo == ONCEMARK )  {
	    // forget choicepoints
	    if (! ((Object *)o)->tipo == CHOICEPOINT)     
	    //tempstack.push(o);
	    push(&tempstack, o);
	    //o = stack.pop();
	    o=pop(&e->stack)->info;
	 }
	 while (! (estaVacia(tempstack))) 
	    push(&e->stack, pop(&tempstack)->info);
	 tempstack = NULL;
         e->goal = e->goal->next;
      }
      else {
         char msj[256];
        sprintf(msj, "Unknown builtin: %s/%d", func, arity);
	 diagnostic( msj );
	 e->goal = e->goal->next;
      }	
      //exit(0);
   } // while
   //exit(0);
} // run

void more(Engine *e) {
	char *r;
        r = more0(e, false) ;
}
char *more0(Engine *e, bool b) {
   e->goal = creaTermList1(creaTerm2("fail",0), e->goal);
   resolve(e->goal, e->db);
   //time = System.currentTimeMillis();
   return run1(e, b);
	// run();
}		

Engine  *eng;
char *programa;
//extern NodoL **vardict;
   
long get_file_size(char *filename) {
    struct stat file_status;
    if (stat(filename, &file_status) < 0) {
        return -1;
    }
    return file_status.st_size;
}
   
void runM(char *prog, char *query) {
     Term *t; 
     NodoL **vardict;
     creaHashTab(&vardict, 1000); 
		  t = creaTerm3(
		creaParseString(query));
		printf("Term query= <<%s>>" , toStringTerm(t));
		//exit(0);
		
		if (t != NULL) { // parse query succeeded
			eng = creaEngine(t,
			/*ParseString.*/consult(
					prog,	
					vardict/*,   
					 programa*/
					)
				);
                        //results.append(eng.run(true)+"\n");
                        //System.out.println("goal "+eng.run(true));
        printf("runM Goal e->goal->numcla=%d iiih=(%d)\n",  
            eng->goal->numclauses, iiih);
            //exit(0);
             puts("de vuelta de creaEngine consult");
            printf("goal %s \n", run1(eng, true));
            //exit(0);
            //printf("goal more %s \n", more0(eng, false));
            //printf("goal more %s \n", more0(eng, false));    
		}
}
   
void main (){ 
      /*char *programa=("hijo(adan, abel).\
                      hijo(adan, cain).\
                      hijo(dios, adan).\
                      hijo(dios, jesus).\
                      hermano(X,G):-hijo(H,X), hijo(H,G).");*/
   char *file="./deriva000.pro";
   char *query="deriv(ln(sin(exp(suma(x,x)))),x,M).";
   //char *query="deriv(ln(cos(x)),x,M).";
   //char *query="deriv(sin(x),x,M).";
   //char *query="simpli(mul(exp(cos(ln(sin(suma(x,x))))), uno), D).";
   //char *query="simpli(div(sin(x),sin(x)),M).";
   //char *query="simpli(ln(uno),M).";
      
         FILE *fis=fopen(file,"rb");
         if ( fis == NULL){
             error("Load","Can't open: ");
             exit(1);
         }
	     int fsize = get_file_size(file)+1;
	     char *b=(char*)malloc(fsize+10);
		//byte b[] = new byte[fis.available()];
		
		fread(b,fsize,1, fis);
		b[fsize-1]=0;
		printf("fsize=<%d>", fsize);
		printf("<%s>", b);
		

		fclose(fis);
		//exit(0);
              
                //-------creaHashTab(&vardict, 1000);  
		//runM(b, "rela(M, bob)"
		runM(b, query
		/*args[1]*/
		);
		
   }


