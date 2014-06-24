#ifndef VNSEM_H
#define VNSEM_H 1

#include <stdio.h>
#include <stdint.h>

typedef struct _vnsem_configuration {
    uint8_t interactive_mode;
    uint8_t verbose_mode;
    uint16_t step_time_ms;
    char *infile_name;
    FILE *infile_d;
} vnsem_configuration;

typedef uint8_t led;

typedef struct _vnsem_machine {
    int step_count;
    char halted;
    /* the memory unit */
    char memory[256];
    char curr_mem_data;
    char curr_mem_addr;
    /* register */
    char program_counter;
    char address_buffer;
    char data_buffer;
    char l_register;
    char stack_pointer;
    /* alu */ 
    char accumulator;
    char carry_flag;
    char zero_flag;
    char sign_flag;
} vnsem_machine;

#endif /* VNSEM_H */
