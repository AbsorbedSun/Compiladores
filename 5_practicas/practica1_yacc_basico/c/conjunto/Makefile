Gram=y.tab.c y.tab.h

all: $(Gram) 
	@gcc -o conj y.tab.c conjunto_calc.c
	@echo Compiled

$(Gram): conjunto_calc.y
	@yacc -d conjunto_calc.y

clean:
	@rm -f *.tab.* conj
	@echo Clean
