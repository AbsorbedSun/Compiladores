YACC_CMD ?= $(shell command -v yacc 2>/dev/null || command -v byacc 2>/dev/null || echo ../../../../../herramientas/yacc/oyacc)
calcNumG: y.tab.c
	gcc -o calcNumG y.tab.c
y.tab.c: hoc1.y
	$(YACC_CMD) -d hoc1.y
clean:
	rm -f calcNumG y.tab.c y.tab.h
.PHONY: clean
