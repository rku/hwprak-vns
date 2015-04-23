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

%{
#include "vnsasm.h"
#include "instructionset.h"

int yylex (void);
void yyerror(char*);
%}

%union {
    uint8_t ival;
    char *sval;
}

%token <ival> TOK_INT;
%token <ival> TOK_ARG;
%token <sval> TOK_INS;
%token <sval> TOK_ID;

%token TOK_BYTE;
%token TOK_OFFSET;
%token TOK_NEWL;
%token TOK_UNKNOWN;

%%

asnasmfile
     :
     | asnasmfile line
     ;

line
    : TOK_NEWL
    | line instruction TOK_NEWL
    | line TOK_ID ':' { prc_label_decl($2); } instruction TOK_NEWL
    | TOK_ID ':'      { prc_label_decl($1); } instruction TOK_NEWL
    | instruction TOK_NEWL
    ;

instruction
    : asm_command
    | TOK_ID ':'        { prc_label_decl($1);                     }
    | TOK_INS TOK_ARG   { prc_ins($1, $2, AT_NONE, 0, NULL);      }
    | TOK_INS TOK_INT   { prc_ins($1, AT_INT, AT_NONE, $2, NULL); }
    | TOK_INS TOK_ID    { prc_ins($1, AT_LABEL, AT_NONE, 0, $2);  }
    | TOK_INS TOK_ARG ',' TOK_INT {
                          prc_ins($1, $2, AT_INT, $4, NULL);
                        }
    | TOK_INS TOK_ARG ',' TOK_ARG {
                          prc_ins($1, $2, $4, 0, NULL);
                        }
    | TOK_INS TOK_ARG ',' TOK_ID  {
                          prc_ins($1, $2, AT_LABEL, 0, $4);
                        }
    | TOK_INS           { prc_ins($1, AT_NONE, AT_NONE, 0, NULL); }
    ;

asm_command
    : offset
    | byte
    ;

offset
    : TOK_OFFSET TOK_INT    { prc_offset($2); }
    ;

byte
    : TOK_BYTE TOK_INT      { prc_byte($2); }
    | byte ',' TOK_INT      { prc_byte($3); }
    ;

%%
