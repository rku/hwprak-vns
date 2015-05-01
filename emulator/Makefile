CC=gcc
CFLAGS=-Wall -O2 -I ../common/
LDFLAGS=-lreadline -lm

vns-emu: vnsem.c vnsem.h console.c console.h \
	../common/utils.c ../common/utils.h
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	@rm -f vns-emu *.o
