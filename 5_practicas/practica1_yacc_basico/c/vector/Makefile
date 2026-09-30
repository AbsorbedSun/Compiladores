Gram=y.tab.c y.tab.h

all: $(Gram) 
	@gcc -o vect y.tab.c vector_calc.c
	@echo Compiled

$(Gram): vector_calc.y
	@yacc -d vector_calc.y
clean:
	@rm -f *.tab.* vect
	@echo Clean
