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
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 19;
    m1.reg_l = 23;

    vnsem_machine m2 = _get_machine(&m1);
    m2.flags = F_SIGN | F_CARRY;

    process_instruction(0xbd, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "CMP L instruction failed!");

    return TEST_OK;
}

TEST(test_ins_cmp_m)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 19;
    m1.reg_l = 23;
    m1.mem[23] = 42;

    vnsem_machine m2 = _get_machine(&m1);
    m2.flags = F_SIGN | F_CARRY;

    process_instruction(0xbe, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "CMP M instruction failed!");

    return TEST_OK;
}

TEST(test_ins_cpi_n)
{
    vnsem_machine m1 = _get_machine(NULL);
    m1.accu = 23;
    m1.mem[0] = 42;

    vnsem_machine m2 = _get_machine(&m1);
    m2.pc++;
    m2.flags = F_SIGN | F_CARRY;

    process_instruction(0xfe, &m1);

    ASSERT(MACHINES_EQUAL(m1, m2), "CPI n instruction failed!");

    return TEST_OK;
}

/* ------------------------------------------------------------------------ */

char *run_tests(void) {
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
