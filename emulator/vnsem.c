
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "vnsem.h"

vnsem_configuration config;

void print_machine_state(vnsem_machine *machine)
{
    printf("#%.3i  ", machine->step_count);
    printf("R:[pc=%.2x ab=%.2x db=%.2x l=%.2x sp=%.2x]  ",
            machine->program_counter,
            machine->address_buffer,
            machine->data_buffer,
            machine->l_register,
            machine->stack_pointer);
    printf("A:[v=%.2x c=%.1i z=%.1i s=%.1i]\n",
            machine->accumulator,
            machine->flags & FLAG_CARRY,
            machine->flags & FLAG_ZERO,
            machine->flags & FLAG_SIGN);
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
    machine->flags ^= flag;
}

uint8_t read_argument(vnsem_machine *machine)
{
    return machine->memory[machine->program_counter++];
}

void process_instruction(uint8_t ins, vnsem_machine *m)
{
    if(config.verbose_mode) {
        printf("Processing instruction %x\n", ins);
    }

    switch(ins) {
        /* ----- TRANSFER ----- */
        case 0x7d: /* MOV A,L */
            m->accumulator = m->l_register;
            break;
        case 0x7e: /* MOV A,M */
            m->accumulator = m->memory[m->l_register];
            break;
        case 0x77: /* MOV M,A */
            m->memory[m->l_register] = m->accumulator;
            break;
        case 0x3e: /* MVI A,n */
            m->accumulator = read_argument(m);
            break;
        case 0x3a: /* LDA adr */
            m->accumulator = m->memory[read_argument(m)];
            break;
        case 0x32: /* STA adr */
            m->memory[read_argument(m)] = m->accumulator;
            break;
        case 0x6f: /* MOV L,A */
            m->l_register = m->accumulator;
            break;
        case 0x6e: /* MOV L,M */
            m->l_register = m->memory[m->l_register];
            break;
        case 0x2e: /* MVI L,n */
            m->l_register = read_argument(m);
            break;
        case 0x31: /* LXI SP,n */
            m->stack_pointer = read_argument(m);
            break;
        case 0xf5: /* PUSH A */
            --m->stack_pointer;
            m->memory[m->stack_pointer] = m->accumulator;
            break;
        case 0xe5: /* PUSH L */
            --m->stack_pointer;
            m->memory[m->stack_pointer] = m->l_register;
            break;
        case 0xed: /* PUSH FL */
            --m->stack_pointer;
            m->memory[m->stack_pointer] = m->flags;
            break;
        case 0xf1: /* POP A */
            m->accumulator = m->memory[m->stack_pointer];
            ++m->stack_pointer;
            break;
        case 0xe1: /* POP L */
            m->l_register = m->memory[m->stack_pointer];
            ++m->stack_pointer;
            break;
        case 0xfd: /* POP FL */
            m->flags = m->memory[m->stack_pointer];
            ++m->stack_pointer;
            break;
        case 0xdb: /* IN adr */
            break;
        case 0xd3: /* OUT adr */
            break;
        /* ------ ARITHMETIC  ------ */
        case 0x3c: /* INR A */
            ++m->accumulator;
            break;
        case 0x2c: /* INR L */
            ++m->l_register;
            break;
        case 0x3d: /* DCR A */
            --m->accumulator;
            break;
        case 0x2d: /* DCR L */
            --m->l_register;
            break;
        case 0x87: /* ADD A */
            m->accumulator *= 2;
            break;
        case 0x85: /* ADD L */
            m->accumulator += m->l_register;
            break;
        case 0x86: /* ADD M */
            m->accumulator += m->memory[m->l_register];
            break;
        case 0xc6: /* ADI n */
            m->accumulator += read_argument(m);
            break;
        case 0x97: /* SUB A */
            m->accumulator = 0;
            break;
        case 0x95: /* SUB L */
            m->l_register = 0;
            break;
        case 0x96: /* SUB M */
            m->accumulator -= m->memory[m->l_register];
            break;
        case 0xd6: /* SUI n */
            m->accumulator -= read_argument(m);
            break;
        case 0xbf: /* CMP A */
            set_flag(FLAG_ZERO, m);
            break;
        case 0xbd: /* CMP L */
            if(m->l_register == m->accumulator) {
                set_flag(FLAG_ZERO, m);
            } else {
                unset_flag(FLAG_ZERO, m);
            }
            break;
        case 0xbe: /* CMP M */
            if(m->memory[m->l_register] == m->accumulator) {
                set_flag(FLAG_ZERO, m);
            } else {
                unset_flag(FLAG_ZERO, m);
            }
            break;
        case 0xfe: /* CPI n */
            if(m->accumulator == read_argument(m)) {
                set_flag(FLAG_ZERO, m);
            } else {
                unset_flag(FLAG_ZERO, m);
            }
            break;
        /* ----- LOGIC ----- */
        case 0xa7: /* ANA A */
            /* nothing to do */
            break;
        case 0xa5: /* ANA L */
            m->accumulator &= m->l_register;
            break;
        case 0xa6: /* ANA M */
            m->accumulator &= m->memory[m->l_register];
            break;
        case 0xe6: /* ANI n */
            m->accumulator &= read_argument(m);
            break;
        case 0xb7: /* ORA A */
            /* nothing to do */
            break;
        case 0xb5: /* ORA L */
            m->accumulator |= m->l_register;
            break;
        case 0xb6: /* ORA M */
            m->accumulator |= m->memory[m->l_register];
            break;
        case 0xf6: /* ORI n */
            m->accumulator |= read_argument(m);
            break;
        case 0xaf: /* XRA A */
            m->accumulator = 0;
            break;
        case 0xad: /* XRA L */
            m->accumulator ^= m->l_register;
            break;
        case 0xae: /* XRA M */
            m->accumulator ^= m->memory[m->l_register];
            break;
        case 0xee: /* XRI n */
            m->accumulator ^= read_argument(m);
            break;
        /* ----- BRANCH ----- */
        case 0xc3: /* JMP adr */
            break;
        case 0xca: /* JZ adr */
            break;
        case 0xc2: /* JNZ adr */
            break;
        case 0xda: /* JC adr */
            break;
        case 0xd2: /* JNC adr */
            break;
        case 0xcd: /* CALL adr */
            m->program_counter = read_argument(m);
            break;
        case 0xcc: /* CZ adr */
            break;
        case 0xc4: /* CNZ adr */
            break;
        case 0xdc: /* CC adr */
            break;
        case 0xd4: /* CNC adr */
            break;
        case 0xc9: /* RET */
            break;
        /* ----- SPECIAL ----- */
        case 0x76: /* HLT */
            m->halted = TRUE;
            break;
        case 0x00: /* NOP */
            break;
        case 0xfb: /* EI */
            m->interrupt_enabled = TRUE;
            break;
        case 0xf3: /* DI */
            m->interrupt_enabled = FALSE;
            break;
    }
}

int emulate(void)
{
    uint8_t next_ins;

    vnsem_machine machine;
    reset_machine(&machine);

    load_program(&machine);

    while(!machine.halted) {

        next_ins = machine.memory[machine.program_counter];
        ++machine.program_counter;
        ++machine.step_count;

        process_instruction(next_ins, &machine);

        print_machine_state(&machine);
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
    printf("\n");
}

int main(int argc, char **argv)
{
    unsigned int opt;
    char *process_name = argv[0];

    printf(BANNER_LINE1, "Emulator");
    printf(BANNER_LINE2, VERSION);

    config.interactive_mode = FALSE;
    config.verbose_mode = FALSE;
    config.step_time_ms = 1000;
    config.infile_name = NULL;

    while(-1 != (opt = getopt(argc, argv, "hvis:"))) {
        switch(opt) {
            case 'h':
                print_usage(process_name);
                return EXIT_SUCCESS;
            case 'v':
                config.verbose_mode = TRUE;
                break;
            case 'i':
                config.step_time_ms = atoi(optarg);
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
