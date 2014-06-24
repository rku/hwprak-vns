CC=gcc
CFLAGS=-Wall -O2 -I ../include/
LDFLAGS=-lreadline

vnsem: vnsem.c vnsem.h
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	@rm -f vnsem *.o
