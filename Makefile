.PHONY: vnsasm vnsem all

all: vnsasm vnsem

vnsasm:
	@make -C assembler

vnsem:
	@make -C emulator

clean:
	@make -C assembler clean
	@make -C emulator clean
