CC=gcc
CFLAGS=-Wall -O2 -g -I../common/
LIBS=-lfl
LDFLAGS=
BISON=bison
BFLAGS=-d
LEX=flex
LFLAGS=

vnsasm: vnsasm.c vnsasm.h lexer.c grammar.tab.c \
		../common/utils.c ../common/instructionset.c \
		../common/utils.h ../common/globals.h ../common/instructionset.h
	$(CC) -o $@ $(filter %c, $^) $(CFLAGS) $(LIBS)

lexer.c: lexer.l grammar.tab.h
	$(LEX) -o $@ $< $(FLEXARGS)

grammar.tab.h: grammar.y
	$(BISON) $(BFLAGS) $<

grammar.tab.c: grammar.tab.h

clean:
	@rm -f *.o lexer.c grammar.tab.* grammar.dot vnsasm
