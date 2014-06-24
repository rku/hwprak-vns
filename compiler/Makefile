CC=gcc
CFLAGS=-Wall -O2 -g
LIBS=-lfl
LDFLAGS=
BISON=bison
BFLAGS=-d
LEX=flex
LFLAGS=

vnsasm: vnsasm.c lex.yy.c vns.tab.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

lex.yy.c: vns.l vns.tab.h
	$(LEX) -o $@ $< $(FLEXARGS)

vns.tab.h: vns.y
	$(BISON) $(BFLAGS) $<

vns.tab.c: vns.tab.h

clean:
	@rm -f *.o lex.yy.c vns.tab.c vns.tab.h vns.dot vnsasm
