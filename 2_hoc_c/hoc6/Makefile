# make        -> compila ./hoc      make clean -> limpia
# Usa yacc/byacc del sistema; si no hay, usa el oyacc de ../../herramientas/yacc (hacer make alla primero)
YACC_CMD ?= $(shell command -v yacc 2>/dev/null || command -v byacc 2>/dev/null || echo ../../herramientas/yacc/oyacc)

hoc: y.tab.c symbol.c code.c init.c math.c
	gcc -o hoc y.tab.c symbol.c code.c init.c math.c -lm

y.tab.c y.tab.h: hoc6.y
	$(YACC_CMD) -d hoc6.y

clean:
	rm -f hoc y.tab.c y.tab.h *.o
.PHONY: clean
