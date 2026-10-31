# make        -> compila ./calc      make clean -> limpia
# Usa yacc/byacc/bison -y del sistema
YACC_CMD ?= $(shell command -v yacc 2>/dev/null || command -v byacc 2>/dev/null || echo ../../herramientas/yacc/oyacc)

calc: y.tab.c code.c symbol.c init.c math.c hoc.h
	gcc -Wall -o calc y.tab.c code.c symbol.c init.c math.c

y.tab.c y.tab.h: hoc5.y hoc.h
	$(YACC_CMD) -d hoc5.y

clean:
	rm -f calc y.tab.c y.tab.h *.o
.PHONY: clean
