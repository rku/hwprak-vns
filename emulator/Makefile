CC=gcc
CFLAGS=-Wall -O2 -I ../common/
LDFLAGS=-lreadline -lm

vnsem: vnsem.c vnsem.h console.c console.h \
	../common/utils.c ../common/utils.h \
	../common/instructionset.c ../common/instructionset.h
	$(CC) -o $@ $(filter %c, $^) $(CFLAGS) $(LDFLAGS)

clean:
	@rm -f vnsem *.o
