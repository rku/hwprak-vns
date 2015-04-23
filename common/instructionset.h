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

#ifndef INSTRUCTIONSET_H
#define INSTRUCTIONSET_H 1

#include <stdint.h>

typedef uint8_t argtype;

#define AT_NONE     0x00
#define AT_INT      0x01
#define AT_ADDR     0x03    // sets AT_INT bit too
#define AT_REG_A    0x04
#define AT_REG_L    0x08
#define AT_REG_FL   0x10
#define AT_REG_SP   0x20
#define AT_MEM      0x40
#define AT_LABEL    0x83    // set AT_INT, AT_ADDR too 

typedef struct _vns_instruction {
    const char *mnemonic;
    argtype at1;
    argtype at2;
    uint8_t opcode;
} vns_instruction;

int is_lookup_mnemonic_name(const char *str);
vns_instruction *is_find_mnemonic(const char *mnemonic,
                                  argtype at1, argtype at2);
vns_instruction *is_find_opcode(uint8_t opcode);

#endif /* INSTRUCTIONSET_H */
