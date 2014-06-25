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

#ifndef INSTRUCTIONSET_H
#define INSTRUCTIONSET_H 1

#include <stdint.h>

typedef uint8_t argtype;

#define AT_NONE     0x01
#define AT_INT      0x02
#define AT_ADDR     0x04
#define AT_REG_A    0x08
#define AT_REG_L    0x10
#define AT_REG_FL   0x20
#define AT_REG_SP   0x40
#define AT_MEM      0x80

typedef struct _vns_instruction {
    char *mnemonic;
    argtype at1;
    argtype at2;
    uint8_t opcode;
} vns_instruction;

vns_instruction *find_mnemonic(char *mnemonic, argtype at1, argtype at2);
vns_instruction *find_opcode(uint8_t opcode);

/**
 * This list must be sorted by mnemonic,arg1,arg2 in ascending order
 * to allow application of binary search algorithms.
 */
vns_instruction instructionset[] = {
    /* mnemonic   arg1        arg2 */
    { "ADD",    AT_REG_A,   AT_NONE,    0x87 },
    { "ADD",    AT_REG_L,   AT_NONE,    0x85 },
    { "ADD",    AT_MEM,     AT_NONE,    0x86 },
    { "ADI",    AT_INT,     AT_NONE,    0xc6 },
    { "ANA",    AT_REG_A,   AT_NONE,    0xa7 },
    { "ANA",    AT_REG_L,   AT_NONE,    0xa5 },
    { "ANA",    AT_MEM,     AT_NONE,    0xa6 },
    { "ANI",    AT_INT,     AT_NONE,    0xe6 },
    { "CALL",   AT_ADDR,    AT_NONE,    0xcd },
    { "CC",     AT_ADDR,    AT_NONE,    0xdc },
    { "CNC",    AT_ADDR,    AT_NONE,    0xd4 },
    { "CMP",    AT_REG_A,   AT_NONE,    0xbf },
    { "CMP",    AT_REG_L,   AT_NONE,    0xbd },
    { "CMP",    AT_MEM,     AT_NONE,    0xbe },
    { "CNZ",    AT_ADDR,    AT_NONE,    0xc4 },
    { "CPI",    AT_INT,     AT_NONE,    0xfe },
    { "CZ",     AT_ADDR,    AT_NONE,    0xcc },
    { "DCR",    AT_REG_A,   AT_NONE,    0x3d },
    { "DCR",    AT_REG_L,   AT_NONE,    0x2d },
    { "DI",     AT_NONE,    AT_NONE,    0xf3 }
    { "EI",     AT_NONE,    AT_NONE,    0xfb },
    { "HLT",    AT_NONE,    AT_NONE,    0x76 },
    { "IN",     AT_ADDR,    AT_NONE,    0xdb },
    { "INR",    AT_REG_A,   AT_NONE,    0x3c },
    { "INR",    AT_REG_L,   AT_NONE,    0x2c },
    { "JC",     AT_ADDR,    AT_NONE,    0xda },
    { "JMP",    AT_ADDR,    AT_NONE,    0xc3 },
    { "JNC",    AT_ADDR,    AT_NONE,    0xd2 },
    { "JNZ",    AT_ADDR,    AT_NONE,    0xc2 },
    { "JZ",     AT_ADDR,    AT_NONE,    0xca },
    { "LDA",    AT_ADDR,    AT_NONE,    0x3a },
    { "LXI",    AT_REG_SP,  AT_INT,     0x31 },
    { "MOV",    AT_REG_A,   AT_REG_L,   0x7d },
    { "MOV",    AT_REG_A,   AT_MEM,     0x7e },
    { "MOV",    AT_REG_L,   AT_REG_A,   0x6f },
    { "MOV",    AT_REG_L,   AT_MEM,     0x6e },
    { "MOV",    AT_MEM,     AT_REG_A,   0x77 },
    { "MVI",    AT_REG_A,   AT_INT,     0x3e },
    { "MVI",    AT_REG_L,   AT_INT,     0x2e },
    { "NOP",    AT_NONE,    AT_NONE,    0x00 },
    { "ORA",    AT_REG_A,   AT_NONE,    0xb7 },
    { "ORA",    AT_REG_L,   AT_NONE,    0xb5 },
    { "ORA",    AT_MEM,     AT_NONE,    0xb6 },
    { "ORI",    AT_INT,     AT_NONE,    0xf6 },
    { "OUT",    AT_ADDR,    AT_NONE,    0xd3 },
    { "POP",    AT_REG_A,   AT_NONE,    0xf1 },
    { "POP",    AT_REG_L,   AT_NONE,    0xe1 },
    { "POP",    AT_REG_FL,  AT_NONE,    0xfd },
    { "PUSH",   AT_REG_A,   AT_NONE,    0xf5 },
    { "PUSH",   AT_REG_L,   AT_NONE,    0xe5 },
    { "PUSH",   AT_REG_FL,  AT_NONE,    0xed },
    { "RET",    AT_NONE,    AT_NONE,    0xc9 },
    { "STA",    AT_ADDR,    AT_NONE,    0x32 },
    { "SUB",    AT_REG_A,   AT_NONE,    0x97 },
    { "SUB",    AT_REG_L,   AT_NONE,    0x95 },
    { "SUB",    AT_MEM,     AT_NONE,    0x96 },
    { "SUI",    AT_INT,     AT_NONE,    0xd6 },
    { "XRA",    AT_REG_A,   AT_NONE,    0xaf },
    { "XRA",    AT_REG_L,   AT_NONE,    0xad },
    { "XRA",    AT_MEM,     AT_NONE,    0xae },
    { "XRI",    AT_INT,     AT_NONE,    0xee },
};

#endif /* INSTRUCTIONSET_H */
