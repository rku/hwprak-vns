/* lexer */

%{
#include <stdlib.h>
#include <stdint.h>
#include "vnsasm.tab.h"
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

,                   { return TOK_SEP;  }

A                   { return TOK_A;    }
L                   { return TOK_L;    }
M                   { return TOK_M;    }
FL                  { return TOK_FL;   }
SP                  { return TOK_SP;   }

MOV                 { return TOK_MOV;  }
MVI                 { return TOK_MVI;  }
LDA                 { return TOK_LDA;  }
STA                 { return TOK_STA;  }
LXI                 { return TOK_LXI;  }
PUSH                { return TOK_PUSH; }
POP                 { return TOK_POP;  }
IN                  { return TOK_IN;   }
OUT                 { return TOK_OUT;  }
INR                 { return TOK_INR;  }
DCR                 { return TOK_DCR;  }
ADD                 { return TOK_ADD;  }
ADI                 { return TOK_ADI;  }
SUB                 { return TOK_SUB;  }
SUI                 { return TOK_SUI;  }
CMP                 { return TOK_CMP;  }
CPI                 { return TOK_CPI;  }
ANA                 { return TOK_ANA;  }
ANI                 { return TOK_ANI;  }
ORA                 { return TOK_ORA;  }
ORI                 { return TOK_ORI;  }
XRA                 { return TOK_XRA;  }
XRI                 { return TOK_XRI;  }
JMP                 { return TOK_JMP;  }
JZ                  { return TOK_JZ;   }
JNZ                 { return TOK_JNZ;  }
JC                  { return TOK_JC;   }
JNC                 { return TOK_JNC;  }
CALL                { return TOK_CALL; }
CZ                  { return TOK_CZ;   }
CNZ                 { return TOK_CNZ;  }
CC                  { return TOK_CC;   }
CNC                 { return TOK_CNC;  }
RET                 { return TOK_RET;  }
HLT                 { return TOK_HLT;  }
NOP                 { return TOK_NOP;  }
EI                  { return TOK_EI;   }
DI                  { return TOK_DI;   }

.|\n                {}

%%
