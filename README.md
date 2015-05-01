# hwprak-vns

This is a very minimalistic assembler/emulator for the Von-Neumann
computer used in the hardware practice course at the TU Dresden
university (TUD).

If you do not know what this is for, you probably do not need it.

In order to build the assembler and emulator, just type:

```Shell
make
```

This software requires gnu bison/flex and libreadline to
be built.


## Example usage

  ```Shell
  ./vns-asm -o multiply.bin examples/multiply.asm
  ./vns-emu -s 10 multiply.bin
  ```

The emulator prints a status line for each executed instruction. It
represents the machine state *after* the instruction has been executed.
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

## Notes on the emulator

The emulator features an interactive console. You are dropped into it
if an error occours or by hitting `CTRL-C` while a program is running.
You may also start the emulator without a program file argument in order
to enter the interactive console mode. The console provides several
commands that may help to debug your programs and investigate the current
state of the machine and memory.

* `help [<command>]`

  Show the available commands or detailed information on a specified
  `<command>`.

* `break [<addr>|clear]`

  If no arguments are passed, the command will display the current
  breakpoint (if set). By passing an `<addr>`, the break point is set to
  that address and the emulator will drop you into the console as soon
  as the program counter arrives at that address. The breakpoint can
  be removed with the `clear` argument.

* `load <file> [<offset>]`

  Load the (compiled) program file `<file>` into memory. You may
  optionally specify an `<offset>`. The program is then placed at that
  position. This is useful if you want to place a program at a certain
  address.

* `machine`

  Display the current machine state. This includes the program counter,
  stack pointer, step counter, register contents and flag states.

* `memdump [<addr>]`

  Dump the current memory state. A row/line marker points to the
  position of the program counter. The memory contents and addresses
  are displayed as hexadecimal (base 16) values.

  You may optionally specify an `<addr>` of a single cell you want to
  get the value for.

* `memset <addr> <value>`

  Set the memory cell at address `<addr>` to `<value>`.

* `pcset <addr>`

  Set the program counter to `<addr>`.

* `reset pc|mem|all`

  Reset the program counter (pc), memory (mem) or both (all).

* `run`

  Start executation from the current position of the program counter.

* `step`

  Run the instruction the program counter is pointing at and stop.

* `quit`

  Quit the emulator.

The interactive console supports tab completion and a command history.

## Notes on the assembler

The assembler is case insensitive.

* `in <port>`

  Requests input for port <port> from user and stores it
  in the accumulator.

* `out <port>`

  Prints content of accumulator to stdout.

* `.offset <offset>`
  
  Sets address offset for the following code to <offset>.

  Example:

     ```Assembly
     jmp 0x10           ; jump to address 0x10
     .offset 0x10       ; set code offset to 0x10
     hlt                ; halt
     ```

* `<label>:`
  
  Declares a label. Note that labels are case sensitive.

  Example:

     ```Assembly
     jmp foo            ; jump to address labeled 'fooÄ
     foo2:              ; declare label 'foo2'
     ret                ; return
     foo: call foo2     ; declare label 'foo' and jump to
                        ; address labeled 'foo2'
     hlt                ; halt
     ```

* `.byte <value> [, <value2> [, ...]]`

  Set byte(s) at current position to <value>.

  Example:

     ```Assembly
     .offset 0x50
     .byte 4, 25, 42, 58
     ```
