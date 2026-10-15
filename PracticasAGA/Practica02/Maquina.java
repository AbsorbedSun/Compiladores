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
      Color colors[]={Color.red,Color.green,Color.blue,
                      Color.black,Color.orange,Color.gray};
      double d1;
      d1=((Double)pila.pop()).doubleValue();
      if(g!=null){
         g.setColor(colors[(int)d1]);
      }
   }
   /* line x1 y1 x2 y2: linea entre dos puntos (coordenadas absolutas) */
   public void line(){
      int y2=(int)((Double)pila.pop()).doubleValue();
      int x2=(int)((Double)pila.pop()).doubleValue();
      int y1=(int)((Double)pila.pop()).doubleValue();
      int x1=(int)((Double)pila.pop()).doubleValue();
      traza(x1,y1,x2,y2);
      x=x2;  y=y2;           // la posicion actual queda en el extremo final
   }
   /* circulo x y d: circulo de diametro d con la esquina de su caja en (x,y) */
   public void circulo(){
      int d=(int)((Double)pila.pop()).doubleValue();
      int cy=(int)((Double)pila.pop()).doubleValue();
      int cx=(int)((Double)pila.pop()).doubleValue();
      circ(cx,cy,d);
      x=cx;  y=cy;
   }
   /* rectangulo x y ancho alto: esquina superior izquierda en (x,y) */
   public void rectangulo(){
      int h=(int)((Double)pila.pop()).doubleValue();
      int w=(int)((Double)pila.pop()).doubleValue();
      int ry=(int)((Double)pila.pop()).doubleValue();
      int rx=(int)((Double)pila.pop()).doubleValue();
      rect(rx,ry,w,h);
      x=rx;  y=ry;
   }

   /* ---- auxiliares (coordenadas del usuario: origen en (150,150), Y hacia arriba) ---- */
   private void traza(int x1,int y1,int x2,int y2){
      if(g!=null) (new Linea(x1+150,150-y1, x2+150,150-y2)).dibuja(g);
   }
   private void rect(int rx,int ry,int w,int h){
      if(g!=null) (new Rectangulo(rx+150,150-ry, w,h)).dibuja(g);
   }
   private void circ(int cx,int cy,int d){
      if(g!=null) (new Circulo(cx+150,150-cy, d)).dibuja(g);
   }

   /* ---- figuras compuestas (el suelo esta en y=-200) ---- */
   public void casa(){
      if(g==null) return;
      Color previo=g.getColor();
      g.setColor(Color.black);                       // techo triangular
      traza(-140,-100, -55,-15);
      traza(-55,-15,   30,-100);
      traza(30,-100, -140,-100);
      g.setColor(Color.red);     rect(-130,-100,150,100);   // paredes
      g.setColor(Color.orange);  rect(-75,-140,40,60);      // puerta
      g.setColor(Color.blue);    rect(-120,-115,30,30);     // ventana
      g.setColor(previo);
   }
   public void persona(){
      if(g==null) return;
      Color previo=g.getColor();
      g.setColor(Color.black);
      circ(68,-100,24);                              // cabeza
      traza(80,-124, 80,-170);                       // cuerpo
      traza(80,-170, 65,-200);                       // pierna izquierda
      traza(80,-170, 95,-200);                       // pierna derecha
      traza(80,-135, 55,-150);                       // brazo izquierdo
      traza(80,-135, 105,-150);                      // brazo derecho
      g.setColor(previo);
   }
   public void auto(){
      if(g==null) return;
      Color previo=g.getColor();
      g.setColor(Color.green);
      rect(140,-150,200,35);                         // carroceria
      rect(180,-120,100,30);                         // cabina
      g.setColor(Color.gray);
      circ(160,-170,30);                             // rueda trasera
      circ(290,-170,30);                             // rueda delantera
      g.setColor(Color.orange);
      circ(326,-158,8);                              // faro
      g.setColor(previo);
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
      d=(Double)pila.pop();
      System.out.println(""+d.doubleValue());
   }
   public void prexpr(){
      Double d;
      d=(Double)pila.pop();
      System.out.print("["+d.doubleValue()+"]");
   }
}
