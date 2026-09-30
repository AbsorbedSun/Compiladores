Gram=y.tab.c y.tab.h

all: $(Gram) 
	@gcc -o poli y.tab.c lex.yy.c polinomio_calc.c
	@echo Compiled

$(Gram): polinomio_calc.y
	@yacc -d polinomio_calc.y

lex.yy.c: polinomio_calc.l
	@flex polinomio_calc.l

clean:
	@rm -f *.tab.* lex.yy.c poli
	@echo Clean
