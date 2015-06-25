.PHONY: vnsasm vnsem tests all

all: vnsasm vnsem

vnsasm:
	@make -C assembler

vnsem:
	@make -C emulator

tests:
	@make -C tests
	@make -C tests run-tests

clean:
	@make -C assembler clean
	@make -C emulator clean
	@make -C tests clean
