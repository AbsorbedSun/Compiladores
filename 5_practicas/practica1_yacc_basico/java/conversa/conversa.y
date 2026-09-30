/* gramar.y */
/*oracion -> SN SV 
   | who
SN -> SUSTANTIVO 
   | DET SUSTANTIVO 
   | SUSTANTIVO ADJETIVO 
   | DET SUSTANTIVO ADJETIVO
SV -> VERBO 
   | VERBO SN 
who-> QUIEN VERBO 
   | QUIEN VERBO SN 
where -> DONDE VERBO SUSTANTIVO 
   |  DONDE VERBO DET SUSTANTIVO
*/
%{
import java.io.*;
import java.util.*;
%}
%token SUSTANTIVO VERBO DET DONDE COMO QUIEN PREPOSICION ADJETIVO ADVERBIO
%%
entrada:    /* cadena vacia */
        | entrada linea
        ;
linea:    '\n'
        | oracion '\n'
        ;
oracion : SN SV { 
                  System.out.println($1.sval); 
		  System.out.println($2.sval);
          
                  sujetos.add($1.sval);
                  System.out.println("sujeto: "+$1.sval);
                  norac++;
                  System.out.println("num orac: ( "+norac+" )");
                 }
        | where
        | who
        | how
  ;
SN : SUSTANTIVO 
   | DET SUSTANTIVO { 
        $$.sval= $1.sval +" "+ $2.sval;
     }
   ;
SV : VERBO { acciones.add($1.sval);}
   ;
how: COMO VERBO SUSTANTIVO { 
   make_respuesta3($3.sval, $2.sval);
   System.out.println(respuesta);
   }
   ;
who: QUIEN VERBO { 
   make_respuesta1_1($2.sval);
   System.out.println(respuesta);
   }
   ;
where : DONDE VERBO SUSTANTIVO { 
   make_respuesta2($3.sval, $2.sval);
   System.out.println(respuesta);
   }
   ;
%%
//FILE* yyin;
HashMap<String, Integer> tabla;
int norac=0;
ArrayList<String> sujetos;
ArrayList<String> acciones;
ArrayList<String> lugares;
ArrayList<String> modos;
ArrayList<String> objsdirec;
String respuesta;
/*extern int yydebug;
int yyerror(char const *str){
    extern char *yytext;
    fprintf(stderr, "parser error near %s\n", yytext);
    return 0;
}*/
void yyerror(String s){
  System.out.println("parser error: "+s);
}

void initialize(){
   int i;
   sujetos= new ArrayList<String>() ;
   acciones = new ArrayList<String>();
   lugares = new ArrayList<String>();
   modos = new ArrayList<String>();
   objsdirec = new ArrayList<String>();
   //for(int i=0;i<us.size();i++){
   norac = 0;
   return;
}
void make_answer3(int prev_norac){
   respuesta = sujetos.get(prev_norac);
   respuesta = respuesta+" ";
   respuesta = respuesta+ acciones.get(prev_norac);
   respuesta = respuesta+" ";
   //get_verb_ing();
   respuesta = respuesta+ modos.get(prev_norac);
   return;
}
void make_respuesta3(String sujeto, String verbo){
   int i;
   //printf("norac=(%d) (%s) (%s)", norac, sujeto, verbo );
   //for(int i=0;i<us.size();i++){
   for (i=norac-1; i >= 0; i--) {
   //printf("i=(%d) (%s) (%s)", i, sujetos[i], acciones[i]);  
      if ( sujeto.equals(sujetos.get(i))  &&
           verbo.equals(acciones.get(i))  &&
          (modos.get(i).length()!= 0) ) {
   //printf("i=(%d) (%s) (%s)", i, sujetos[i], acciones[i]);        
         make_answer3(i);
         return;
      }
   }
   respuesta = "I don't know";
   return;
}
void make_answer2(int prev_norac){
   respuesta = sujetos.get(prev_norac);
   respuesta = respuesta+" ";
   respuesta = respuesta+ acciones.get(prev_norac);
   respuesta = respuesta+" ";
   //get_verb_ing();
   respuesta = respuesta+ lugares.get(prev_norac);
   return;
}
void make_respuesta2(String sujeto, String verbo){
   int i;
   /* Last input norac is a where-question      */
   //printf("norac=(%d) (%s) (%s)", norac, sujeto, verbo );
   for (i=norac-1; i >= 0; i--) {
   //printf("i=(%d) (%s) (%s)", i, sujetos[i], acciones[i]);  
      if ( sujeto.equals(sujetos.get(i))  &&
           verbo.equals(acciones.get(i))  &&
          (lugares.get(i).length()      != 0) ) {
   //printf("i=(%d) (%s) (%s)", i, sujetos[i], acciones[i]);        
         make_answer2(i);
         return;
      }
   }
   respuesta = "I don't know";
   return;
}
void make_respuesta1_2(String sujeto, String verbo){
   int i;
   
   //printf("norac=(%d) (%s) (%s)", norac, sujeto, verbo );
   for (i=norac-1; i >= 0; i--) {
   //printf("i=(%d) (%s) (%s)", i, objsdirec[i], acciones[i]);  
      if ( sujeto.equals(objsdirec.get(i)) &&
           verbo.equals(acciones.get(i)) ) {
   //printf("i=(%d) (%s) (%s)", i, objsdirec[i], acciones[i]);        
         respuesta = sujetos.get(i); 
         return;
      }
   }
   respuesta = "I don't know";
   return;
}
void make_respuesta1_1(String verbo){
   int i;
   /* Last input norac is a where-question      */
   //printf("norac=(%d) (%s) (%s)", norac, sujeto, verbo );
   System.out.println("verbo "+verbo+"acc "+acciones.get(0));
   for (i=norac-1; i >= 0; i--) {
   //printf("i=(%d) (%s) (%s)", i, sujetos[i], acciones[i]);  
      if (verbo.equals(acciones.get(i))) {
   //printf("i=(%d) (%s) (%s)", i, sujetos[i], acciones[i]);  
         respuesta = sujetos.get(i);      
         return;
      }
   }
   respuesta = "I don't know";
   return;
}
Parser(int foo){
   tabla=new HashMap<String, Integer>();
   //tabla.put("", );
   tabla.put("hombre", new Integer(SUSTANTIVO));
   tabla.put("mujer", new Integer(SUSTANTIVO));
   tabla.put("pedro", new Integer(SUSTANTIVO));
   tabla.put("juan", new Integer(SUSTANTIVO));
   tabla.put("jose", new Integer(SUSTANTIVO));
   tabla.put("eva", new Integer(SUSTANTIVO));
   tabla.put("maria", new Integer(SUSTANTIVO));
   tabla.put("perro", new Integer(SUSTANTIVO));
   tabla.put("gato", new Integer(SUSTANTIVO));
   tabla.put("pato", new Integer(SUSTANTIVO));
   tabla.put("pollo", new Integer(SUSTANTIVO));
   tabla.put("tienda", new Integer(SUSTANTIVO));                        
   tabla.put("fonda", new Integer(SUSTANTIVO));
   tabla.put("bosque", new Integer(SUSTANTIVO));
   tabla.put("restaurante", new Integer(SUSTANTIVO));
   tabla.put("cantina", new Integer(SUSTANTIVO));
   tabla.put("biblioteca", new Integer(SUSTANTIVO));
   tabla.put("rio", new Integer(SUSTANTIVO));
   tabla.put("lago", new Integer(SUSTANTIVO));
   tabla.put("plaza", new Integer(SUSTANTIVO));
   tabla.put("alcohol", new Integer(SUSTANTIVO));   
   tabla.put("jugo", new Integer(SUSTANTIVO));
   tabla.put("agua", new Integer(SUSTANTIVO));   
   tabla.put("refresco", new Integer(SUSTANTIVO)); 
   tabla.put("juega", new Integer(VERBO));
   tabla.put("camina", new Integer(VERBO));
   tabla.put("corre", new Integer(VERBO));
   tabla.put("nada", new Integer(VERBO));
   tabla.put("come", new Integer(VERBO));
   tabla.put("bebe", new Integer(VERBO));
   tabla.put("lee", new Integer(VERBO));
   tabla.put("escribe", new Integer(VERBO));
   tabla.put("estudia", new Integer(VERBO));
   tabla.put("compra", new Integer(VERBO));
   tabla.put("vende", new Integer(VERBO));
   tabla.put("como", new Integer(COMO));
   tabla.put("donde", new Integer(DONDE));
   tabla.put("quien", new Integer(QUIEN));
   tabla.put("el", new Integer(DET));
   tabla.put("ella", new Integer(DET));
   tabla.put("un", new Integer(DET));
   tabla.put("una", new Integer(DET));
   tabla.put("la", new Integer( DET));
   tabla.put("negro", new Integer(ADJETIVO));
   tabla.put("blanco", new Integer(ADJETIVO));
   tabla.put("grande", new Integer(ADJETIVO));
   tabla.put("lento", new Integer(ADJETIVO));
   tabla.put("chico", new Integer(ADJETIVO));
   tabla.put("joven", new Integer(ADJETIVO));
   tabla.put("viejo", new Integer(ADJETIVO));
   tabla.put("en", new Integer(PREPOSICION));
   tabla.put("a", new Integer(PREPOSICION));
   tabla.put("rapidamente", new Integer(ADVERBIO));
   tabla.put("alegremente", new Integer(ADVERBIO));
   tabla.put("lentamente", new Integer(ADVERBIO));
}

static String ins;
static StringTokenizer st;
static boolean newline;
int yylex()
{
String s;
int tok;
Double d;
Integer token;
  if (!st.hasMoreTokens())
    if (!newline){
       newline=true;
       return '\n'; //So we look like classic YACC example
    } else return 0;
   s = st.nextToken();
   if(Character.isLetter(s.charAt(0))){
	if(( token=tabla.get(s))==null) return 0;
        yylval = new ParserVal(s);
	tok= token.intValue();	
   } else {
    	tok = s.charAt(0);
  }
  return tok;
}
public static void main(String s[]){  
   Parser par = new Parser(0);
   BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
   par.initialize();
   while (true){
      System.out.print("expression:");
      try {  ins = in.readLine();
      } catch (Exception e){}
      st = new StringTokenizer(ins);
      newline=false;
      par.yyparse();
  }
}
