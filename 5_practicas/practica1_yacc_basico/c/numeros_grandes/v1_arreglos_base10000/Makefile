Gram=y.tab.c y.tab.h

all: $(Gram) 
	@gcc -o big y.tab.c big_calc.c
	@echo Compiled

$(Gram): big_calc.y
	@yacc -d big_calc.y

clean:
	@rm -f *.tab.* big
	@echo Clean
