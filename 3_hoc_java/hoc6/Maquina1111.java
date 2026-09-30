import java.awt.*;
import java.util.*;
import java.lang.reflect.*;

class  Maquina {
//ver como automatizar esto

public final static short NUMBER=257;
public final static short PRINT=258;
public final static short VAR=259;
public final static short BLTIN=260;
public final static short INDEF=261;
public final static short WHILE=262;
public final static short IF=263;
public final static short ELSE=264;
public final static short FUNCTION=265;
public final static short PROCEDURE=266;
public final static short RETURN=267;
public final static short FUNC=268;
public final static short PROC=269;
public final static short ARG=270;
public final static short OR=271;
public final static short AND=272;
public final static short GT=273;
public final static short GE=274;
public final static short LT=275;
public final static short LE=276;
public final static short EQ=277;
public final static short NE=278;
public final static short UNARYMINUS=279;
public final static short NOT=280;
public final static short YYERRCODE=256;


//hasta aqui
Stack pila;
Stack marcos;
Vector prog=new Vector();

static int pc=0;
int progp;
int progbase=0;
boolean returning=false;

Method metodo;
Class c;
   public Vector getProg(){ return prog; }
   public int getProgP(){ return progp; }
   public int getProgBase(){ return progbase; }
   public int getProgSize(){ return prog.size(); }
   Maquina(){ }
   void initcode(){
      Vector tmp;
      progp = progbase;
      pila=new Stack();
      marcos=new Stack();
      tmp=(Vector)prog.clone();
      prog=new Vector();
      for( int i = 0; i < progbase; i++)
        prog.addElement(tmp.elementAt(i));
      returning = false;
   }
   Object pop(){ return pila.pop(); }
   int code(Object f){
      System.out.println("Gen ("+f+") size="+prog.size());
      prog.addElement(f);
      progp = prog.size();
      return prog.size()-1;
   }
   void execute(int p){
      String inst;
      System.out.println("progsize="+prog.size());
      for(pc=0;pc < prog.size(); pc=pc+1){
	System.out.println("pc="+pc+" inst "+prog.elementAt(pc));
      }
      for(pc = p; !(inst=(String)prog.elementAt(pc)).equals("STOP") &&
                    !returning; ){
		//for(pc=p;pc < prog.size();){
			try {
			//System.out.println("111 pc= "+pc);
			inst=(String)prog.elementAt(pc);
			pc=pc+1;
			System.out.println("3333 pc= "+pc+" instr "+inst);
                        c=this.getClass();
			//System.out.println("clase "+c.getName());
          		metodo=c.getDeclaredMethod(inst, null);
			metodo.invoke(this, null);
			}
			catch(NoSuchMethodException e){
				System.out.println("No metodo "+e);
                        }

			catch(InvocationTargetException e){
System.out.println(" inst = "+inst+"  "+e+"this "+this+" pc = [ "+pc+" ]");
				//System.out.println(e);
                        }
			catch(IllegalAccessException e){
				System.out.println(e);
                        }
		}
           //progbase=pc+1; funionaba pero es gral?
	}
   void constpush(){
        Object s;  
	s=prog.elementAt(pc);
    System.out.println("en constpush pc= "+pc+" se push ="+s);
	pc=pc+1;
	pila.push(s);
   }
   void varpush(){
	Simbolo s;
        s=(Simbolo)prog.elementAt(pc);
	pc=pc+1;
	pila.push(s);
   }
   void whilecode(){
      boolean d;
      int savepc = pc;
      execute(savepc+2);	/* condition */
      d=((Boolean)pila.pop()).booleanValue();     
      while (d) {
	 execute(
            ((Integer)prog.elementAt(savepc)).intValue());
	 execute(savepc+2);
	 d=((Boolean)pila.pop()).booleanValue();	
      }
      if (!returning)
         pc=((Integer)prog.elementAt(savepc+1)).intValue();
   }
   void ifcode(){
      boolean d;
      //double d;
      int savepc=pc;
      //System.out.println("en if code pc= "+pc);
      execute(savepc+3);
      //System.out.println("en ant pop ifcode  pc= "+pc);
      //d=((Double)pila.pop()).doubleValue();
      d=((Boolean)pila.pop()).booleanValue();
      //System.out.println("en ifcode  pc= "+pc+" val ="+d);
      if(d){
         //System.out.println("en el if");
         execute(((Integer)prog.elementAt(savepc)).intValue());
      }
      else if(!prog.get(savepc+1).toString().equals("STOP"))
         execute(((Integer)prog.elementAt(savepc+1)).intValue());
      if (!returning)
         pc=((Integer)prog.elementAt(savepc+2)).intValue();
   }
   void define(Simbolo s){
      s.defn=progbase;
      progbase=prog.size();
   }
   void call(){
      Simbolo s;
      Marco m;
      System.out.println("en (call) "); 
      System.out.println("11en (call) dir ret ("+(pc+2)+")"+
    " tope = "+(pila.size()-1)+"progbase = "+progbase+" pc = "+pc);
     //+"pila val"+pila.top()
    //+"val top ="+pila.elementAt(pila.size()-1));       
      s=(Simbolo)prog.elementAt(pc);
      System.out.println("22en (call) "+s.nombre+" dir ret ("+(pc+2)+")"+
    " tope = "+(pila.size()-1)+"progbase = "+progbase+" pc = "+pc);
     //+"pila val"+pila.top()
    //+"val top ="+pila.elementAt(pila.size()-1));
      m=new Marco(s, pc+2, pila.size()-1, 
	          Integer.parseInt((String)prog.elementAt(pc+1)));
      marcos.push(m);
      execute(s.defn);
      returning = false;
   }
   void ret(){
      Marco m=(Marco)marcos.peek();
      System.out.println(m.s.nombre+" (ret) "+m.nargs+ "  ]"+m.retpc);
      for(int i=0 ; i< m.nargs; i++)
         pila.pop();
      pc=m.retpc;     
      marcos.pop();
      returning = true;
   }	
   void funcret(){
      Object o;
      Marco m=(Marco)marcos.peek();
      if(m.s.tipo==PROCEDURE)
         System.out.println(m.s.nombre+" (proc) regresa valor");
      o=pila.pop();
      ret();
      pila.push(o);
   }
   void procret(){
      Marco m=(Marco)marcos.peek();
      if(m.s.tipo == FUNCTION)
	 System.out.println(m.s.nombre+" (func) regresa valor");
      ret();
   }
   int getarg(){
      Marco m=(Marco)marcos.peek();
      //System.out.println("*****GETARG pc="+pc+" inst "+prog.elementAt(pc));
      int nargs =Integer.parseInt((String)prog.elementAt(pc));
      pc=pc+1;
      if(nargs > m.nargs)
         System.out.println(m.s.nombre+" argumentos insuficientes");
      return m.argn+nargs-m.nargs;
   }
   void arg(){
      Object o;
     //System.out.println("ARG "+d+" pc ="+pc);
      o=pila.elementAt(getarg()); 
      pila.push(o);
   }
   void argassign()  {
      Double doble;
      double d;
      doble = (Double)pila.pop();
      d=doble.doubleValue();
      //pila.push(d);       
      pila.push(doble); /* dejar valor en la pila */
      pila.setElementAt( doble, getarg());
      //*getarg() = d; 
   }
   void add(){
        Polynomial d1, d2;
        d2=(Polynomial)pila.pop();
        d1=(Polynomial)pila.pop();
        pila.push(d1.add(d2));
   }
   void sub(){
        Polynomial d1, d2;
        d2=(Polynomial)pila.pop();
        d1=(Polynomial)pila.pop();
        pila.push(d1.subtract(d2));
   }
   void mul(){
        Polynomial d1, d2;
        d2=(Polynomial)pila.pop();
        d1=(Polynomial)pila.pop();
        System.out.println("en MUL  pc= "+pc+" res ="+d1.multiply(d2)+" d1 ="+d1+" d2="+d2);
        pila.push(d1.multiply(d2));
   }
   void div(){
	Polynomial d1, d2;
        d2=(Polynomial)pila.pop();
        d1=(Polynomial)pila.pop();
	pila.push(d1.divide(d2));
   }
   /*void power(){
        double d1, d2;
        d2=((Double)pila.pop()).doubleValue();
        d1=((Double)pila.pop()).doubleValue();
	d1= Math.pow(d1, d2);
        pila.push(new Double(d1));
   }
   void negate(){
	double d;
	d=((Double)pila.pop()).doubleValue();
	d=-d;
	pila.push(new Double(d));
   }*/
   void eq(){
      boolean res;	
      Polynomial d1, d2;
        d2=(Polynomial)pila.pop();
        d1=(Polynomial)pila.pop();        
      res= d1.eqPolinomio(d2);
      System.out.println("eq: ("+res+" )");
      pila.push(new Boolean(res));
   }
   void ne(){
      boolean res;
      Polynomial d1, d2;
        d2=(Polynomial)pila.pop();
        d1=(Polynomial)pila.pop();	        
      res= d1.nePolinomio(d2);
      pila.push(new Boolean(res));
   }
   void lt(){
      boolean res;
      Polynomial d1, d2;
        d2=(Polynomial)pila.pop();
        d1=(Polynomial)pila.pop(); 
          
      res= d1.ltPolinomio(d2);
       System.out.println("en LT  pc= "+pc+" res ="+res+" d1 ="+d1+" d2="+d2);
      pila.push(new Boolean(res));
   }
   void le(){
      boolean res;
      Polynomial d1, d2;
        d2=(Polynomial)pila.pop();
        d1=(Polynomial)pila.pop();     
      res= d1.lePolinomio(d2);
       
      pila.push(new Boolean(res));
   }
   void gt(){
      boolean res;
      Polynomial d1, d2;
        d2=(Polynomial)pila.pop();
        d1=(Polynomial)pila.pop();	
      res= d1.gtPolinomio(d2);
      pila.push(new Boolean(res));
   }
   void ge(){
      boolean res;
      Polynomial d1, d2;
        d2=(Polynomial)pila.pop();
        d1=(Polynomial)pila.pop();
      res= d1.gePolinomio(d2);
      pila.push(new Boolean(res));
   }
   void or(){
      boolean d1, d2;
      d2=((Boolean)pila.pop()).booleanValue();
      d1=((Boolean)pila.pop()).booleanValue();
      d1=(d1 || d2);
      pila.push(new Boolean(d1));
   }
   void and(){
      boolean d1, d2;
      d2=((Boolean)pila.pop()).booleanValue();
      d1=((Boolean)pila.pop()).booleanValue();
      d1=(d1 && d2);
      pila.push(new Boolean(d1));
   }
   void not(){
      boolean d;
      d=((Boolean)pila.pop()).booleanValue();
      d=!d;
      pila.push(new Boolean(d));
   }
   void eval(){
      Simbolo s;
      s=(Simbolo)pila.pop();
      if(s.tipo == INDEF)
	 System.out.println("variable no definida "+ s.nombre);
      pila.push(s.obj);
   }
   void assign(){
	Simbolo s;
        Object d2;
	s=(Simbolo)pila.pop();
	d2=pila.pop();
	if(s.tipo != VAR && s.tipo != INDEF)
	   System.out.println("asignacion  a una no variable "+s.nombre);
	s.obj = d2;
	s.tipo= VAR;
	pila.push(d2);
   }
   void print(){
        Polynomial d;
	d=(Polynomial)pila.pop();
	System.out.println(""+d);
   }
   void prexpr(){
	Polynomial d;
	d=(Polynomial)pila.pop();
	System.out.print("["+d+"]");
   }
   Object bltinGen(Class c, String inst, Object parsrecopi[]){
	Object ret;
        Method metodo=null;
        Class paramTypes[];
        Object params[];

	try {
           Method[] ma = c.getMethods();
           for (Method m : ma) {
            
            if(m.getName().equals(inst)){
                metodo=m;
            }  
           }
           paramTypes = metodo.getParameterTypes();
           params = new Object[paramTypes.length];
           for (int p = 0; p < paramTypes.length; p++){
	      Object val = null;
              params[p] = parsrecopi[p];
           }
	       ret=metodo.invoke( parsrecopi[0], params);
           return ret;
	}
	catch(InvocationTargetException e){
		System.out.println(e);
        }
	catch(IllegalAccessException e){
		System.out.println(e);
        }
        return null;
   }
   void bltin(){
      Polynomial d; //prob habia Double en lugar de Polynomial en ret
      String inst;
      Class c;
      Polynomial ret;
      int npar=0;
    
      inst=((Simbolo)prog.elementAt(pc)).nombre;
      npar=Integer.parseInt((String)prog.elementAt(pc+1));
      Object param[]=new Object[npar];
      for(int i=0; i< npar; i++){
         param[npar-1-i]=(Polynomial)pila.pop();
      }
      c=Polynomial.class;
      ret=(Polynomial)bltinGen(c, inst, param);
      pila.push(ret);
      pc=pc+2;
   }
}
