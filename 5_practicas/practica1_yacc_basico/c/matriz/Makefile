Gram=y.tab.c y.tab.h

all: $(Gram) 
	@gcc -o matriz y.tab.c matriz_calc.c
	@echo Compiled

$(Gram): matriz_calc.y
	@yacc -d matriz_calc.y

clean:
	@rm -f *.tab.* matriz
	@echo Clean
