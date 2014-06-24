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
