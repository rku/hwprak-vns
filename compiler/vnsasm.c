#include "vnsasm.h"

FILE *outfile = NULL;
char *outfile_name = "program.bin";

void yyerror(char *error)
{
    fprintf(stderr, "%s near line %i\n", error, yylineno);
    exit(EXIT_ERROR);
}

void write_byte(uint8_t byte)
{
    fwrite((void*)&byte, 1, 1, outfile);
}

void prc_simple_instr(uint8_t ins)
{
    write_byte(ins);
}

void prc_addr_instr(uint8_t ins, uint8_t addr)
{
    write_byte(ins);
    write_byte(addr);
}

void prc_const_instr(uint8_t ins, uint8_t c)
{
    write_byte(ins);
    write_byte(c);
}

int compile_file(char *filename)
{
    if(NULL == (yyin = fopen(filename, "r"))) {
        perror(filename);
        return EXIT_ERROR;
    }

    if(NULL == (outfile = fopen(outfile_name, "w+"))) {
        perror(outfile_name);
        return EXIT_ERROR;
    }

    fprintf(stdout, "Compiling %s into %s...\n", filename, outfile_name);
    if(0 != yyparse()) {
        return EXIT_ERROR;
    }

    fclose(yyin);
    fclose(outfile);

    return EXIT_OK;
}

int main(int argc, char **argv)
{
    fprintf(stdout, "TUD HWPRAK Von-Neumann Simulator - ASM Compiler\n");

    if(argc < 2) {
        fprintf(stderr, "Usage: %s <asmfile>\n", argv[0]);
        return EXIT_OK;
    }

    return compile_file(argv[1]);
}
