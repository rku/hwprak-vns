
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "globals.h"
#include "vnsem.h"

vnsem_configuration config;

void print_usage(char *pname)
{
    printf("\nUsage: %s [-h] | [-v] [-i] [-s <ms>] <program>\n\n", pname);
    printf("  -h         Show this help text.\n");
    printf("  -v         Turn on verbose output.\n");
    printf("  -i         Turn on interactive mode.\n");
    printf("  -s <ms>    Set step time to <ms> milliseconds.\n");
    printf("\n");
}

int main(int argc, char **argv)
{
    unsigned int opt;
    char *process_name = argv[0];

    printf(BANNER_LINE1, "Emulator");
    printf(BANNER_LINE2, VERSION);

    while(-1 != (opt = getopt(argc, argv, "hvis:"))) {
        switch(opt) {
            case 'h':
                print_usage(process_name);
                return EXIT_SUCCESS;
            case 'v':
                config.verbose_mode = TRUE;
                break;
            case 'i':
                config.step_time_ms = atoi(optarg);
                break;
            default:
                print_usage(process_name);
                return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}
