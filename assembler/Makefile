CC=gcc
CFLAGS=-Wall -O2 -g -I../common/

OS := $(shell uname)
ifeq ($(OS), Darwin)
LIBS=-ll
else
LIBS=-lfl
endif

LDFLAGS=
BISON=bison
BFLAGS=-d
LEX=flex
LFLAGS=

vnsasm: vnsasm.c vnsasm.h scanner.c parser.tab.c \
		../common/utils.c ../common/utils.h ../common/globals.h \
		../common/instructionset.c ../common/instructionset.h \
		../common/list.c ../common/list.h
	$(CC) -o $@ $(filter %c, $^) $(CFLAGS) $(LIBS)

scanner.c: scanner.l parser.tab.h
	$(LEX) $(LFLAGS) -o $@ $<

parser.tab.h: parser.y
	$(BISON) $(BFLAGS) $<

parser.tab.c: parser.tab.h

clean:
	@rm -f *.o scanner.c parser.tab.* parser.dot vnsasm
