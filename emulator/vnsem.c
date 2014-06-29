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
#include <signal.h>
#include <readline/readline.h>

#include "globals.h"
#include "utils.h"
#include "vnsem.h"

#define ERR_ILLEGAL_INSTRUCTION (1)

vnsem_configuration config;

void print_machine_state(vnsem_machine *machine)
{
    printf("#%.3i  ", machine->step_count);
    printf("[ ACCU=0x%.2x  L=0x%.2x  PC=0x%.2x  SP=0x%.2x ]  ",
            machine->accu,
            machine->reg_l,
            machine->pc,
            machine->sp);
    printf("C:%c  Z:%c  S:%c\n",
            (machine->flags & F_CARRY) ? '*' : '.',
            (machine->flags & F_ZERO)  ? '*' : '.',
            (machine->flags & F_SIGN)  ? '*' : '.');
}

void print_key(void)
{
    printf("\n");
    printf(" Registers: program counter (PC), L register (L),\n");
    printf("            stack pointer (SP), accumulator (ACCU)\n");
    printf("     Flags: carry (C), zero (Z), sign (S)\n");
    printf("\n");
}

void dump_memory(vnsem_machine *machine)
{
    int i;
    printf("\n 0x00   ");
    for (i = 0; i < 256; ++i) {
        if (i > 0 && !(i % 8)) printf("\n 0x%.2x   ", i);
        printf("%.2x ", machine->mem[i]);
    }
    printf("\n\n");
}

void reset_machine(vnsem_machine *machine)
{
    /* set everything to zero */
    memset(machine, 0, sizeof(*machine));
}

void load_program(vnsem_machine *machine)
{
    /* load the program */
    if (NULL == (config.infile_d = fopen(config.infile_name, "r"))) {
        perror(config.infile_name);
        exit(EXIT_FAILURE);
    }

    printf("Loading program...");

    fread((void*)&machine->mem[0],
          1,
          sizeof(machine->mem),
          config.infile_d);

    fclose(config.infile_d);

    printf("Done.\n");
}

void handle_interrupt(int signal)
{
    printf("\nInterrupt received. ");
    printf("(Interrupt handling not yet implemented.)\n");
    exit(EXIT_SUCCESS);
}

void set_flag(uint8_t flag, uint8_t state, vnsem_machine *machine)
{
    if (state) {
        machine->flags |= flag;
    } else {
        if (machine->flags & flag) {
            machine->flags ^= flag;
        }
    }
}

uint8_t read_arg(vnsem_machine *machine)
{
    return machine->mem[machine->pc++];
}

void call(uint8_t addr, vnsem_machine *machine)
{
    --machine->sp;
    machine->mem[machine->sp] = machine->pc;
    machine->pc = addr;
}

void con_call(uint8_t addr, uint8_t flag, vnsem_machine *machine)
{
    if (machine->flags & flag) {
        call(addr, machine);
    }
}

void con_no_call(uint8_t addr, uint8_t flag, vnsem_machine *machine)
{
    if (!(machine->flags & flag)) {
        call(addr, machine);
    }
}

void con_jmp(uint8_t addr, uint8_t flag, vnsem_machine *machine)
{
    if (machine->flags & flag) {
        machine->pc = addr;
    }
}

void con_no_jmp(uint8_t addr, uint8_t flag, vnsem_machine *machine)
{
    if (!(machine->flags & flag)) {
        machine->pc = addr;
    }
}

void update_flags_for_value(int16_t value, vnsem_machine *machine)
{
    set_flag(F_ZERO, (0 == (value & 0xff)) ? TRUE : FALSE, machine);
    set_flag(F_SIGN, (value & 0x80) ? TRUE : FALSE, machine);
    set_flag(F_CARRY, (value & 0x100) ? TRUE : FALSE, machine);
}

void compare(uint8_t other, vnsem_machine *machine)
{
    update_flags_for_value(machine->accu - other, machine);
}

void accu_op(int16_t result, vnsem_machine *machine)
{
    uint8_t value = (result & 0xff);
    update_flags_for_value(result, machine);
    machine->accu = (value < 0) ? -value : value;
}

void user_output(uint8_t port, vnsem_machine *machine)
{
    printf("[%.2x] Program output => 0x%x (%i)\n",
            port, machine->accu, machine->accu);
}

void user_input(uint8_t port, vnsem_machine *machine)
{
    short int value;
    char prompt[32], *input;

    snprintf((char*)&prompt, 32, "[%.2x] Program input => ", port);

    while (1) {
        input = readline(prompt);

        if (NULL != input && 1 == sscanf(input, "%hd", &value)) {
            free(input);
            break;
        }

        fprintf(stderr, "Bad input. Expected byte in hex/dec notation.\n");
        free(input);
    }

    machine->accu = value;
}

int process_instruction(uint8_t ins, vnsem_machine *m)
{
    switch (ins) {
        /* ----- TRANSFER ----- */
        case 0x7d: /* MOV A,L */ m->accu = m->reg_l;                   break;
        case 0x7e: /* MOV A,M */ m->accu = m->mem[m->reg_l];           break;
        case 0x77: /* MOV M,A */ m->mem[m->reg_l] = m->accu;           break;
        case 0x3e: /* MVI A,n */ m->accu = read_arg(m);                break;
        case 0x3a: /* LDA adr */ m->accu = m->mem[read_arg(m)];        break;
        case 0x32: /* STA adr */ m->mem[read_arg(m)] = m->accu;        break;
        case 0x6f: /* MOV L,A */ m->reg_l = m->accu;                   break;
        case 0x6e: /* MOV L,M */ m->reg_l = m->mem[m->reg_l];          break;
        case 0x2e: /* MVI L,n */ m->reg_l = read_arg(m);               break;
        case 0x31: /* LXI SP,n*/ m->sp = read_arg(m);                  break;
        case 0xf5: /* PUSH A  */ --m->sp; m->mem[m->sp] = m->accu;     break;
        case 0xe5: /* PUSH L  */ --m->sp; m->mem[m->sp] = m->reg_l;    break;
        case 0xed: /* PUSH FL */ --m->sp; m->mem[m->sp] = m->flags;    break;
        case 0xf1: /* POP A   */ m->accu  = m->mem[m->sp]; ++m->sp;    break;
        case 0xe1: /* POP L   */ m->reg_l = m->mem[m->sp]; ++m->sp;    break;
        case 0xfd: /* POP FL  */ m->flags = m->mem[m->sp]; ++m->sp;    break;
        case 0xdb: /* IN adr  */ user_input(read_arg(m), m);           break;
        case 0xd3: /* OUT adr */ user_output(read_arg(m), m);          break;
        /* ------ ARITHMETIC  ------ */
        case 0x3c: /* INR A */ accu_op(m->accu + 1, m);                break;
        case 0x2c: /* INR L */ ++m->reg_l;                             break;
        case 0x3d: /* DCR A */ accu_op(m->accu - 1, m);                break;
        case 0x2d: /* DCR L */ --m->reg_l;                             break;
        case 0x87: /* ADD A */ accu_op(m->accu * 2, m);                break;
        case 0x85: /* ADD L */ accu_op(m->accu + m->reg_l, m);         break;
        case 0x86: /* ADD M */ accu_op(m->accu + m->mem[m->reg_l], m); break;
        case 0xc6: /* ADI n */ accu_op(m->accu + read_arg(m), m);      break;
        case 0x97: /* SUB A */ accu_op(0, m);                          break;
        case 0x95: /* SUB L */ accu_op(m->accu - m->reg_l, m);         break;
        case 0x96: /* SUB M */ accu_op(m->accu - m->mem[m->reg_l], m); break;
        case 0xd6: /* SUI n */ accu_op(m->accu - read_arg(m), m);      break;
        case 0xbf: /* CMP A */ compare(m->accu, m);                    break;
        case 0xbd: /* CMP L */ compare(m->reg_l, m);                   break;
        case 0xbe: /* CMP M */ compare(m->mem[m->reg_l], m);           break;
        case 0xfe: /* CPI n */ compare(read_arg(m), m);                break;
        /* ----- LOGIC ----- */
        case 0xa7: /* ANA A */ accu_op(m->accu & m->accu, m);          break;
        case 0xa5: /* ANA L */ accu_op(m->accu & m->reg_l, m);         break;
        case 0xa6: /* ANA M */ accu_op(m->accu & m->mem[m->reg_l], m); break;
        case 0xe6: /* ANI n */ accu_op(m->accu & read_arg(m), m);      break;
        case 0xb7: /* ORA A */ accu_op(m->accu | m->accu, m);          break;
        case 0xb5: /* ORA L */ accu_op(m->accu | m->reg_l, m);         break;
        case 0xb6: /* ORA M */ accu_op(m->accu | m->mem[m->reg_l], m); break;
        case 0xf6: /* ORI n */ accu_op(m->accu | read_arg(m), m);      break;
        case 0xaf: /* XRA A */ accu_op(m->accu ^ m->accu, m);          break;
        case 0xad: /* XRA L */ accu_op(m->accu ^ m->reg_l, m);         break;
        case 0xae: /* XRA M */ accu_op(m->accu ^ m->mem[m->reg_l], m); break;
        case 0xee: /* XRI n */ accu_op(m->accu ^ read_arg(m), m);      break;
        /* ----- BRANCH ----- */
        case 0xc3: /* JMP adr */ m->pc = read_arg(m);                  break;
        case 0xcd: /* CALL adr*/ call(read_arg(m), m);                 break;
        case 0xca: /* JZ  adr */ con_jmp(read_arg(m), F_ZERO, m);      break;
        case 0xcc: /* CZ  adr */ con_call(read_arg(m), F_ZERO, m);     break;
        case 0xc4: /* CNZ adr */ con_no_call(read_arg(m), F_ZERO,  m); break;
        case 0xc2: /* JNZ adr */ con_no_jmp(read_arg(m), F_ZERO,  m);  break;
        case 0xdc: /* CC  adr */ con_call(read_arg(m), F_CARRY, m);    break;
        case 0xda: /* JC  adr */ con_jmp(read_arg(m), F_CARRY, m);     break;
        case 0xd2: /* JNC adr */ con_no_jmp(read_arg(m), F_CARRY, m);  break;
        case 0xd4: /* CNC adr */ con_no_call(read_arg(m), F_CARRY, m); break;
        case 0xc9: /* RET     */ m->pc = m->mem[m->sp]; ++m->sp;       break;
        /* ----- SPECIAL ----- */
        case 0x76: /* HLT */ m->halted = TRUE;                         break;
        case 0x00: /* NOP */                                           break;
        case 0xfb: /* EI  */ m->int_active = TRUE;                     break;
        case 0xf3: /* DI  */ m->int_active = FALSE;                    break;
        default:
            return ERR_ILLEGAL_INSTRUCTION;
    }
    return 0;
}

int emulate(void)
{
    uint8_t next_ins;

    vnsem_machine machine;
    reset_machine(&machine);

    load_program(&machine);

    print_key();

    signal(SIGINT, handle_interrupt);

    while (1) {
        next_ins = machine.mem[machine.pc];
        ++machine.pc;
        ++machine.step_count;

        switch (process_instruction(next_ins, &machine)) {
            case 0: {
                break;
            }
            case ERR_ILLEGAL_INSTRUCTION: {
                fprintf(stderr,
                        "\nError: Unknown instruction 0x%.2x "
                        "at address 0x%.2x.\n",
                        next_ins, machine.pc - 1);
                dump_memory(&machine);
                return EXIT_FAILURE;
            }
            default: {
                fprintf(stderr,
                        "\nError: Could not execute instruction 0x%.2x "
                        "at address 0x%.2x for unknown reason.\n",
                        next_ins, machine.pc - 1);
                dump_memory(&machine);
                return EXIT_FAILURE;
            }
        }

        print_machine_state(&machine);

        usleep(config.step_time_ms * 1000);

        if (machine.halted) {
            printf("Machine halted.\n");
            if (config.dump_mem_on_halt) {
                dump_memory(&machine);
            }
            while (machine.halted) sleep(1);
        }
    }

    return EXIT_SUCCESS;
}

void print_usage(char *pname)
{
    printf("\nUsage: %s [-h] | [-v] [-i] [-s <ms>] <program>\n\n", pname);
    printf("  -h         Show this help text.\n");
    printf("  -v         Turn on verbose output.\n");
    printf("  -i         Turn on interactive mode.\n");
    printf("  -s <ms>    Set step time to <ms> milliseconds.\n");
    printf("  -d         Dump memory when machine is halted.\n");
    printf("\n");
}

int main(int argc, char **argv)
{
    unsigned int opt;
    char *p, *process_name = util_basename(argv[0]);

    printf(BANNER_LINE1, "Emulator");
    printf(BANNER_LINE2, VERSION);

    config.interactive_mode = FALSE;
    config.verbose_mode = FALSE;
    config.step_time_ms = 0;
    config.infile_name = NULL;
    config.dump_mem_on_halt = FALSE;

    while (-1 != (opt = getopt(argc, argv, "hvis:d"))) {
        switch (opt) {
            case 'h':
                print_usage(process_name);
                return EXIT_SUCCESS;
            case 'v':
                config.verbose_mode = TRUE;
                break;
            case 's':
                config.step_time_ms = strtol(optarg, &p, 10);
                if (!optarg || *p) {
                    fprintf(stderr, "Invalid step time argument.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'i':
                config.interactive_mode = TRUE;
                break;
            case 'd':
                config.dump_mem_on_halt = TRUE;
                break;
            default:
                print_usage(process_name);
                return EXIT_SUCCESS;
        }
    }

    if (optind >= argc) {
        print_usage(process_name);
        return EXIT_SUCCESS;
    }

    config.infile_name = strdup(argv[optind]);

    return emulate();
}
