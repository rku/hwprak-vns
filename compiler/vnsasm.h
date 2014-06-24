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

#ifndef VNSASM_H
#define VNSASM_H 1

#include <stdio.h>
#include <stdint.h>

typedef struct _vnsasm_configuration {
    FILE *outfile_d;
    char *outfile_name;
    char *infile_name;
    uint8_t verbose_mode;
} vnsasm_configuration;

void yyerror(char *error);
int yyparse(void);

void prc_smpl_instr(uint8_t ins);
void prc_addr_instr(uint8_t ins, uint8_t addr);
void prc_cons_instr(uint8_t ins, uint8_t c);

extern FILE *yyin;
extern int yylineno;

#endif /* VNSASM_H */
