# make        -> compila ./hoc3      make clean -> limpia
# Requiere: yacc o byacc, lex o flex (se necesita flex por la regla <<EOF>>) y gcc.
# Si no hay yacc en el sistema se usa el oyacc de ../../herramientas/yacc (hacer make alla primero)
YACC_CMD ?= $(shell command -v yacc 2>/dev/null || command -v byacc 2>/dev/null || echo ../../herramientas/yacc/oyacc)
LEX_CMD  ?= $(shell command -v lex 2>/dev/null || command -v flex 2>/dev/null)

TARGET = hoc3
OBJS   = y.tab.o lex.yy.o symbol.o init.o math.o

CC     = gcc
CFLAGS = -Wall -g

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

y.tab.c y.tab.h: hoc3.y hoc.h
	$(YACC_CMD) -d hoc3.y

lex.yy.c: lex.l y.tab.h hoc.h
	$(LEX_CMD) lex.l

$(OBJS): hoc.h
lex.yy.o init.o: y.tab.h

clean:
	rm -f $(TARGET) $(OBJS) y.tab.c y.tab.h lex.yy.c
.PHONY: clean
