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
