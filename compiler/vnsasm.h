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

#include "globals.h"
#include "list.h"

#include "instructionset.h"

#define MEMORY_UNIT_SIZE 256

typedef struct _vnsasm_program {
    uint8_t data[MEMORY_UNIT_SIZE];
    uint8_t counter;
    list labels;
} vnsasm_program;

typedef struct _vnsasm_configuration {
    char *outfile_name;
    char *infile_name;
    uint8_t verbose_mode;
    uint8_t strip_trailing_zeros;
    uint8_t print_resolved_labels;
    vnsasm_program *program;
} vnsasm_configuration;

typedef struct _vnsasm_label {
    char *name;
    int addr;
    list positions;
} vnsasm_label;

int yyparse(void);
void yyerror(char *error);

void prc_label_decl(char *name);
void prc_ins(char *mnemonic, argtype at1, argtype at2, uint8_t i, char *s);
void prc_data(uint8_t addr, uint8_t value);
void prc_offset(uint8_t offset);

extern FILE *yyin;
extern int yylineno;

#endif /* VNSASM_H */
