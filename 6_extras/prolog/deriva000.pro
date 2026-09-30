deriv(x, x, uno).
deriv(sin(U), x, mul(cos(U), DU)):-deriv(U,x,DU).
deriv(cos(U), x, mul(menos(sin(U)), DU)):-deriv(U,x,DU).
deriv(exp(U),x, mul(exp(U), DU)):-deriv(U, x, DU).
deriv(ln(U),x, mul(div(uno,U), DU)):-deriv(U, x, DU).
deriv(suma(U, V), x, D ) :- deriv(U, x, DU), deriv(V, x, DV), simpli(suma(DU, DV), D).
deriv(mul(U, V), x, D) :- deriv(U, x, DU), deriv(V, x, DV), simpli(mul(V, DU), P1), simpli(mul(U, DV),P2), simpli(suma(P1, P2), D).
simpli(suma(U,cero), U).
simpli(suma(cero, U) ,U).
simpli(suma(uno, uno), dos).
simpli(mul(U,uno) , U).
simpli(mul(uno, U) , U).
simpli(mul(U,cero),cero).
simpli(mul(cero,U),cero).
simpli(div(U,U), uno).
simpli(suma(U, U) ,mul( dos, U)).
simpli(pot(U,cero), uno).
simpli(ln(uno), cero).
simpli(ln(pot(U,cte)), mul(cte, ln(U))).




       
       
      


