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
#include <stdlib.h>
#include <string.h>

#include "instructionset.h"

/**
 * This list must be sorted by mnemonic,arg1,arg2 in ascending order
 * to allow application of binary search algorithms.
 */
static vns_instruction vns_instructionset[] = {
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
    { "DI",     AT_NONE,    AT_NONE,    0xf3 },
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

/**
 * Compare two instructions. The comparison is done for the mnemonic,
 * argtype1 and argtype2 in exactly this order. If one of these elements
 * from *key* match its counterpart from *other*, 0 is returned.
 * A negative or positive value is returned otherwise to indicate if
 * *key* is understood to be less or greater than *other*.
 */
int inscmp(const void *key, const void *other)
{
    int cmp;
    vns_instruction *key_ins = (vns_instruction*)key;
    vns_instruction *other_ins = (vns_instruction*)other;

    if (0 != (cmp = strcasecmp(key_ins->mnemonic, other_ins->mnemonic))) {
        return cmp;
    }

    if (!(key_ins->at1 & other_ins->at1)) {
        return (key_ins->at1 - other_ins->at1);
    }

    if (!(key_ins->at2 & other_ins->at2)) {
        return (key_ins->at2 - other_ins->at2);
    }

    return 0;
}

/**
 * Compare a mnemonic (key) with the mnemonic of an
 * instruction (other).
 */
int mnemonic_name_cmp(const void *key, const void *other)
{
    vns_instruction *ins = (vns_instruction*)other;
    return strcasecmp((char*)key, ins->mnemonic);
}

/**
 * Look up a mnemonic by its name and return 1 if it could
 * be found or 0 otherwise.
 */
int is_lookup_mnemonic_name(const char *str)
{
    size_t ins_size = sizeof(vns_instruction);
    int n = sizeof(vns_instructionset) / ins_size;

    if (NULL == bsearch((void*)str, (void*)&vns_instructionset,
            n, ins_size, mnemonic_name_cmp)) {
        return 0;
    }

    return 1;
}

/**
 * Search for the instruction with given mnemonic and argtypes. Returns a
 * pointer on success or NULL if no such instruction can be found. This
 * lookup is done with binary search in O(log(n)).
 */
vns_instruction *is_find_mnemonic(const char *mnemonic,
                                  argtype at1, argtype at2)
{
    size_t ins_size;
    unsigned int n;
    vns_instruction key, *result;

    key.mnemonic = mnemonic;
    key.at1 = at1;
    key.at2 = at2;

    ins_size = sizeof(vns_instruction);
    n = sizeof(vns_instructionset) / ins_size;

    result = (vns_instruction*)bsearch(
            (void*)&key, (void*)&vns_instructionset,
            n, ins_size, inscmp);

    return result;
}

/**
 * Search for an instruction with the given opcode and return a pointer
 * to it. If it cannot be found, NULL is returned.
 */
vns_instruction *is_find_opcode(uint8_t opcode)
{
    size_t ins_size;
    unsigned int n, i;
    vns_instruction *result;

    /* Since opcode->instruction search is only useful for debugging 
     * purposes, the instruction set is sorted for find_mnemonic to
     * be searchable in O(log(n)), so we have to go trough the whole
     * list in worst cases here. */

    ins_size = sizeof(vns_instruction);
    n = sizeof(vns_instructionset) / ins_size;

    for (i = 0; i < n; ++i) {
        result = &vns_instructionset[i];
        if (result->opcode == opcode) {
            return result;
        }
    }

    return NULL;
}
