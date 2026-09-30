Gram=y.tab.c y.tab.h

all: $(Gram) 
	@gcc -o racio y.tab.c lex.yy.c racional_calc.c
	@echo Compiled

$(Gram): racional_calc.y
	@yacc -d racional_calc.y

lex.yy.c: racional_calc.l
	@flex racional_calc.l

clean:
	@rm -f lex.yy.c *.tab.* racio
	@echo Clean
