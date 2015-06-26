CC=gcc
CFLAGS=-Wall -O2 -I../common/ -I../emulator/ -Wl,--unresolved-symbols=ignore-all
LDFLAGS=-L. -ltestobjs
AR=ar
STRIP=strip

all: libtestobjs.a emulator-tests

libtestobjs.a: vnsem.o
	$(AR) cq $@ vnsem.o

vnsem.o: ../emulator/vnsem.c
	$(CC) -c $< $(CFLAGS)
	$(STRIP) -N main $@

emulator-tests: emulator-tests.c unittest.h
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

run-tests: emulator-tests
	@echo '*** Running emulator tests ***'
	@./emulator-tests

clean:
	@rm -f *.o libtestobjs.a tests
