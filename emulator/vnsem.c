
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
    /*printf("M:[d=%.2x a=%.2x]  ",
            machine->curr_mem_data,
            machine->curr_mem_addr);*/
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

void reset_machine(vnsem_machine *machine)
{
    /* set everything to zero */
    memset(machine, 0, sizeof(*machine));
}

uint8_t read_byte()
{
    uint8_t instruction = 0;

    if(1 != fread((void*)&instruction, 1, 1, config.infile_d)) {
        perror(config.infile_name);
        exit(EXIT_FAILURE);
    }

    return instruction;
}

void process_instruction(uint8_t ins, vnsem_machine *machine)
{
    if(config.verbose_mode) {
        printf("Processing instruction %x\n", ins);
    }

    switch(ins) {
        /* ----- TRANSFER ----- */
        case 0x7d: /* MOV A,L */
            machine->accumulator = machine->l_register;
            break;
        case 0x7e: /* MOV A,M */
            machine->accumulator = machine->memory[machine->l_register];
            break;
        case 0x77: /* MOV M,A */
            machine->memory[machine->l_register] = machine->accumulator;
            break;
        case 0x3e: /* MVI A,n */
            machine->accumulator = read_byte();
            break;
        case 0x3a: /* LDA adr */
            machine->accumulator = machine->memory[read_byte()];
            break;
        case 0x32: /* STA adr */
            machine->memory[read_byte()] = machine->accumulator;
            break;
        case 0x6f: /* MOV L,A */
            machine->l_register = machine->accumulator;
            break;
        case 0x6e: /* MOV L,M */
            machine->l_register = machine->memory[machine->l_register];
            break;
        case 0x2e: /* MVI L,n */
            machine->l_register = read_byte();
            break;
        case 0x31: /* LXI SP,n */
            machine->stack_pointer = read_byte();
            break;
        case 0xf5: /* PUSH A */
            --machine->stack_pointer;
            machine->memory[machine->stack_pointer] = machine->accumulator;
            break;
        case 0xe5: /* PUSH L */
            --machine->stack_pointer;
            machine->memory[machine->stack_pointer] = machine->l_register;
            break;
        case 0xed: /* PUSH FL */
            --machine->stack_pointer;
            machine->memory[machine->stack_pointer] = machine->flags;
            break;
        case 0xf1: /* POP A */
            machine->accumulator = machine->memory[machine->stack_pointer];
            ++machine->stack_pointer;
            break;
        case 0xe1: /* POP L */
            machine->l_register = machine->memory[machine->stack_pointer];
            ++machine->stack_pointer;
            break;
        case 0xfd: /* POP FL */
            machine->flags = machine->memory[machine->stack_pointer];
            ++machine->stack_pointer;
            break;
        case 0xdb: /* IN adr */
            break;
        case 0xd3: /* OUT adr */
            break;
        /* ------ ARITHMETIC  ------ */
        case 0x3c: /* INR A */
            ++machine->accumulator;
            break;
        case 0x2c: /* INR L */
            ++machine->l_register;
            break;
        case 0x3d: /* DCR A */
            --machine->accumulator;
            break;
        case 0x2d: /* DCR L */
            --machine->l_register;
            break;
        case 0x87: /* ADD A */
            machine->accumulator *= 2;
            break;
        case 0x85: /* ADD L */
            machine->accumulator += machine->l_register;
            break;
        case 0x86: /* ADD M */
            machine->accumulator += machine->memory[machine->curr_mem_addr];
            break;
        case 0xc6: /* ADI n */
            machine->accumulator += read_byte();
            break;
        case 0x97: /* SUB A */
            machine->accumulator = 0;
            break;
        case 0x95: /* SUB L */
            machine->l_register = 0;
            break;
        case 0x96: /* SUB M */
            machine->accumulator -= machine->memory[machine->curr_mem_addr];
            break;
        case 0xd6: /* SUI n */
            machine->accumulator -= read_byte();
            break;
        case 0xbf: /* CMP A */
            break;
        case 0xbd: /* CMP L */
            break;
        case 0xbe: /* CMP M */
            break;
        case 0xfe: /* CPI n */
            break;
        /* ----- LOGIC ----- */
        case 0xa7: /* ANA A */
            break;
        case 0xa5: /* ANA L */
            break;
        case 0xa6: /* ANA M */
            break;
        case 0xe6: /* ANI n */
            break;
        case 0xb7: /* ORA A */
            break;
        case 0xb5: /* ORA L */
            break;
        case 0xb6: /* ORA M */
            break;
        case 0xf6: /* ORI n */
            break;
        case 0xaf: /* XRA A */
            break;
        case 0xad: /* XRA L */
            break;
        case 0xae: /* XRA M */
            break;
        case 0xee: /* XRI n */
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
            machine->halted = TRUE;
            break;
        case 0x00: /* NOP */
            break;
        case 0xfb: /* EI */
            machine->interrupt_enabled = TRUE;
            break;
        case 0xf3: /* DI */
            machine->interrupt_enabled = FALSE;
            break;
    }
}

int emulate(void)
{
    vnsem_machine machine;
    reset_machine(&machine);

    /* load the program */
    if(NULL == (config.infile_d = fopen(config.infile_name, "r"))) {
        perror(config.infile_name);
        return EXIT_FAILURE;
    }

    while(!feof(config.infile_d) && !machine.halted) {
        uint8_t instruction = read_byte();
        ++machine.step_count;
        process_instruction(instruction, &machine);
        print_machine_state(&machine);
    }

    fclose(config.infile_d);

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
