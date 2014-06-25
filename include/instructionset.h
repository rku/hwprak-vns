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

vns_instruction instructionset[] = {
    /* TRANSFER */
    { "MOV",    AT_REG_A,   AT_REG_L,   0x7d },
    { "MOV",    AT_REG_A,   AT_MEM,     0x7e },
    { "MOV",    AT_REG_L,   AT_REG_A,   0x6f },
    { "MOV",    AT_REG_L,   AT_MEM,     0x6e },
    { "MOV",    AT_MEM,     AT_REG_A,   0x77 },
    { "MVI",    AT_REG_A,   AT_INT,     0x3e },
    { "MVI",    AT_REG_L,   AT_INT,     0x2e },
    { "LDA",    AT_ADDR,    AT_NONE,    0x3a },
    { "STA",    AT_ADDR,    AT_NONE,    0x32 },
    { "LXI",    AT_REG_SP,  AT_INT,     0x31 },
    { "PUSH",   AT_REG_A,   AT_NONE,    0xf5 },
    { "PUSH",   AT_REG_L,   AT_NONE,    0xe5 },
    { "PUSH",   AT_REG_FL,  AT_NONE,    0xed },
    { "POP",    AT_REG_A,   AT_NONE,    0xf1 },
    { "POP",    AT_REG_L,   AT_NONE,    0xe1 },
    { "POP",    AT_REG_FL,  AT_NONE,    0xfd },
    { "IN",     AT_ADDR,    AT_NONE,    0xdb },
    { "OUT",    AT_ADDR,    AT_NONE,    0xd3 },
    /* ARITHMETIC */
    { "INR",    AT_REG_A,   AT_NONE,    0x3c },
    { "INR",    AT_REG_L,   AT_NONE,    0x2c },
    { "DCR",    AT_REG_A,   AT_NONE,    0x3d },
    { "DCR",    AT_REG_L,   AT_NONE,    0x2d },
    { "ADD",    AT_REG_A,   AT_NONE,    0x87 },
    { "ADD",    AT_REG_L,   AT_NONE,    0x85 },
    { "ADD",    AT_MEM,     AT_NONE,    0x86 },
    { "ADI",    AT_INT,     AT_NONE,    0xc6 },
    { "SUB",    AT_REG_A,   AT_NONE,    0x97 },
    { "SUB",    AT_REG_L,   AT_NONE,    0x95 },
    { "SUB",    AT_MEM,     AT_NONE,    0x96 },
    { "SUI",    AT_INT,     AT_NONE,    0xd6 },
    { "CMP",    AT_REG_A,   AT_NONE,    0xbf },
    { "CMP",    AT_REG_L,   AT_NONE,    0xbd },
    { "CMP",    AT_MEM,     AT_NONE,    0xbe },
    { "CPI",    AT_INT,     AT_NONE,    0xfe },
    /* LOGIC */
    { "ANA",    AT_REG_A,   AT_NONE,    0xa7 },
    { "ANA",    AT_REG_L,   AT_NONE,    0xa5 },
    { "ANA",    AT_MEM,     AT_NONE,    0xa6 },
    { "ANI",    AT_INT,     AT_NONE,    0xe6 },
    { "ORA",    AT_REG_A,   AT_NONE,    0xb7 },
    { "ORA",    AT_REG_L,   AT_NONE,    0xb5 },
    { "ORA",    AT_MEM,     AT_NONE,    0xb6 },
    { "ORI",    AT_INT,     AT_NONE,    0xf6 },
    { "XRA",    AT_REG_A,   AT_NONE,    0xaf },
    { "XRA",    AT_REG_L,   AT_NONE,    0xad },
    { "XRA",    AT_MEM,     AT_NONE,    0xae },
    { "XRI",    AT_INT,     AT_NONE,    0xee },
    /* BRANCH */
    { "JMP",    AT_ADDR,    AT_NONE,    0xc3 },
    { "JZ",     AT_ADDR,    AT_NONE,    0xca },
    { "JNZ",    AT_ADDR,    AT_NONE,    0xc2 },
    { "JC",     AT_ADDR,    AT_NONE,    0xda },
    { "JNC",    AT_ADDR,    AT_NONE,    0xd2 },
    { "CALL",   AT_ADDR,    AT_NONE,    0xcd },
    { "CZ",     AT_ADDR,    AT_NONE,    0xcc },
    { "CNZ",    AT_ADDR,    AT_NONE,    0xc4 },
    { "CC",     AT_ADDR,    AT_NONE,    0xdc },
    { "CNC",    AT_ADDR,    AT_NONE,    0xd4 },
    { "RET",    AT_NONE,    AT_NONE,    0xc9 },
    /* SPECIAL */
    { "HLT",    AT_NONE,    AT_NONE,    0x76 },
    { "NOP",    AT_NONE,    AT_NONE,    0x00 },
    { "EI",     AT_NONE,    AT_NONE,    0xfb },
    { "DI",     AT_NONE,    AT_NONE,    0xf3 }
};

#endif /* INSTRUCTIONSET_H */
