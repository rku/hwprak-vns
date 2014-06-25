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

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>

#include "instructionset.h"

#include "utils.h"
#include "vnsasm.h"

vnsasm_configuration config;

void yyerror(char *error)
{
    fprintf(stderr, "%s near line %i\n", error, yylineno);
    exit(EXIT_FAILURE);
}

void write_program()
{
    uint8_t *start, *end;
    size_t size = MEMORY_UNIT_SIZE;
    FILE *outfile = fopen(config.outfile_name, "w+");

    if(NULL == outfile) {
        perror(config.outfile_name);
        exit(EXIT_FAILURE);
    }

    if(config.strip_trailing_zeros && size) {
        start = (uint8_t*)config.program;
        for(end = start + size - 1; *end == 0 && end != start; --end) --size;
    }

    if(1 != fwrite((void*)config.program, size, 1, outfile)) {
        perror(config.outfile_name);
        exit(EXIT_FAILURE);
    }

    fclose(outfile);
}

void write_byte(uint8_t byte)
{
    config.program->data[config.program->counter] = byte;
    ++(config.program->counter);
}

void prc_ins(char *mnemonic, argtype at1, argtype at2, uint8_t iarg)
{
    vns_instruction *ins = is_find_mnemonic(mnemonic, at1, at2);

    if(NULL == ins) {
        fprintf(stderr, "Error near line %i: ", yylineno);
        fprintf(stderr, "Unknown instruction '%s' or ", mnemonic);
        fprintf(stderr, "invalid argument types!");
        exit(EXIT_FAILURE);
    }

    write_byte(ins->opcode);

    if((ins->at1 & AT_INT) || (ins->at2 & AT_INT)) {
        write_byte(iarg);
    }
}

void prc_offset(uint8_t offset)
{
    config.program->counter = offset;
}

int compile(void)
{
    if(NULL == (yyin = fopen(config.infile_name, "r"))) {
        perror(config.infile_name);
        return EXIT_FAILURE;
    }

    fprintf(stdout, "Compiling %s into %s...\n",
            util_basename(config.infile_name),
            util_basename(config.outfile_name));

    if(0 != yyparse()) {
        return EXIT_FAILURE;
    }

    fclose(yyin);
    write_program();

    return EXIT_SUCCESS;
}

void print_usage(char *pname)
{
    printf("\nUsage: %s [-hvz] [-o <outfile>] <asmfile>\n\n", pname);
    printf("  -h             Show this help text.\n");
    printf("  -v             Turn on verbose output.\n");
    printf("  -o <outfile>   Write compiled program to <outfile>.\n");
    printf("  -z             Do NOT strip trailing zeros.\n");
    printf("\n");
}

int main(int argc, char **argv)
{
    unsigned int opt;
    char *process_name = util_basename(argv[0]);
    vnsasm_program program;

    printf(BANNER_LINE1, "ASM Compiler");
    printf(BANNER_LINE2, VERSION);

    /* initialize program */
    memset((void*)&program, 0, sizeof(program));

    /* initialize default configuration */
    config.outfile_name = "program.bin";
    config.infile_name = NULL;
    config.verbose_mode = FALSE;
    config.strip_trailing_zeros = TRUE;
    config.program = &program;

    /* parse cmdline arguments */
    while((opt = getopt(argc, argv, "ho:vz")) != -1) {
        switch(opt) {
            case 'h':
                print_usage(process_name);
                return EXIT_SUCCESS;
            case 'v':
                config.verbose_mode = TRUE;
                break;
            case 'o':
                config.outfile_name = strdup(optarg);
                break;
            case 'z':
                config.strip_trailing_zeros = FALSE;
                break;
            default:
                print_usage(process_name);
                return EXIT_SUCCESS;
        }
    }

    if(optind >= argc) {
        print_usage(process_name);
        return EXIT_SUCCESS;
    }

    config.infile_name = strdup(argv[optind]);

    return compile();
}
