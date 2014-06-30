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
#include <stdlib.h>
#include <stdint.h>
#include "parser.tab.h"
#include "instructionset.h"
%}

%option nounput
%option noinput
%option nodefault
%option yylineno

DECNUM              [0-9]+
HEXNUM              0[xX][[:xdigit:]]+
SINGLE              [,:]
COMMENT             ;.*\n
TEXT                [A-Z][A-Z0-9_]+
WHITESPACE          [ \s\t\r\f]

%%

{DECNUM}            {
                        yylval.ival = atoi(yytext);
                        return TOK_INT;
                    }
{HEXNUM}            {
                        yylval.ival = strtol(yytext, NULL, 16);
                        return TOK_INT;
                    }

{SINGLE}            { return yytext[0];  }
{COMMENT}           { return TOK_NEWL;   }
\.(?i:memset)       { return TOK_MEMSET; }
\.(?i:offset)       { return TOK_OFFSET; }

A                   { yylval.ival = AT_REG_A;  return TOK_ARG; }
L                   { yylval.ival = AT_REG_L;  return TOK_ARG; }
M                   { yylval.ival = AT_MEM;    return TOK_ARG; }
FL                  { yylval.ival = AT_REG_FL; return TOK_ARG; }
SP                  { yylval.ival = AT_REG_SP; return TOK_ARG; }

(?i:{TEXT})         {
                        yylval.sval = strdup(yytext);
                        if (is_lookup_mnemonic_name(yylval.sval)) {
                            return TOK_INS;
                        }
                        return TOK_ID;
                    }

\n                  { return TOK_NEWL;       }
{WHITESPACE}+       { /* skip whitespaces */ }

%%
