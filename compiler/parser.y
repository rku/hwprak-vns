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
%token <sval> TOK_TXT;
%token <sval> TOK_LABELDECL;

%token TOK_SEP;
%token TOK_OFFSET;

%%

asnasmfile
     : %empty
     | asnasmfile line
     ;

line
    : line instruction
    | line TOK_LABELDECL { prc_label_declaration($2); } instruction
    | TOK_LABELDECL      { prc_label_declaration($1); } instruction
    | instruction
    ;

instruction
    : asm_command
    | TOK_TXT TOK_ARG           { prc_ins($1, $2, AT_NONE, 0, NULL);       }
    | TOK_TXT TOK_INT           { prc_ins($1, AT_INT, AT_NONE, $2, NULL);  }
    | TOK_TXT TOK_TXT           { prc_ins($1, AT_LABEL, AT_NONE, 0, $2);   }
    | TOK_TXT TOK_ARG TOK_SEP TOK_INT { prc_ins($1, $2, AT_INT, $4, NULL); }
    | TOK_TXT TOK_ARG TOK_SEP TOK_ARG { prc_ins($1, $2, $4, 0, NULL);      }
    | TOK_TXT TOK_ARG TOK_SEP TOK_TXT { prc_ins($1, $2, AT_LABEL, 0, $4);  }
    | TOK_TXT                   { prc_ins($1, AT_NONE, AT_NONE, 0, NULL);  }
    ;

asm_command
    : TOK_OFFSET TOK_INT        { prc_offset($2); }
    ;

%%
