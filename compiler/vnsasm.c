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
#include <assert.h>

#include "utils.h"
#include "vnsasm.h"

vnsasm_configuration config;

void yyerror(char *error)
{
    util_perror("%s near line %i\n", error, yylineno);
    exit(EXIT_FAILURE);
}

void write_program(void)
{
    uint8_t *start, *end;
    size_t size = MEMORY_UNIT_SIZE;
    FILE *outfile = fopen(config.outfile_name, "w+");

    if (NULL == outfile) {
        perror(config.outfile_name);
        exit(EXIT_FAILURE);
    }

    if (config.strip_trailing_zeros && size) {
        start = (uint8_t*)config.program;
        for (end = start + size - 1; *end == 0 && end != start; --end) {
            --size;
        }
    }

    if (1 != fwrite((void*)config.program, size, 1, outfile)) {
        perror(config.outfile_name);
        exit(EXIT_FAILURE);
    }

    fclose(outfile);
}

void backpatch_label(vnsasm_label *label)
{
    list_item *pos;
    assert(-1 != label->addr);

    pos = label->positions.head;
    while (NULL != pos) {
        *((uint8_t*)pos->payload) = label->addr;
        pos = pos->next;
    }

    list_destroy(&label->positions);
}

vnsasm_label *find_label(const char *name)
{
    vnsasm_label *label;
    list_item *item = config.program->labels.head;

    while (NULL != item) {
        label = (vnsasm_label*)item->payload;
        if (0 == strcmp(label->name, name)) {
            return label;
        }
        item = item->next;
    }

    return NULL;
}

vnsasm_label* declare_label(const char *name, unsigned int addr)
{
    vnsasm_label *label;

    if (NULL != (label = find_label(name))) {
        /* label exists */
        if ((-1 != addr) && (-1 == label->addr)) {
            /* we've found the declaration for a pending label */
            label->addr = addr;
            backpatch_label(label);
        } else {
            util_perror("Duplicated label declaration "
                        "near line %i: %s\n",
                        yylineno, name);
            exit(EXIT_FAILURE);
        }
    } else {
        label = (vnsasm_label*)malloc(sizeof(vnsasm_label));

        label->name = strdup(name);
        label->addr = addr;

        if (-1 == addr) {
            /* pending label */
            list_init(&label->positions);
        }

        list_insert(&config.program->labels, NULL, label, free);
    }

    return label;
}

void resolve_label(const char *name)
{
    vnsasm_label *label;
    uint8_t *byte = &config.program->data[config.program->counter];

    if (NULL == (label = find_label(name))) {
        /* declare pending label */
        label = declare_label(name, -1);
    }

    if (-1 == label->addr) {
        /* label is pending, store position of current byte */
        list_insert(&label->positions, NULL, (void*)byte, NULL);
    } else {
        *byte = label->addr;
    }
}

void finalize_labels(void)
{
    vnsasm_label *label;
    list_item *item;
 
    for (item = config.program->labels.head; NULL != item; item = item->next) {
        label = (vnsasm_label*)item->payload;
        if (-1 == label->addr) {
            list_destroy(&label->positions);
            util_perror("Could not resolve label: %s\n", label->name);
            exit(EXIT_FAILURE);
        }
        if (config.print_resolved_labels) {
            printf(" -> Label '%s' resolved to address 0x%.2x (%i)\n",
                   label->name, label->addr, label->addr);
        }
    }
}

void push_byte(uint8_t byte)
{
    config.program->data[config.program->counter] = byte;
    config.program->counter++;
}

void skip_byte(void)
{
    config.program->counter++;
}

void prc_label_decl(const char *name)
{
    declare_label(name, config.program->counter);
}

void prc_ins(char *mnemonic, argtype at1, argtype at2, uint8_t i, char *s)
{
    vns_instruction *ins;
    
    util_str_toupper(mnemonic);
    ins = is_find_mnemonic(mnemonic, at1, at2);

    if (NULL == ins) {
        util_perror("near line %i: Unknown instruction '%s' "
                    "or invalid argument types!\n",
                    yylineno, mnemonic);
        exit(EXIT_FAILURE);
    }

    push_byte(ins->opcode);

    if (NULL != s && ((ins->at1 & AT_LABEL) || (ins->at2 & AT_LABEL))) {
        resolve_label(s);
        skip_byte();
    } else
    if ((ins->at1 & AT_INT) || (ins->at2 & AT_INT)) {
        push_byte(i);
    }
}

void prc_byte(uint8_t value)
{
    push_byte(value);
}

void prc_offset(uint8_t offset)
{
    config.program->counter = offset;
}

int compile(void)
{
    if (NULL == (yyin = fopen(config.infile_name, "r"))) {
        perror(config.infile_name);
        return EXIT_FAILURE;
    }

    printf("Compiling %s into %s...\n", 
            util_basename(config.infile_name),
            util_basename(config.outfile_name));

    list_init(&config.program->labels);

    if (0 != yyparse()) {
        return EXIT_FAILURE;
    }

    finalize_labels();

    write_program();

    list_destroy(&config.program->labels);
    fclose(yyin);

    printf("Finished.\n");

    return EXIT_SUCCESS;
}

void print_usage(char *pname)
{
    printf("\nUsage: %s [-hvzr] [-o <outfile>] <asmfile>\n\n", pname);
    printf("  -h             Show this help text.\n");
    printf("  -v             Turn on verbose output.\n");
    printf("  -o <outfile>   Write compiled program to <outfile>.\n");
    printf("  -z             Do NOT strip trailing zeros.\n");
    printf("  -r             Print resolved label addresses.\n");
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
    config.print_resolved_labels = FALSE;
    config.program = &program;

    /* parse cmdline arguments */
    while ((opt = getopt(argc, argv, "ho:vzr")) != -1) {
        switch (opt) {
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
            case 'r':
                config.print_resolved_labels = TRUE;
                break;
            default:
                print_usage(process_name);
                return EXIT_SUCCESS;
        }
    }

    if (optind >= argc) {
        print_usage(process_name);
        return EXIT_SUCCESS;
    }

    config.infile_name = strdup(argv[optind]);

    return compile();
}
