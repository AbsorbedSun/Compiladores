import java.awt.*;
import java.awt.image.*;
import java.util.*;
import java.lang.reflect.*;

class  Maquina {
   Filtradora poste;
   HashMap<String, Simbolo> tabla;
   Stack pila;
   ArrayList prog;

   static int pc=0;
   int progbase=0;
   boolean returning=false;

   Method metodo;
   Method metodos[];
   Class c;
   Graphics g;
   double angulo;
   int x=0, y=0;
   Class parames[];
   Maquina(){ 
      poste= new Filtradora( ); 
   }
   public void setTabla(HashMap<String, Simbolo> t){ tabla = t; }
   public void setGraphics(Graphics g){ this.g=g; }
   Maquina(Graphics g){ this.g=g; }
   public ArrayList getProg(){ return prog; }
   void initcode(){
      pila=new Stack();
      prog=new ArrayList();
   }
   Object pop(){ return pila.pop(); }
   int code(Object f){
      System.out.println("Gen ("+f+") size="+prog.size());
      prog.add(f);
      return prog.size()-1;
   }
   void execute(int p){
      String inst;
      System.out.println("progsize="+prog.size());
      for(pc=0;pc < prog.size(); pc=pc+1){
         System.out.println("pc="+pc+" inst "+prog.get(pc));
      }
      for(pc=p; !(inst=(String)prog.get(pc)).equals("STOP") && !returning;){
	 //for(pc=p;pc < prog.size();){
         try {
	    //System.out.println("111 pc= "+pc);
	    inst=(String)prog.get(pc);
	    pc=pc+1;
	    System.out.println("222 pc= "+pc+" instr "+inst);
            c=this.getClass();
	    //System.out.println("clase "+c.getName());
            metodo=c.getDeclaredMethod(inst, null);
	    metodo.invoke(this, null);
	 } catch(NoSuchMethodException e){
				System.out.println("No metodo "+e);
         } catch(InvocationTargetException e){
				System.out.println(e);
         } catch(IllegalAccessException e){
				System.out.println(e);
         }
      }
   }
   public void constpush(){
      Simbolo s;
      Double d;
      s=(Simbolo)prog.get(pc);
      pc=pc+1;
      pila.push(new Double(s.val));
   }
   public void varpush(){
      Simbolo s;
      double d;
      //System.out.println("varpush ");
      s=(Simbolo)prog.get(pc);
      pc=pc+1;
      pila.push(s);
   }
   public void color(){
      Color colors[]={Color.red,Color.green,Color.blue,Color.black,
                      Color.orange,Color.gray,Color.magenta,Color.cyan};
      double d1;
      d1=((Double)pila.pop()).doubleValue();
      if(g!=null){
         g.setColor(colors[(int)d1]);
      }
   }
   public void line(){
      double d1;
      d1=((Double)pila.pop()).doubleValue();
      int nx=(int)Math.round(x+d1*Math.cos(angulo));
      int ny=(int)Math.round(y+d1*Math.sin(angulo));
      if(g!=null){
         (new Linea(x+150,150-y, nx+150, 150-ny)).dibuja(g);
      }
      x=nx;
      y=ny;
      System.out.println("x="+x+" y="+y+" d1="+d1);
   }
   public void circulo(){
      double d1;
      d1=((Double)pila.pop()).doubleValue();
      if(g!=null){
         (new Circulo(x+150, 150-y, (int)d1)).dibuja(g);
      }
   }
   public void rectangulo(){
      if(g!=null){
         (new Rectangulo(x+150, 150-y, 100, 200 )).dibuja(g);
      }
   }
   /* rectangulo ancho alto: esquina superior izquierda en la posicion actual */
   public void rectangulo2(){
      double h=((Double)pila.pop()).doubleValue();
      double w=((Double)pila.pop()).doubleValue();
      if(g!=null){
         (new Rectangulo(x+150, 150-y, (int)w, (int)h)).dibuja(g);
      }
   }
   /* angulo grados: direccion de las siguientes lineas (0 = derecha, 90 = arriba) */
   public void angulo(){
      double grados=((Double)pila.pop()).doubleValue();
      angulo=Math.toRadians(grados);
   }
   /* ir x y: mueve la posicion actual sin dibujar */
   public void ir(){
      double ny=((Double)pila.pop()).doubleValue();
      double nx=((Double)pila.pop()).doubleValue();
      x=(int)nx;
      y=(int)ny;
   }
   public void draw(){      
      Simbolo s;
      Dibujable dibu;       
      s=(Simbolo)pila.pop();
      System.out.println("draw ( "+s.nombre+" )");
      dibu=s.obtenDibu();
      if(g!=null && dibu != null){
         System.out.println("draw DIBUJAR");
         dibu.dibuja(g);
         pila.push(dibu);
      }
   }
   public void filtro(){
      Simbolo s;
      Dibujable dibu;
      Imagen ima;
      System.err.println("FILTRO filtro");         
      s=(Simbolo)pila.pop();
      System.out.println("draw ( "+s.nombre+" )");
      dibu=s.obtenDibu(); 
      if(dibu != null){    
         System.out.println("if filtro");
         ima=(Imagen)dibu;
         //poste.set_Escala_de_Grises((BufferedImage)ima.ima);
         float[] kernel = new float[9];
         for (int i = 0; i < 9; i++) {
            kernel[i] = (float)(((Double)pila.pop()).doubleValue());
         }
         System.out.println("desp for filtro");
         poste.ponFiltro(new Kernel(3, 3, kernel));
         poste.procesa("User", (BufferedImage)ima.ima);
         new Imagen(poste.getFoto()).dibuja(g) ;
      }
      /*poste.set_Escala_de_Grises(leeImagen("Imagen.jpg"));
      System.err.println("antes dibujar ");
      new Imagen(poste.getFoto(), 100, 100, con).dibujar(g);*/        
   }
   public void print(){
      Double d;
      if(pila.isEmpty() || !(pila.peek() instanceof Double)) return;  // nada que imprimir
      d=(Double)pila.pop();
      System.out.println(""+d.doubleValue());
   }
   public void prexpr(){
      Double d;
      d=(Double)pila.pop();
      System.out.print("["+d.doubleValue()+"]");
   }
}
