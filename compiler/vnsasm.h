#ifndef VNSASM_H
#define VNSASM_H 1

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define EXIT_OK     0
#define EXIT_ERROR  1

#ifndef TRUE
#define TRUE        1
#endif

#ifndef FALSE
#define FALSE       0
#endif

void yyerror(char *error);
int yyparse(void);

void prc_simple_instr(uint8_t ins);
void prc_addr_instr(uint8_t ins, uint8_t addr);
void prc_const_instr(uint8_t ins, uint8_t c);

extern FILE *yyin;
extern int yylineno;

#endif /* VNSASM_H */
