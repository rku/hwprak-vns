/**
 * This file is part of hwprak-vns.
 * Copyright 2013-2015 (c) René Küttner <rene@spaceshore.net>
 *
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
    uint16_t step_time_ms;
    char *infile_name;
} vnsem_configuration;

typedef uint8_t led;

typedef struct _vnsem_machine {
    unsigned int step_count;
    uint8_t step_mode;
    uint8_t halted;
    uint8_t int_active;
    uint8_t break_enabled;
    uint8_t break_point;
    /* the memory unit */
    uint8_t mem[256];
    /* register */
    uint8_t pc;
    uint8_t reg_l;
    uint8_t sp;
    /* alu */ 
    uint8_t accu;
    uint8_t flags;
} vnsem_machine;

#define F_NONE  0x0
#define F_CARRY 0x1
#define F_ZERO  0x2
#define F_SIGN  0x4

void dump_memory(vnsem_machine *machine);
void reset_machine(vnsem_machine *machine);
int load_program(char *filepath, uint8_t offset, vnsem_machine *machine);

#define ERR_ILLEGAL_INSTRUCTION (1)

#endif /* VNSEM_H */
