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
    unsigned int step_count;
    unsigned int halted;
    uint8_t interrupt_enabled;
    /* the memory unit */
    uint8_t memory[256];
    /* register */
    uint8_t program_counter;
    uint8_t address_buffer;
    uint8_t data_buffer;
    uint8_t l_register;
    uint8_t stack_pointer;
    /* alu */ 
    uint8_t accumulator;
    uint8_t flags;
} vnsem_machine;

#define FLAG_CARRY  0x1
#define FLAG_ZERO   0x2
#define FLAG_SIGN   0x4

#endif /* VNSEM_H */
