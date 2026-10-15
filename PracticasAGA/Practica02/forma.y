%{
import java.util.*;
import java.lang.Math;
import java.io.*;
import java.util.StringTokenizer;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.lang.reflect.*;
%}
%token NUMBER LINE CIRCULO RECTANGULO COLOR PRINT AUTO CASA PERSONA
%start list
%%
list :
     | list ';'
     | list inst ';'   { 
             maq.code("print"); maq.code("STOP"); if (true) return 1; ;
     }   
     ;
inst:  NUMBER  { ((Algo)$$.obj).inst=maq.code("constpush");
                maq.code(((Algo)$1.obj).simb); }
      | RECTANGULO NUMBER NUMBER NUMBER NUMBER { 
                maq.code("constpush"); maq.code(((Algo)$2.obj).simb);
                maq.code("constpush"); maq.code(((Algo)$3.obj).simb);
                maq.code("constpush"); maq.code(((Algo)$4.obj).simb);
                maq.code("constpush"); maq.code(((Algo)$5.obj).simb);
                maq.code("rectangulo"); }
      | LINE NUMBER NUMBER NUMBER NUMBER { 
                maq.code("constpush"); maq.code(((Algo)$2.obj).simb);
                maq.code("constpush"); maq.code(((Algo)$3.obj).simb);
                maq.code("constpush"); maq.code(((Algo)$4.obj).simb);
                maq.code("constpush"); maq.code(((Algo)$5.obj).simb);
                maq.code("line"); }
      | CIRCULO NUMBER NUMBER NUMBER { 
                maq.code("constpush"); maq.code(((Algo)$2.obj).simb);
                maq.code("constpush"); maq.code(((Algo)$3.obj).simb);
                maq.code("constpush"); maq.code(((Algo)$4.obj).simb);
                maq.code("circulo"); }
      | CASA     { maq.code("casa"); }
      | AUTO     { maq.code("auto"); }
      | PERSONA  { maq.code("persona"); }
      | COLOR NUMBER { maq.code("constpush");
                maq.code(((Algo)$2.obj).simb); maq.code("color");}
      ;
%%
class Algo {
	Simbolo simb;
	int inst;
        Dibujable dibu;
	public Algo(int i){ inst=i; }
        public Algo(Dibujable d){ dibu=d; }
	public Algo(Simbolo s, int i){
		simb=s; inst=i;
	}
}
public void setTokenizer(StringTokenizer st){
 	this.st= st;
}
public void setNewline(boolean newline){
 	this.newline= newline;
}
HashMap<String, Simbolo> tabla=new HashMap<String, Simbolo>();
Maquina maq;

StringTokenizer st;
boolean newline;
int yylex(){
String s;
int tok;
Double d;
Simbolo simbo;
   if (!st.hasMoreTokens())
      if (!newline) {
         newline=true; 
	 return ';';  
      }
   else
      return 0;
   s = st.nextToken();
   try {
      d = Double.valueOf(s);
      simbo=new Simbolo(NUMBER, d.doubleValue());
      yylval = new ParserVal(new Algo(simbo, 0) );
      tok = NUMBER;
   } catch (Exception e){
   if(Character.isLetter(s.charAt(0))){
	if((simbo=tabla.get(s))!=null){
           yylval = new ParserVal(new Algo(simbo, 0));  
        }	
         tok= simbo.tipo;
    } else {
    	tok = s.charAt(0);
    }
   }
   return tok;
}
void yyerror(String s){
   System.out.println("parser error: "+s);
}
static Parser par = new Parser(0);
static JFrame jf;
static JLabel lmuestra=new JLabel("                                 ");
static Canvas canv;
static Graphics g;
Parser(int foo){
   maq=new Maquina();
 try {
   Method[] ma = ((Maquina)maq).getClass().getMethods();
   for (Method m : ma) {  
            Class[] parameters = m.getParameterTypes();
            if( m.getReturnType().equals(Void.TYPE) && 
                m.getParameterTypes().length==0){
                System.out.println( m.getName()  + " returns void ");
            }  
   } 
  } catch (Exception e) {
       e.printStackTrace();
  }
   tabla.put("line", new Simbolo(LINE, 0.0));
   tabla.put("circulo", new Simbolo(CIRCULO, 0.0));
   tabla.put("rectangulo", new Simbolo(RECTANGULO, 0.0));
   tabla.put("color", new Simbolo(COLOR, 0.0));
   tabla.put("print", new Simbolo( PRINT, 0.0));
   tabla.put("casa", new Simbolo(CASA, 0.0));
   tabla.put("auto", new Simbolo(AUTO, 0.0));     
   tabla.put("persona", new Simbolo(PERSONA, 0.0));
   maq.setTabla(tabla);
   jf=new JFrame("Calcula");
   canv=new Canvas();
   canv.setSize(600,600);
   jf.add("North", new PanelEjecuta(maq, this));
   jf.add("Center", canv);
   jf.setSize( 600, 700);
   jf.setVisible(true);
   g=canv.getGraphics();
   maq.setGraphics(g);
   jf.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);  
}
public static void main(String args[]){ new Parser(); }
