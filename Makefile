CC=gcc
CFLAGS=-Wall -O2
LIBS=
LDFLAGS=

all: vnsasm vnsem

vnsasm:
	@make -C compiler

vnsem:
	@make -C emulator

clean:
	@make -C compiler clean
	@make -C simulator clean
