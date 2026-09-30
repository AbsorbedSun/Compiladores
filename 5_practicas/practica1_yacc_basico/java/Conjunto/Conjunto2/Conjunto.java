

public class Conjunto {
   private charcontenedor[ ];
   private int tam;
   public Conjunto(){
       contenedor = new char[256];
       tam = 0;}
   public Conjunto(Conjunto S){
       contenedor = new char[256];
       for (int i = 0; i < S.length; i++)
          contenedor[i] = S.contenedor[i];
       tam = S.tam;
   }
   public int posicion (char c){
      int i = 0;
      while (i < tam && contenedor[i] != c)
         i++;
      return i == tam ? -1 : i;
   }
   public boolean contiene (char c){return posicion(c) >= 0;}
   public boolean vacio (){ return tam == 0;}
   public boolean lleno (){ return tam == 256;}
   public void insertar (char c){
      if (!contiene(c)){
          contenedor[tam] = c;
          tam++;
      }
   }
   public void eliminar(char c){
      int pos = posicion(c);
      if (pos >= 0){
         for (int i = pos+1; i<contenedor.length; i++)
            contenedor[i-1] = contenedor[i];
            tam--;
      }
   }
   Conjunto union (Conjunto s){
      Conunto res = new Conjunto(s);
      for (int i = 0; i < this.tam; i++)
         res.insertar(this.contenedor[i]);
      return res;
    }
    public Conjunto interseccion (Conjunto s){
    }
}

