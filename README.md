# hwprak-vns

This is a very minimalistic assembler/emulator for the Von-Neumann
computer used in the hardware practice course at the TU Dresden
university (TUD).

If you do not know what this is for, you probably do not need it.

In order to build the assembler and emulator, just type:

```
make
```

This software requires gnu bison/flex and libreadline to
be built.


## Example usage

  ```
  vnsasm -o multiply.bin examples/multiply.asm
  vnsem -s 10 multiply.bin
  ```

The emulator prints a status line for each executed instruction.
It looks like this (output taken from the multiply example program):

  ```
  #1026  [ ACCU=0xca  L=0xca  PC=0x17  SP=0x00 ]  C:*  Z:.  S:*
  ```

The fields have the following meanings (hexadezimal values are
prefixed with '0x'):

* #1026         current step count
* ACCU=0xca     the accu register is loaded with 0xca
* L=0xca        the L register is loaded with 0xca
* PC=0x17       the program counter is loaded with 0x17
* SP=0x00       the stack pointer is loaded with 0x00

**C**, **Z** and **S** show the state of the **C**arry, **Z**ero and
**S**ign flags respectively where '*' means 1 and '.' means 0.

## Notes on the assembler

The assembler is case insensitive.

* IN <port>

  Requests input for port <port> from user and stores it
  in the accumulator.

* OUT <port>

  Prints content of accumulator to stdout.

* .offset <offset>
  
  Sets address offset for the following code to <offset>.

  Example:

     ```
     JMP 0x10           ; jump to address 0x10
     .offset 0x10       ; set code offset to 0x10
     HLT                ; halt
     ```

* <label>:
  
  Declares a label. Note that labels are case sensitive.

  Example:

     ```
     JMP foo            ; jump to address labeled 'fooÄ
     foo2:              ; declare label 'foo2'
     RET                ; return
     foo: CALL foo2     ; declare label 'foo' and jump to
                        ; address labeled 'foo2'
     HLT                ; halt
     ```

* .byte <value> [, <value2> [, ...]]

  Set byte at current position to <value>.

  Example:

     ```
     .offset 0x50
     .byte 4, 25, 42, 58
     ```
