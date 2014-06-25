/**
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>

#include "globals.h"
#include "utils.h"
#include "vnsem.h"

vnsem_configuration config;

void print_machine_state(vnsem_machine *machine)
{
    printf("#%.3i  ", machine->step_count);
    printf("R:[PC=0x%.2x L=0x%.2x SP=0x%.2x]  ",
            machine->pc,
            machine->reg_l,
            machine->sp);
    printf("A:[V=0x%.2x C=%c Z=%c S=%c]\n",
            machine->accu,
            (machine->flags & F_CARRY) ? '*' : '.',
            (machine->flags & F_ZERO)  ? '*' : '.',
            (machine->flags & F_SIGN)  ? '*' : '.');
}

void print_key(void)
{
    printf("\n #002     Step count\n");
    printf(" R:[...]  Registers: program counter (PC), low register (L)\n");
    printf("                     and stack pointer (SP)\n");
    printf(" A:[...]  Accumulator: value (V), carry (C), zero (Z) and\n");
    printf("                       sign (S) flags\n\n");
}

void dump_memory(vnsem_machine *machine)
{
    unsigned int i = 0;

    printf("\n");

    for(; i < sizeof(machine->memory); ++i) {
        printf("%.2x ", machine->memory[i]);
        if(0 == ((i+1) % 8)) {
            printf("\n");
        }
    }
}

void reset_machine(vnsem_machine *machine)
{
    /* set everything to zero */
    memset(machine, 0, sizeof(*machine));
}

void load_program(vnsem_machine *machine)
{
    uint8_t instruction = 0, addr = 0;

    /* load the program */
    if(NULL == (config.infile_d = fopen(config.infile_name, "r"))) {
        perror(config.infile_name);
        exit(EXIT_FAILURE);
    }

    printf("Loading program...");

    while(1 == fread((void*)&instruction, 1, 1, config.infile_d)) {
        machine->memory[addr++] = instruction;
    }

    //dump_memory(machine);

    fclose(config.infile_d);

    printf("Done.\n");
}

void set_flag(uint8_t flag, vnsem_machine *machine)
{
    machine->flags |= flag;
}

void unset_flag(uint8_t flag, vnsem_machine *machine)
{
    if(machine->flags & flag) {
        machine->flags ^= flag;
    }
}

uint8_t read_arg(vnsem_machine *machine)
{
    return machine->memory[machine->pc++];
}

void call(uint8_t addr, vnsem_machine *machine)
{
    --machine->sp;
    machine->memory[machine->sp] = machine->pc;
    machine->pc = addr;
}

void con_call(uint8_t addr, uint8_t flag, vnsem_machine *machine)
{
    if(machine->flags & flag) {
        call(addr, machine);
    }
}

void con_no_call(uint8_t addr, uint8_t flag, vnsem_machine *machine)
{
    if(!(machine->flags & flag)) {
        call(addr, machine);
    }
}

void con_jmp(uint8_t addr, uint8_t flag, vnsem_machine *machine)
{
    if(machine->flags & flag) {
        machine->pc = addr;
    }
}

void con_no_jmp(uint8_t addr, uint8_t flag, vnsem_machine *machine)
{
    if(!(machine->flags & flag)) {
        machine->pc = addr;
    }
}

void compare(uint8_t a, uint8_t b, vnsem_machine *machine)
{
    if(a == b) {
        set_flag(F_ZERO, machine);
    } else {
        unset_flag(F_ZERO, machine);
    }
}

void user_output(uint8_t port, vnsem_machine *machine)
{
    printf("PROGRAM OUTPUT> 0x%x (%i)\n", machine->accu, machine->accu);
}

void user_input(uint8_t port, vnsem_machine *machine)
{
    short int value;

    while(1 != sscanf(readline("PROGRAM INPUT> "), "%hd", &value)) {
        printf("Invalid input!\n");
    }

    machine->accu = value;
}

void process_instruction(uint8_t ins, vnsem_machine *m)
{
    switch(ins) {
        /* ----- TRANSFER ----- */
        case 0x7d: /* MOV A,L */ m->accu = m->reg_l; break;
        case 0x7e: /* MOV A,M */ m->accu = m->memory[m->reg_l]; break;
        case 0x77: /* MOV M,A */ m->memory[m->reg_l] = m->accu; break;
        case 0x3e: /* MVI A,n */ m->accu = read_arg(m); break;
        case 0x3a: /* LDA adr */ m->accu = m->memory[read_arg(m)]; break;
        case 0x32: /* STA adr */ m->memory[read_arg(m)] = m->accu; break;
        case 0x6f: /* MOV L,A */ m->reg_l = m->accu; break;
        case 0x6e: /* MOV L,M */ m->reg_l = m->memory[m->reg_l]; break;
        case 0x2e: /* MVI L,n */ m->reg_l = read_arg(m); break;
        case 0x31: /* LXI SP,n */ m->sp = read_arg(m); break;
        case 0xf5: /* PUSH A  */ --m->sp; m->memory[m->sp] = m->accu; break;
        case 0xe5: /* PUSH L  */ --m->sp; m->memory[m->sp] = m->reg_l; break;
        case 0xed: /* PUSH FL */ --m->sp; m->memory[m->sp] = m->flags; break;
        case 0xf1: /* POP A   */ m->accu = m->memory[m->sp]; ++m->sp; break;
        case 0xe1: /* POP L   */ m->reg_l = m->memory[m->sp]; ++m->sp; break;
        case 0xfd: /* POP FL  */ m->flags = m->memory[m->sp]; ++m->sp; break;
        case 0xdb: /* IN adr  */ user_input(read_arg(m), m); break;
        case 0xd3: /* OUT adr */ user_output(read_arg(m), m); break;
        /* ------ ARITHMETIC  ------ */
        case 0x3c: /* INR A */ ++m->accu; break;
        case 0x2c: /* INR L */ ++m->reg_l; break;
        case 0x3d: /* DCR A */ --m->accu; break;
        case 0x2d: /* DCR L */ --m->reg_l; break;
        case 0x87: /* ADD A */ m->accu *= 2; break;
        case 0x85: /* ADD L */ m->accu += m->reg_l; break;
        case 0x86: /* ADD M */ m->accu += m->memory[m->reg_l]; break;
        case 0xc6: /* ADI n */ m->accu += read_arg(m); break;
        case 0x97: /* SUB A */ m->accu = 0; break;
        case 0x95: /* SUB L */ m->reg_l = 0; break;
        case 0x96: /* SUB M */ m->accu -= m->memory[m->reg_l]; break;
        case 0xd6: /* SUI n */ m->accu -= read_arg(m); break;
        case 0xbf: /* CMP A */ set_flag(F_ZERO, m); break;
        case 0xbd: /* CMP L */ compare(m->reg_l, m->accu, m); break;
        case 0xbe: /* CMP M */ compare(m->memory[m->reg_l], m->accu, m); break;
        case 0xfe: /* CPI n */ compare(m->accu, read_arg(m), m); break;
        /* ----- LOGIC ----- */
        case 0xa7: /* ANA A */ /* nothing to do */ break;
        case 0xa5: /* ANA L */ m->accu &= m->reg_l; break;
        case 0xa6: /* ANA M */ m->accu &= m->memory[m->reg_l]; break;
        case 0xe6: /* ANI n */ m->accu &= read_arg(m); break;
        case 0xb7: /* ORA A */ /* nothing to do */ break;
        case 0xb5: /* ORA L */ m->accu |= m->reg_l; break;
        case 0xb6: /* ORA M */ m->accu |= m->memory[m->reg_l]; break;
        case 0xf6: /* ORI n */ m->accu |= read_arg(m); break;
        case 0xaf: /* XRA A */ m->accu = 0; break;
        case 0xad: /* XRA L */ m->accu ^= m->reg_l; break;
        case 0xae: /* XRA M */ m->accu ^= m->memory[m->reg_l]; break;
        case 0xee: /* XRI n */ m->accu ^= read_arg(m); break;
        /* ----- BRANCH ----- */
        case 0xc3: /* JMP adr  */ m->pc = read_arg(m); break;
        case 0xcd: /* CALL adr */ call(read_arg(m), m); break;
        case 0xca: /* JZ adr */ con_jmp(read_arg(m), F_ZERO, m); break;
        case 0xcc: /* CZ adr */ con_call(read_arg(m), F_ZERO, m); break;
        case 0xc4: /* CNZ adr */ con_no_call(read_arg(m), F_ZERO, m); break;
        case 0xc2: /* JNZ adr */ con_no_jmp(read_arg(m), F_ZERO, m); break;
        case 0xdc: /* CC adr */ con_call(read_arg(m), F_CARRY, m); break;
        case 0xda: /* JC adr */ con_jmp(read_arg(m), F_CARRY, m); break;
        case 0xd2: /* JNC adr */ con_no_jmp(read_arg(m), F_CARRY, m); break;
        case 0xd4: /* CNC adr */ con_no_call(read_arg(m), F_CARRY, m); break;
        case 0xc9: /* RET */ m->pc = m->memory[m->sp]; ++m->sp; break;
        /* ----- SPECIAL ----- */
        case 0x76: /* HLT */ m->halted = TRUE; break;
        case 0x00: /* NOP */ break;
        case 0xfb: /* EI  */ m->interrupt_enabled = TRUE; break;
        case 0xf3: /* DI  */ m->interrupt_enabled = FALSE; break;
    }
}

int emulate(void)
{
    uint8_t next_ins;

    vnsem_machine machine;
    reset_machine(&machine);

    load_program(&machine);

    print_key();

    while(!machine.halted) {

        next_ins = machine.memory[machine.pc];
        ++machine.pc;
        ++machine.step_count;

        process_instruction(next_ins, &machine);

        print_machine_state(&machine);

        usleep(config.step_time_ms * 1000);
    }

    printf("HALTED\n");

    return EXIT_SUCCESS;
}

void print_usage(char *pname)
{
    printf("\nUsage: %s [-h] | [-v] [-i] [-s <ms>] <program>\n\n", pname);
    printf("  -h         Show this help text.\n");
    printf("  -v         Turn on verbose output.\n");
    printf("  -i         Turn on interactive mode.\n");
    printf("  -s <ms>    Set step time to <ms> milliseconds.\n");
    printf("\n");
}

int main(int argc, char **argv)
{
    unsigned int opt;
    char *process_name = util_basename(argv[0]);

    printf(BANNER_LINE1, "Emulator");
    printf(BANNER_LINE2, VERSION);

    config.interactive_mode = FALSE;
    config.verbose_mode = FALSE;
    config.step_time_ms = 100;
    config.infile_name = NULL;

    while(-1 != (opt = getopt(argc, argv, "hvis:"))) {
        switch(opt) {
            case 'h':
                print_usage(process_name);
                return EXIT_SUCCESS;
            case 'v':
                config.verbose_mode = TRUE;
                break;
            case 's':
                config.step_time_ms = atoi(optarg);
                break;
            case 'i':
                config.interactive_mode = TRUE;
                break;
            default:
                print_usage(process_name);
                return EXIT_SUCCESS;
        }
    }

    if(optind > argc) {
        print_usage(process_name);
        return EXIT_SUCCESS;
    }

    config.infile_name = strdup(argv[optind]);

    return emulate();
}
