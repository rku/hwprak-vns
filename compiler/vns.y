/* grammar */

%{
#include "vnsasm.h"

int yylex (void);
void yyerror(char*);

%}

%union {
    uint8_t ival;
}

%token <ival> TOK_N
%token <ival> TOK_ADDR

%token TOK_SEP;

%left TOK_A;
%left TOK_L;
%left TOK_M;
%left TOK_FL;

%token TOK_MOV;
%token TOK_MVI;
%token TOK_LDA;
%token TOK_STA;
%token TOK_LXI;
%token TOK_PUSH;
%token TOK_POP;
%token TOK_IN;
%token TOK_OUT;
%token TOK_INR;
%token TOK_DCR;
%token TOK_ADD;
%token TOK_ADI;
%token TOK_SUB;
%token TOK_SUI;
%token TOK_CMP;
%token TOK_CPI;
%token TOK_ANA;
%token TOK_ANI;
%token TOK_ORA;
%token TOK_ORI;
%token TOK_XRA;
%token TOK_XRI;
%token TOK_JMP;
%token TOK_JZ;
%token TOK_JNZ;
%token TOK_JC;
%token TOK_JNC;
%token TOK_CALL;
%token TOK_CZ;
%token TOK_CNZ;
%token TOK_CC;
%token TOK_CNC;
%token TOK_RET;
%token TOK_HLT;
%token TOK_NOP;
%token TOK_EI;
%token TOK_DI;

%%

asnasmfile:
     %empty
     | asnasmfile line
     ;

line:
    line block
  | block
    ;

block:
    instruction
    ;

instruction:
    ins_mov
    ;

ins_mov:
      TOK_MOV TOK_A TOK_SEP TOK_L       { prc_simple_instr(0x7E);   }
    | TOK_MOV TOK_A TOK_SEP TOK_ADDR    { prc_addr_instr(0x2E, $4); }
    | TOK_MOV TOK_A                     { prc_simple_instr(0x4E);   }
    ;

%%

