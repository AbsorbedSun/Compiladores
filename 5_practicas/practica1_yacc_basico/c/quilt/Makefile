Gram=y.tab.c y.tab.h

all: $(Gram)
	@gcc -o quil y.tab.c quilt.c
	@echo Compiled

$(Gram): quilt.y
	@yacc -d quilt.y

clean:
	@rm -f  *.tab.* quil
	@echo Clean
