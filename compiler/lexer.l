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
#include "grammar.tab.h"
%}

%option nounput
%option noinput
%option yylineno
%option outfile="lexer.c"

DECNUM              [0-9]+
HEXNUM              0[xX][[:xdigit:]]+

%%

{DECNUM}            {
                        yylval.ival = atoi(yytext);
                        return TOK_INT;
                    }
{HEXNUM}            {
                        yylval.ival = strtol(yytext, NULL, 16);
                        return TOK_INT;
                    }

;.*\n               {  /* comment */     }

\.(?i:offset)       { return TOK_OFFSET; }

,                   { return TOK_SEP;    }

(?i:A)              { return TOK_A;      }
(?i:L)              { return TOK_L;      }
(?i:M)              { return TOK_M;      }
(?i:FL)             { return TOK_FL;     }
(?i:SP)             { return TOK_SP;     }

(?i:MOV)            { return TOK_MOV;    }
(?i:MVI)            { return TOK_MVI;    }
(?i:LDA)            { return TOK_LDA;    }
(?i:STA)            { return TOK_STA;    }
(?i:LXI)            { return TOK_LXI;    }
(?i:PUSH)           { return TOK_PUSH;   }
(?i:POP)            { return TOK_POP;    }
(?i:IN)             { return TOK_IN;     }
(?i:OUT)            { return TOK_OUT;    }
(?i:INR)            { return TOK_INR;    }
(?i:DCR)            { return TOK_DCR;    }
(?i:ADD)            { return TOK_ADD;    }
(?i:ADI)            { return TOK_ADI;    }
(?i:SUB)            { return TOK_SUB;    }
(?i:SUI)            { return TOK_SUI;    }
(?i:CMP)            { return TOK_CMP;    }
(?i:CPI)            { return TOK_CPI;    }
(?i:ANA)            { return TOK_ANA;    }
(?i:ANI)            { return TOK_ANI;    }
(?i:ORA)            { return TOK_ORA;    }
(?i:ORI)            { return TOK_ORI;    }
(?i:XRA)            { return TOK_XRA;    }
(?i:XRI)            { return TOK_XRI;    }
(?i:JMP)            { return TOK_JMP;    }
(?i:JZ)             { return TOK_JZ;     }
(?i:JNZ)            { return TOK_JNZ;    }
(?i:JC)             { return TOK_JC;     }
(?i:JNC)            { return TOK_JNC;    }
(?i:CALL)           { return TOK_CALL;   }
(?i:CZ)             { return TOK_CZ;     }
(?i:CNZ)            { return TOK_CNZ;    }
(?i:CC)             { return TOK_CC;     }
(?i:CNC)            { return TOK_CNC;    }
(?i:RET)            { return TOK_RET;    }
(?i:HLT)            { return TOK_HLT;    }
(?i:NOP)            { return TOK_NOP;    }
(?i:EI)             { return TOK_EI;     }
(?i:DI)             { return TOK_DI;     }

.|\n                {}

%%
