# make        -> compila ./calc      make clean -> limpia
# Usa yacc/byacc del sistema; si no hay, usa el oyacc de ../../herramientas/yacc (hacer make alla primero)
YACC_CMD ?= $(shell command -v yacc 2>/dev/null || command -v byacc 2>/dev/null || echo ../../herramientas/yacc/oyacc)

calc: y.tab.c 
	gcc -o calc y.tab.c 

y.tab.c y.tab.h: hoc1.y
	$(YACC_CMD) -d hoc1.y

clean:
	rm -f calc y.tab.c y.tab.h *.o
.PHONY: clean
