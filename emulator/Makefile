CC=gcc
LIBS=
CFLAGS=-Wall -O2 -I ../include/
LDFLAGS=

vnsem: vnsem.c vnsem.h
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	@rm -f vnsem *.o
