/* grammar */

%{
#include "vnsasm.h"

int yylex (void);
void yyerror(char*);

%}

%union {
    uint8_t ival;
}

%token <ival> TOK_INT

%token TOK_SEP;

%left TOK_A;
%left TOK_L;
%left TOK_M;
%left TOK_FL;
%left TOK_SP;

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
    ins_transfer
    | ins_arithmetic
    | ins_logic
    | ins_branch
    | ins_special
    ;

ins_transfer:
      TOK_MOV TOK_A TOK_SEP TOK_L       { prc_smpl_instr(0x7d);     }
    | TOK_MOV TOK_A TOK_SEP TOK_M       { prc_smpl_instr(0x7e);     }
    | TOK_MOV TOK_M TOK_SEP TOK_A       { prc_smpl_instr(0x77);     }
    | TOK_MOV TOK_L TOK_SEP TOK_A       { prc_smpl_instr(0x6f);     }
    | TOK_MOV TOK_L TOK_SEP TOK_M       { prc_smpl_instr(0x6e);     }
    | TOK_MVI TOK_A TOK_SEP TOK_INT     { prc_cons_instr(0x3e, $4); }
    | TOK_MVI TOK_L TOK_SEP TOK_INT     { prc_cons_instr(0x2e, $4); }
    
    | TOK_LDA TOK_INT                   { prc_addr_instr(0x3a, $2); }
    | TOK_STA TOK_INT                   { prc_addr_instr(0x32, $2); }
    | TOK_LXI TOK_SP TOK_SEP TOK_INT    { prc_cons_instr(0x31, $4); }

    | TOK_PUSH TOK_A                    { prc_smpl_instr(0xf5);     }
    | TOK_PUSH TOK_L                    { prc_smpl_instr(0xe5);     }
    | TOK_PUSH TOK_FL                   { prc_smpl_instr(0xed);     }
    | TOK_POP TOK_A                     { prc_smpl_instr(0xf1);     }
    | TOK_POP TOK_L                     { prc_smpl_instr(0xe1);     }
    | TOK_POP TOK_FL                    { prc_smpl_instr(0xfd);     }

    | TOK_IN TOK_INT                    { prc_addr_instr(0xdb, $2); }
    | TOK_OUT TOK_INT                   { prc_addr_instr(0xd3, $2); }
    ;

ins_arithmetic:
      TOK_INR TOK_A                     { prc_smpl_instr(0x3c);     }
    | TOK_INR TOK_L                     { prc_smpl_instr(0x2c);     }
    | TOK_DCR TOK_A                     { prc_smpl_instr(0x3d);     }
    | TOK_DCR TOK_L                     { prc_smpl_instr(0x2d);     }
    | TOK_ADD TOK_A                     { prc_smpl_instr(0x87);     }
    | TOK_ADD TOK_L                     { prc_smpl_instr(0x85);     }
    | TOK_ADD TOK_M                     { prc_smpl_instr(0x86);     }
    | TOK_ADI TOK_INT                   { prc_cons_instr(0xc6, $2); }
    | TOK_SUB TOK_A                     { prc_smpl_instr(0x97);     }
    | TOK_SUB TOK_L                     { prc_smpl_instr(0x95);     }
    | TOK_SUB TOK_M                     { prc_smpl_instr(0x96);     }
    | TOK_SUI TOK_INT                   { prc_cons_instr(0xd6, $2); }
    | TOK_CMP TOK_A                     { prc_smpl_instr(0xbf);     }
    | TOK_CMP TOK_L                     { prc_smpl_instr(0xbd);     }
    | TOK_CMP TOK_M                     { prc_smpl_instr(0xbe);     }
    | TOK_CPI TOK_INT                   { prc_cons_instr(0xfe, $2); }
    ;

ins_logic:
      TOK_ANA TOK_A                     { prc_smpl_instr(0xa7);     }
    | TOK_ANA TOK_L                     { prc_smpl_instr(0xa5);     }
    | TOK_ANA TOK_M                     { prc_smpl_instr(0xa6);     }
    | TOK_ANI TOK_INT                   { prc_cons_instr(0xe6, $2); }
    | TOK_ORA TOK_A                     { prc_smpl_instr(0xb7);     }
    | TOK_ORA TOK_L                     { prc_smpl_instr(0xb5);     }
    | TOK_ORA TOK_M                     { prc_smpl_instr(0xb6);     }
    | TOK_ORI TOK_INT                   { prc_cons_instr(0xf6, $2); }
    | TOK_XRA TOK_A                     { prc_smpl_instr(0xaf);     }
    | TOK_XRA TOK_L                     { prc_smpl_instr(0xad);     }
    | TOK_XRA TOK_M                     { prc_smpl_instr(0xae);     }
    | TOK_XRI TOK_INT                   { prc_cons_instr(0xee, $2); }
    ;

ins_branch:
      TOK_JMP TOK_INT                   { prc_addr_instr(0xc3, $2); }
    | TOK_JZ TOK_INT                    { prc_addr_instr(0xca, $2); }
    | TOK_JNZ TOK_INT                   { prc_addr_instr(0xc2, $2); }
    | TOK_JC TOK_INT                    { prc_addr_instr(0xda, $2); }
    | TOK_JNC TOK_INT                   { prc_addr_instr(0xd2, $2); }
    | TOK_CALL TOK_INT                  { prc_addr_instr(0xcd, $2); }
    | TOK_CZ TOK_INT                    { prc_addr_instr(0xcc, $2); }
    | TOK_CNZ TOK_INT                   { prc_addr_instr(0xc4, $2); }
    | TOK_CC TOK_INT                    { prc_addr_instr(0xdc, $2); }
    | TOK_CNC TOK_INT                   { prc_addr_instr(0xd4, $2); }
    | TOK_RET                           { prc_smpl_instr(0xc9);     }
    ;

ins_special:
      TOK_HLT                           { prc_smpl_instr(0x76);     }
    | TOK_NOP                           { prc_smpl_instr(0x00);     }
    | TOK_EI                            { prc_smpl_instr(0xfb);     }
    | TOK_DI                            { prc_smpl_instr(0xf3);     }
    ;

%%

