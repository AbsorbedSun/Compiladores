# Compila oyacc (yacc portable de OpenBSD) para usarlo si el sistema no tiene yacc/bison.
# Uso:  make        -> genera ./oyacc
oyacc: *.c *.h
	$(CC) -w -D_GNU_SOURCE -D__unused= -o oyacc *.c
clean:
	rm -f oyacc
