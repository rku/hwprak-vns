CC=gcc
CFLAGS=-Wall -O2 -g -I../common/
LIBS=-lfl
LDFLAGS=
BISON=bison
BFLAGS=-d
LEX=flex
LFLAGS=

vnsasm: vnsasm.c vnsasm.h scanner.c parser.tab.c \
		../common/utils.c ../common/instructionset.c \
		../common/utils.h ../common/globals.h \
		../common/instructionset.h
	$(CC) -o $@ $(filter %c, $^) $(CFLAGS) $(LIBS)

scanner.c: scanner.l parser.tab.h
	$(LEX) -o $@ $< $(FLEXARGS)

parser.tab.h: parser.y
	$(BISON) $(BFLAGS) $<

parser.tab.c: parser.tab.h

clean:
	@rm -f *.o scanner.c parser.tab.* parser.dot vnsasm
