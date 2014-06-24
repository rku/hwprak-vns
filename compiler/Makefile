CC=gcc
CFLAGS=-Wall -O2 -g -I../include/
LIBS=-lfl
LDFLAGS=
BISON=bison
BFLAGS=-d
LEX=flex
LFLAGS=

vnsasm: vnsasm.c vnsasm.h lexer.c vnsasm.tab.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

lexer.c: vnsasm.l vnsasm.tab.h
	$(LEX) -o $@ $< $(FLEXARGS)

vnsasm.tab.h: vnsasm.y
	$(BISON) $(BFLAGS) $<

vnsasm.tab.c: vnsasm.tab.h

clean:
	@rm -f *.o lexer.c vnsasm.tab.c vnsasm.tab.h vnsasm.dot vnsasm
