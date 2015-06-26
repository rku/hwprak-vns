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

#include <stdio.h>
#include <string.h>

#include "unittest.h"
#include "globals.h"
#include "vnsem.h"

unsigned int tests_run = 0;

extern int process_instruction(uint8_t, vnsem_machine*);
extern void dump_memory(vnsem_machine*);
extern void print_machine_state(vnsem_machine*);

// return an initialized machine for testing
inline vnsem_machine _get_machine(vnsem_machine *other)
{
    vnsem_machine m;

    if (other == NULL) {
        memset(&m, 0, sizeof(m));
    } else {
        memcpy(&m, other, sizeof(m));
    }

    return m;
}

#define MACHINES_EQUAL(x, y) (memcmp(&x, &y, sizeof(vnsem_machine)) == 0)
/* #define P(x,y) do { \
   print_machine_state(&x); \
   print_machine_state(&y); } while(0); */

/* ------------------------------------------------------------------------
 *                            test instructions
 * ------------------------------------------------------------------------ */

TEST(test_ins_mov_a_l)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.reg_l = 23;

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu = 23;

    process_instruction(0x7d, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "MOV A,L instruction failed!");

    return TEST_OK;
}

TEST(test_ins_mov_a_m)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.mem[42] = 0xed;
    m1.reg_l = 42;

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu = 0xed;

    process_instruction(0x7e, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "MOV A,M instruction failed!");

    return TEST_OK;
}

TEST(test_ins_mov_m_a)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 0xaa;
    m1.reg_l = 42;

    vnsem_machine m2 = _get_machine(&m1);
    m2.mem[42] = 0xaa;

    process_instruction(0x77, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "MOV M,A instruction failed!");

    return TEST_OK;
}

TEST(test_ins_mvi_a_n)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.mem[0] = 42;

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu = 42;
    m2.pc++;

    process_instruction(0x3e, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "MOV A,N instruction failed!");

    return TEST_OK;
}

TEST(test_ins_lda_adr)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.mem[23] = 0xaf;
    m1.mem[0] = 23;

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu = 0xaf;
    m2.pc++;

    process_instruction(0x3a, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "LDA adr instruction failed!");

    return TEST_OK;
}

TEST(test_ins_sta_adr)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.mem[0] = 23;
    m1.accu = 0xaf;

    vnsem_machine m2 = _get_machine(&m1);
    m2.mem[23] = 0xaf;
    m2.pc++;

    process_instruction(0x32, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "STA adr instruction failed!");

    return TEST_OK;
}

TEST(test_ins_mov_l_a)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 23;

    vnsem_machine m2 = _get_machine(&m1);
    m2.reg_l = 23;

    process_instruction(0x6f, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "MOV L,A instruction failed!");

    return TEST_OK;
}

TEST(test_ins_mov_l_m)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.reg_l = 42;
    m1.mem[42] = 23;

    vnsem_machine m2 = _get_machine(&m1);
    m2.reg_l = 23;

    process_instruction(0x6e, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "MOV L,M instruction failed!");

    return TEST_OK;
}

TEST(test_ins_mvi_l_n)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.mem[0] = 23;

    vnsem_machine m2 = _get_machine(&m1);
    m2.pc++;
    m2.reg_l = 23;

    process_instruction(0x2e, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "MVI L,n instruction failed!");

    return TEST_OK;
}

TEST(test_ins_lxi_sp_n)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.mem[0] = 0xff;

    vnsem_machine m2 = _get_machine(&m1);
    m2.pc++;
    m2.sp = 0xff;

    process_instruction(0x31, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "LXI SP,n instruction failed!");

    return TEST_OK;
}

TEST(test_ins_push_a)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 23;
    m1.sp = 0xff;

    vnsem_machine m2 = _get_machine(&m1);
    m2.sp--;
    m2.mem[0xfe] = 23;

    process_instruction(0xf5, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "PUSH A instruction failed!");

    return TEST_OK;
}

TEST(test_ins_push_l)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.reg_l = 23;
    m1.sp = 0xff;

    vnsem_machine m2 = _get_machine(&m1);
    m2.sp--;
    m2.mem[0xfe] = 23;

    process_instruction(0xe5, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "PUSH L instruction failed!");

    return TEST_OK;
}

TEST(test_ins_push_fl)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.flags = 0xff;
    m1.sp = 0xff;

    vnsem_machine m2 = _get_machine(&m1);
    m2.sp--;
    m2.mem[0xfe] = 0xff;

    process_instruction(0xed, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "PUSH FL instruction failed!");

    return TEST_OK;
}

TEST(test_ins_pop_a)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.sp = 0xfe;
    m1.mem[0xfe] = 23;

    vnsem_machine m2 = _get_machine(&m1);
    m2.sp++;
    m2.accu = 23;

    process_instruction(0xf1, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "POP A instruction failed!");

    return TEST_OK;
}

TEST(test_ins_pop_l)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.sp = 0xfe;
    m1.mem[0xfe] = 23;

    vnsem_machine m2 = _get_machine(&m1);
    m2.sp++;
    m2.reg_l = 23;

    process_instruction(0xe1, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "POP L instruction failed!");

    return TEST_OK;
}

TEST(test_ins_pop_fl)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.sp = 0xfe;
    m1.mem[0xfe] = 23;

    vnsem_machine m2 = _get_machine(&m1);
    m2.sp++;
    m2.flags = 23;

    process_instruction(0xfd, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "POP FL instruction failed!");

    return TEST_OK;
}

TEST(test_ins_inr_a)
{
    vnsem_machine m1 = _get_machine(NULL);

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu++;

    process_instruction(0x3c, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "INR A instruction failed!");

    return TEST_OK;
}

TEST(test_ins_inr_l)
{
    vnsem_machine m1 = _get_machine(NULL);

    vnsem_machine m2 = _get_machine(&m1);
    m2.reg_l++;

    process_instruction(0x2c, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "INR L instruction failed!");

    return TEST_OK;
}

TEST(test_ins_dcr_a)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 23;

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu--;

    process_instruction(0x3d, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "DCR A instruction failed!");

    return TEST_OK;
}

TEST(test_ins_dcr_l)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.reg_l = 23;

    vnsem_machine m2 = _get_machine(&m1);
    m2.reg_l--;

    process_instruction(0x2d, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "DCR L instruction failed!");

    return TEST_OK;
}

TEST(test_ins_add_a)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 21;

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu = 42;

    process_instruction(0x87, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "ADD A instruction failed!");

    return TEST_OK;
}

TEST(test_ins_add_l)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 21;
    m1.reg_l = 21;

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu = 42;

    process_instruction(0x85, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "ADD L instruction failed!");

    return TEST_OK;
}

TEST(test_ins_add_m)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 21;
    m1.reg_l = 23;
    m1.mem[23] = 21;

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu = 42;

    process_instruction(0x86, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "ADD M instruction failed!");

    return TEST_OK;
}

TEST(test_ins_adi_n)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 21;
    m1.mem[0] = 21;

    vnsem_machine m2 = _get_machine(&m1);
    m2.pc++;
    m2.accu = 42;

    process_instruction(0xc6, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "ADI n instruction failed!");

    return TEST_OK;
}

TEST(test_ins_sub_a)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 42;

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu = 0;
    m2.flags = F_ZERO;

    process_instruction(0x97, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "SUB A instruction failed!");

    return TEST_OK;
}

TEST(test_ins_sub_l)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 42;
    m1.reg_l = 19;

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu = 23;

    process_instruction(0x95, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "SUB L instruction failed!");

    return TEST_OK;
}

TEST(test_ins_sub_m)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 42;
    m1.reg_l = 23;
    m1.mem[23] = 19;

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu = 23;

    process_instruction(0x96, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "SUB M instruction failed!");

    return TEST_OK;
}

TEST(test_ins_sui_n)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 42;
    m1.mem[0] = 19;

    vnsem_machine m2 = _get_machine(&m1);
    m2.pc++;
    m2.accu = 23;

    process_instruction(0xd6, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "SUI n instruction failed!");

    return TEST_OK;
}

TEST(test_ins_cmp_a)
{
    // ==
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 42;

    vnsem_machine m2 = _get_machine(&m1);
    m2.flags = F_ZERO;

    process_instruction(0xbf, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "CMP A instruction failed!");

    return TEST_OK;
}

TEST(test_ins_cmp_l)
{
    // A < L
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 19;
    m1.reg_l = 23;

    vnsem_machine m2 = _get_machine(&m1);
    m2.flags = F_SIGN | F_CARRY;

    // A == L
    vnsem_machine m3 = _get_machine(NULL);
    m3.accu = 23;
    m3.reg_l = 23;

    vnsem_machine m4 = _get_machine(&m3);
    m4.flags = F_ZERO;

    // A > L
    vnsem_machine m5 = _get_machine(NULL);
    m5.accu = 23;
    m5.reg_l = 19;

    vnsem_machine m6 = _get_machine(&m5);
    m6.flags = F_NONE;

    process_instruction(0xbd, &m1);
    process_instruction(0xbd, &m3);
    process_instruction(0xbd, &m5);

    ASSERT(MACHINES_EQUAL(m1, m2) &&
           MACHINES_EQUAL(m3, m4) &&
           MACHINES_EQUAL(m5, m6),
           "CMP L instruction failed!");

    return TEST_OK;
}

TEST(test_ins_cmp_m)
{
    // A < M
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 19;
    m1.reg_l = 23;
    m1.mem[23] = 42;

    vnsem_machine m2 = _get_machine(&m1);
    m2.flags = F_SIGN | F_CARRY;

    // A == M
    vnsem_machine m3 = _get_machine(NULL);
    m3.accu = 23;
    m3.reg_l = 42;
    m3.mem[m3.reg_l] = 23;

    vnsem_machine m4 = _get_machine(&m3);
    m4.flags = F_ZERO;

    // A > M
    vnsem_machine m5 = _get_machine(NULL);
    m5.accu = 42;
    m5.reg_l = 23;
    m5.mem[m5.reg_l] = 23;

    vnsem_machine m6 = _get_machine(&m5);
    m6.flags = F_NONE;

    process_instruction(0xbe, &m1);
    process_instruction(0xbe, &m3);
    process_instruction(0xbe, &m5);

    ASSERT(MACHINES_EQUAL(m1, m2) &&
           MACHINES_EQUAL(m3, m4) &&
           MACHINES_EQUAL(m5, m6),
           "CMP M instruction failed!");

    return TEST_OK;
}

TEST(test_ins_cpi_n)
{
    // A < n
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 23;
    m1.mem[0] = 42;

    vnsem_machine m2 = _get_machine(&m1);
    m2.pc++;
    m2.flags = F_SIGN | F_CARRY;

    // A == n
    vnsem_machine m3 = _get_machine(NULL);
    m3.accu = 42;
    m3.mem[0] = 42;

    vnsem_machine m4 = _get_machine(&m3);
    m4.pc++;
    m4.flags = F_ZERO;

    // A > n
    vnsem_machine m5 = _get_machine(NULL);
    m5.accu = 42;
    m5.mem[0] = 23;

    vnsem_machine m6 = _get_machine(&m5);
    m6.pc++;
    m6.flags = F_NONE;

    process_instruction(0xfe, &m1);
    process_instruction(0xfe, &m3);
    process_instruction(0xfe, &m5);

    ASSERT(MACHINES_EQUAL(m1, m2) &&
           MACHINES_EQUAL(m3, m4) &&
           MACHINES_EQUAL(m5, m6),
           "CPI n instruction failed!");

    return TEST_OK;
}

TEST(test_ins_ana_a)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 23;

    vnsem_machine m2 = _get_machine(&m1);

    process_instruction(0xa7, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "ANA A instruction failed!");

    return TEST_OK;
}

TEST(test_ins_ana_l)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 23;
    m1.reg_l = 12;

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu = 23 & 12;

    process_instruction(0xa5, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "ANA L instruction failed!");

    return TEST_OK;
}

TEST(test_ins_ana_m)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 23;
    m1.reg_l = 42;
    m1.mem[42] = 21;

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu = 23 & 21;

    process_instruction(0xa6, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "ANA M instruction failed!");

    return TEST_OK;
}

TEST(test_ins_ani_n)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 42;
    m1.mem[0] = 23;

    vnsem_machine m2 = _get_machine(&m1);
    m2.pc++;
    m2.accu = 42 & 23;

    process_instruction(0xe6, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "ANI n instruction failed!");

    return TEST_OK;
}

TEST(test_ins_ora_a)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 23;

    vnsem_machine m2 = _get_machine(&m1);

    process_instruction(0xb7, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "ORA A instruction failed!");

    return TEST_OK;
}

TEST(test_ins_ora_l)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 23;
    m1.reg_l = 32;

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu = 23 | 32;

    process_instruction(0xb5, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "ORA L instruction failed!");

    return TEST_OK;
}

TEST(test_ins_ora_m)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 23;
    m1.reg_l = 42;
    m1.mem[42] = 21;

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu = 23 | 21;

    process_instruction(0xb6, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "ORA M instruction failed!");

    return TEST_OK;
}

TEST(test_ins_ori_n)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 42;
    m1.mem[0] = 23;

    vnsem_machine m2 = _get_machine(&m1);
    m2.pc++;
    m2.accu = 42 | 23;

    process_instruction(0xf6, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "ORI n instruction failed!");

    return TEST_OK;
}

TEST(test_ins_xra_a)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 23;

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu = 0;
    m2.flags = F_ZERO;

    process_instruction(0xaf, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "XRA A instruction failed!");

    return TEST_OK;
}

TEST(test_ins_xra_l)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 23;
    m1.reg_l = 32;

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu = 23 ^ 32;

    process_instruction(0xad, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "XRA L instruction failed!");

    return TEST_OK;
}

TEST(test_ins_xra_m)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 23;
    m1.reg_l = 42;
    m1.mem[42] = 21;

    vnsem_machine m2 = _get_machine(&m1);
    m2.accu = 23 ^ 21;

    process_instruction(0xae, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "XRA M instruction failed!");

    return TEST_OK;
}

TEST(test_ins_xri_n)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 42;
    m1.mem[0] = 23;

    vnsem_machine m2 = _get_machine(&m1);
    m2.pc++;
    m2.accu = 42 ^ 23;

    process_instruction(0xee, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "XRI n instruction failed!");

    return TEST_OK;
}

TEST(test_ins_jmp_adr)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.mem[0] = 0xaf;

    vnsem_machine m2 = _get_machine(&m1);
    m2.pc = 0xaf;

    process_instruction(0xc3, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "JMP adr instruction failed!");

    return TEST_OK;
}

TEST(test_ins_jz_adr)
{
    // true
    vnsem_machine m1 = _get_machine(NULL);
    m1.flags = F_ZERO;
    m1.mem[0] = 0xaf;

    vnsem_machine m2 = _get_machine(&m1);
    m2.pc++;
    m2.pc = 0xaf;

    // false
    vnsem_machine m3 = _get_machine(&m1);
    m3.flags = F_NONE;

    vnsem_machine m4 = _get_machine(&m3);
    m4.pc++;

    process_instruction(0xca, &m1);
    process_instruction(0xca, &m3);

    ASSERT(MACHINES_EQUAL(m1, m2) &&
           MACHINES_EQUAL(m3, m4),
           "JZ adr instruction failed!");

    return TEST_OK;
}

TEST(test_ins_jnz_adr)
{
    // true
    vnsem_machine m1 = _get_machine(NULL);
    m1.mem[0] = 0xaf;

    vnsem_machine m2 = _get_machine(&m1);
    m2.pc = 0xaf;

    // false
    vnsem_machine m3 = _get_machine(&m1);
    m3.flags = F_ZERO;

    vnsem_machine m4 = _get_machine(&m3);
    m4.pc++;

    process_instruction(0xc2, &m1);
    process_instruction(0xc2, &m3);

    ASSERT(MACHINES_EQUAL(m1, m2) &&
           MACHINES_EQUAL(m3, m4),
           "JNZ adr instruction failed!");

    return TEST_OK;
}

TEST(test_ins_jc_adr)
{
    // true
    vnsem_machine m1 = _get_machine(NULL);
    m1.flags = F_CARRY;
    m1.mem[0] = 0xaf;

    vnsem_machine m2 = _get_machine(&m1);
    m2.pc = 0xaf;

    // false
    vnsem_machine m3 = _get_machine(&m1);
    m3.flags = F_NONE;

    vnsem_machine m4 = _get_machine(&m3);
    m4.pc++;

    process_instruction(0xda, &m1);
    process_instruction(0xda, &m3);

    ASSERT(MACHINES_EQUAL(m1, m2) &&
           MACHINES_EQUAL(m3, m4),
           "JC adr instruction failed!");

    return TEST_OK;
}

TEST(test_ins_jnc_adr)
{
    // true
    vnsem_machine m1 = _get_machine(NULL);
    m1.mem[0] = 0xaf;

    vnsem_machine m2 = _get_machine(&m1);
    m2.pc = 0xaf;

    // false
    vnsem_machine m3 = _get_machine(&m1);
    m3.flags = F_CARRY;

    vnsem_machine m4 = _get_machine(&m3);
    m4.pc++;

    process_instruction(0xd2, &m1);
    process_instruction(0xd2, &m3);

    ASSERT(MACHINES_EQUAL(m1, m2) &&
           MACHINES_EQUAL(m3, m4),
           "JNC adr instruction failed!");

    return TEST_OK;
}

TEST(test_ins_call_adr)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.pc = 0xa;
    m1.mem[m1.pc] = 0xaf;

    vnsem_machine m2 = _get_machine(&m1);
    m2.sp = 0xff;
    m2.mem[m2.sp] = 0xb;
    m2.pc = 0xaf;

    process_instruction(0xcd, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "CALL adr instruction failed!");

    return TEST_OK;
}

TEST(test_ins_cz_adr)
{
    // true
    vnsem_machine m1 = _get_machine(NULL);
    m1.flags = F_ZERO;
    m1.pc = 0xa;
    m1.mem[m1.pc] = 0xaf;

    vnsem_machine m2 = _get_machine(&m1);
    m2.sp = 0xff;
    m2.mem[m2.sp] = 0xb;
    m2.pc = 0xaf;

    // false
    vnsem_machine m3 = _get_machine(&m1);
    m3.flags = F_NONE;

    vnsem_machine m4 = _get_machine(&m3);
    m4.pc++;

    process_instruction(0xcc, &m1);
    process_instruction(0xcc, &m3);

    ASSERT(MACHINES_EQUAL(m1, m2) &&
           MACHINES_EQUAL(m3, m4),
           "CZ adr instruction failed!");

    return TEST_OK;
}

TEST(test_ins_cnz_adr)
{
    // true
    vnsem_machine m1 = _get_machine(NULL);
    m1.pc = 0xa;
    m1.mem[m1.pc] = 0xaf;

    vnsem_machine m2 = _get_machine(&m1);
    m2.sp = 0xff;
    m2.mem[m2.sp] = 0xb;
    m2.pc = 0xaf;

    // false
    vnsem_machine m3 = _get_machine(&m1);
    m3.flags = F_ZERO;

    vnsem_machine m4 = _get_machine(&m3);
    m4.pc++;

    process_instruction(0xc4, &m1);
    process_instruction(0xc4, &m3);

    ASSERT(MACHINES_EQUAL(m1, m2) &&
           MACHINES_EQUAL(m3, m4),
           "CNZ adr instruction failed!");

    return TEST_OK;
}

TEST(test_ins_cc_adr)
{
    // true
    vnsem_machine m1 = _get_machine(NULL);
    m1.flags = F_CARRY;
    m1.pc = 0xa;
    m1.mem[m1.pc] = 0xaf;

    vnsem_machine m2 = _get_machine(&m1);
    m2.sp = 0xff;
    m2.mem[m2.sp] = 0xb;
    m2.pc = 0xaf;

    // false
    vnsem_machine m3 = _get_machine(&m1);
    m3.flags = F_NONE;

    vnsem_machine m4 = _get_machine(&m3);
    m4.pc++;

    process_instruction(0xdc, &m1);
    process_instruction(0xdc, &m3);

    ASSERT(MACHINES_EQUAL(m1, m2) &&
           MACHINES_EQUAL(m3, m4),
           "CC adr instruction failed!");

    return TEST_OK;
}

TEST(test_ins_cnc_adr)
{
    // true
    vnsem_machine m1 = _get_machine(NULL);
    m1.pc = 0xa;
    m1.mem[m1.pc] = 0xaf;

    vnsem_machine m2 = _get_machine(&m1);
    m2.sp = 0xff;
    m2.mem[m2.sp] = 0xb;
    m2.pc = 0xaf;

    // false
    vnsem_machine m3 = _get_machine(&m1);
    m3.flags = F_CARRY;

    vnsem_machine m4 = _get_machine(&m3);
    m4.pc++;

    process_instruction(0xd4, &m1);
    process_instruction(0xd4, &m3);

    ASSERT(MACHINES_EQUAL(m1, m2) &&
           MACHINES_EQUAL(m3, m4),
           "CNC adr instruction failed!");

    return TEST_OK;
}

TEST(test_ins_ret)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.sp = 0xff;
    m1.mem[m1.sp] = 0xa;

    vnsem_machine m2 = _get_machine(&m1);
    m2.pc = 0xa;
    m2.sp = 0x0;

    process_instruction(0xc9, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "RET instruction failed!");

    return TEST_OK;
}

TEST(test_ins_hlt)
{
    vnsem_machine m1 = _get_machine(NULL);

    vnsem_machine m2 = _get_machine(&m1);
    m2.halted = TRUE;

    process_instruction(0x76, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "HLT instruction failed!");

    return TEST_OK;
}


TEST(test_ins_nop)
{
    vnsem_machine m1 = _get_machine(NULL);

    vnsem_machine m2 = _get_machine(&m1);

    process_instruction(0x00, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "NOP instruction failed!");

    return TEST_OK;
}


/* ------------------------------------------------------------------------ */

char *run_tests(void)
{
    RUN_TEST(test_ins_mov_a_l);
    RUN_TEST(test_ins_mov_a_m);
    RUN_TEST(test_ins_mov_m_a);
    RUN_TEST(test_ins_mvi_a_n);
    RUN_TEST(test_ins_lda_adr);
    RUN_TEST(test_ins_sta_adr);
    RUN_TEST(test_ins_mov_l_a);
    RUN_TEST(test_ins_mov_l_m);
    RUN_TEST(test_ins_mvi_l_n);
    RUN_TEST(test_ins_lxi_sp_n);
    RUN_TEST(test_ins_push_a);
    RUN_TEST(test_ins_push_l);
    RUN_TEST(test_ins_push_fl);
    RUN_TEST(test_ins_pop_a);
    RUN_TEST(test_ins_pop_l);
    RUN_TEST(test_ins_pop_fl);
    RUN_TEST(test_ins_inr_a);
    RUN_TEST(test_ins_inr_l);
    RUN_TEST(test_ins_dcr_a);
    RUN_TEST(test_ins_dcr_l);
    RUN_TEST(test_ins_add_a);
    RUN_TEST(test_ins_add_l);
    RUN_TEST(test_ins_add_m);
    RUN_TEST(test_ins_adi_n);
    RUN_TEST(test_ins_sub_a);
    RUN_TEST(test_ins_sub_l);
    RUN_TEST(test_ins_sub_m);
    RUN_TEST(test_ins_sui_n);
    RUN_TEST(test_ins_cmp_a);
    RUN_TEST(test_ins_cmp_l);
    RUN_TEST(test_ins_cmp_m);
    RUN_TEST(test_ins_cpi_n);
    RUN_TEST(test_ins_ana_a);
    RUN_TEST(test_ins_ana_l);
    RUN_TEST(test_ins_ana_m);
    RUN_TEST(test_ins_ani_n);
    RUN_TEST(test_ins_ora_a);
    RUN_TEST(test_ins_ora_l);
    RUN_TEST(test_ins_ora_m);
    RUN_TEST(test_ins_ori_n);
    RUN_TEST(test_ins_xra_a);
    RUN_TEST(test_ins_xra_l);
    RUN_TEST(test_ins_xra_m);
    RUN_TEST(test_ins_xri_n);
    RUN_TEST(test_ins_jmp_adr);
    RUN_TEST(test_ins_jz_adr);
    RUN_TEST(test_ins_jnz_adr);
    RUN_TEST(test_ins_jc_adr);
    RUN_TEST(test_ins_jnc_adr);
    RUN_TEST(test_ins_call_adr);
    RUN_TEST(test_ins_cz_adr);
    RUN_TEST(test_ins_cnz_adr);
    RUN_TEST(test_ins_cc_adr);
    RUN_TEST(test_ins_cnc_adr);
    RUN_TEST(test_ins_ret);
    RUN_TEST(test_ins_hlt);
    RUN_TEST(test_ins_nop);

    return NULL;
}

int main(int argc, char **argv)
{
    char *result = run_tests();
    if (result != NULL) {
        printf("\033[1;31m%s\033[m\n", result);
    } else {
        printf("\033[1;32mALL TESTS PASSED\033[m\n");
    }
    printf("Tests run: %d\n", tests_run);
    return (result) ? -1 : 0;
}
