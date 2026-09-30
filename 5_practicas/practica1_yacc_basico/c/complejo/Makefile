Gram=y.tab.c y.tab.h

all: $(Gram) 
	@gcc -o comple y.tab.c lex.yy.c complejo_calc.c
	@echo Compiled

$(Gram): complejo_calc.y
	@yacc -d complejo_calc.y

lex.yy.c: complejo_calc.l
	@flex complejo_calc.l

clean:
	@rm -f lex.yy.c *.tab.* comple
	@echo Clean
