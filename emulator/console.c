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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "globals.h"
#include "vnsem.h"
#include "utils.h"
#include "console.h"

#define CONSOLE_COMMAND_MAX_ARGS (5)

void console_break(int argc, char **argv, vnsem_machine *machine);
void console_help(int argc, char **argv, vnsem_machine *machine);
void console_load(int argc, char **argv, vnsem_machine *machine);
void console_machine(int argc, char **argv, vnsem_machine *machine);
void console_memdump(int argc, char **argv, vnsem_machine *machine);
void console_memset(int argc, char **argv, vnsem_machine *machine);
void console_quit(int argc, char **argv, vnsem_machine *machine);
void console_reset(int argc, char **argv, vnsem_machine *machine);
void console_run(int argc, char **argv, vnsem_machine *machine);
void console_step(int argc, char **argv, vnsem_machine *machine);

/**
 * This list is searched by binary search so keep it ordered by
 * command name.
 */
static const console_command console_commands[] = {
    { "break",   console_break,   "Set break point.",
                 0, 1,            "[<addr>|clear]" },
    { "help",    console_help,    "Show help for commands.",
                 0, 1,            "<command>" },
    { "load",    console_load,    "Load a program.",
                 1, 2,            "<programfile> [<offset>]" },
    { "machine", console_machine, "Show machine information.",
                 0, 0,            NULL },
    { "memdump", console_memdump, "Dump memory.",
                 0, 1,            "[<addr>]" },
    { "memset",  console_memset,  "Set content of a memory cell",
                 2, 2,            "<addr> <value>" },
    { "quit",    console_quit,    "Quit the emulator.",
                 0, 0,            NULL },
    { "reset",   console_reset,   "Reset (parts of) the machine.",
                 1, 1,            "pc|mem|all" },
    { "run",     console_run,     "Start the machine.",
                 0, 0,            NULL },
    { "step",    console_step,    "Execute next stepwise.",
                 0, 0,            NULL }
};

int commandcmp(const void *key, const void *other)
{
    console_command *cmd = (console_command*)other;
    return strcasecmp((char*)key, cmd->name);
}

const console_command *find_command(const char *name)
{
    const console_command *cmd;
    size_t size;
    int n;

    size = sizeof(console_command);
    n = sizeof(console_commands) / size; 

    cmd = bsearch((void*)name, (void*)&console_commands,
            n, size, commandcmp);

    return cmd;
}

char *command_generator(const char *text, int state)
{
    static int len, index;
    static int items = sizeof(console_commands) / sizeof(console_command);
    char *name;

    if (!state) {
        index = 0;
        len = strlen(text);
    }

    while (index < items) {
        name = console_commands[index].name;
        ++index;
        if (0 == strncasecmp(name, text, len)) {
            return strdup(name); /* readline will free this */
        }
    }

    return NULL;
}

char **completion(const char *text, int start, int end)
{
    char **matches = NULL;

    if (0 == start) {
        matches = rl_completion_matches(text, command_generator);
    }

    return matches;
}

void console_break(int argc, char **argv, vnsem_machine *machine)
{
    uint8_t addr = 0;

    if (argc == 1) {
        if (machine->break_enabled) {
            printf("Current break point at address 0x%.2x.\n",
                    machine->break_point);
        } else {
            printf("No breakpoint set.\n");
        }

        return;
    }

    if (!strncasecmp(argv[1], "clear", 5)) {
        machine->break_enabled = FALSE;
        printf("Break point cleared.\n");
        return;
    }

    if (!util_strtouint8(argv[1], &addr)) {
        printf("Invalid address: %s\n", argv[1]);
        return;
    }

    machine->break_point = addr;
    machine->break_enabled = TRUE;
    printf("Break point set at address 0x%.2x\n", addr);
}

void console_help(int argc, char **argv, vnsem_machine *machine)
{
    const console_command *cmd;

    int i = 0;
    int cmds_n = sizeof(console_commands) / sizeof(console_command);

    if (1 == argc) {
        printf("\nCommands available:\n\n");
        for (; i < cmds_n; ++i) {
            cmd = &console_commands[i];
            printf("  %-10s  %s\n", cmd->name, cmd->description);
        }
        printf("\nYou may also try 'help <command>' for additional help.\n\n");
        return;
    }

    if (NULL == (cmd = find_command(argv[1]))) {
        printf("Unknown command: %s\n", argv[1]);
        return;
    }

    printf("\n      Command: %s\n  Description: %s\n    Arguments: %s\n\n",
            cmd->name, cmd->description,
            (NULL == cmd->usage) ? "None" : cmd->usage);
}

void console_load(int argc, char **argv, vnsem_machine *machine)
{
    uint8_t off = 0;

    if (3 == argc) {
        if (!util_strtouint8(argv[2], &off)) {
            printf("Invalid offset address: %s\n", argv[2]);
            return;
        }
    }

    load_program(argv[1], off, machine);
    machine->pc = off;
}

void console_machine(int argc, char **argv, vnsem_machine *machine)
{
    printf("\n  ** Machine information **\n\n");
    printf("           Memory: (-> memdump)\n");
    printf("  Program counter: 0x%.2x\n", machine->pc);
    printf("    Stack pointer: 0x%.2x\n", machine->sp);
    printf("     Step counter: %i (since reset)\n", machine->step_count);
    printf("      Accumulator: 0x%.2x (%i)\n",
            machine->accu, machine->accu);
    printf("                L: 0x%.2x (%i)\n",
            machine->reg_l, machine->reg_l);
    printf("       Carry flag: %s\n",
            (machine->flags & F_CARRY) ? "set" : "unset");
    printf("        Zero flag: %s\n",
            (machine->flags & F_ZERO) ? "set" : "unset");
    printf("        Sign flag: %s\n",
            (machine->flags & F_SIGN) ? "set" : "unset");
    printf("\n");
}

void console_memdump(int argc, char **argv, vnsem_machine *machine)
{
    uint8_t addr = 0;

    if (2 == argc) {
        if (!util_strtouint8(argv[1], &addr)) {
            printf("Invalid address: %s\n", argv[1]);
            return;
        }

        printf(" 0x%.2x   0x%.2x (%i)\n",
                addr, machine->mem[addr], machine->mem[addr]);
        return;
    }

    dump_memory(machine);
}

void console_memset(int argc, char **argv, vnsem_machine *machine)
{
    uint8_t a = 0, v = 0;

    if (!util_strtouint8(argv[1], &a)) {
        printf("Invalid address: %s\n", argv[1]);
        return;
    }

    if (!util_strtouint8(argv[2], &v)) {
        printf("Invalid value: %s\n", argv[2]);
        return;
    }

    machine->mem[a] = v;
    printf(" 0x%.2x   0x%.2x (%i)\n", a, v, v);
}

void console_quit(int argc, char **argv, vnsem_machine *machine)
{
    exit(EXIT_SUCCESS);
}

void console_reset(int argc, char **argv, vnsem_machine *machine)
{
    if (!strncasecmp("mem", argv[1], 3)) {
        memset((void*)&machine->mem, 0, sizeof(machine->mem));
        printf("Memory unit has been reset.\n");
    } else
    if (!strncasecmp("pc", argv[1], 2)) {
        machine->pc = 0;
        machine->step_count = 0;
        printf("Program counter has been reset to 0x%.2x.\n", machine->pc);
    } else
    if (!strncasecmp("all", argv[1], 3)) {
        reset_machine(machine);
        machine->halted = TRUE;
        printf("Machine has been reset.\n");
    } else {
        printf("Invalid argument.\n");
    }
}

void console_run(int argc, char **argv, vnsem_machine *machine)
{
    machine->halted = FALSE;
}

void console_step(int argc, char **argv, vnsem_machine *machine)
{
    machine->halted = FALSE;
    machine->step_mode = TRUE;
}

int call_command(char *name, int argc, char **argv, vnsem_machine *machine)
{
    const console_command *cmd;
    int arg_n = argc - 1; /* argc counts the command itself too */

    if (NULL != (cmd = find_command(name))) {
        if (arg_n < cmd->minargs || arg_n > cmd->maxargs) {
            printf("Invalid number of arguments.\n");
            printf("Usage: %s %s\n", name,
                    (NULL == cmd->usage) ? 
                    "(No arguments allowed)" : cmd->usage);
        } else {
            cmd->func(argc, argv, machine);
            return TRUE;
        }
    } else {
        printf("Unknown command: %s\n", name);
    }

    return FALSE;
}

int call_command_for_input(const char *input, vnsem_machine *machine)
{
    char *argv[CONSOLE_COMMAND_MAX_ARGS], *delim = " ";
    char *s = strdup(input);
    int result = FALSE, argc = 0;

    /* skip whitespaces */
    while (isspace(*s)) {
        ++s;
    }

    if (*s) {
        /* split up arguments */
        s = strtok(s, delim);
        while (NULL != s && argc < CONSOLE_COMMAND_MAX_ARGS) {
            argv[argc++] = s;
            s = strtok(NULL, delim);
        }

        result = call_command(argv[0], argc, (char**)&argv, machine);
    }

    free(s);
    return result;
}

void vnsem_console(vnsem_machine *machine)
{
    char *input;

    rl_attempted_completion_function = completion;

    while (1) {
        input = readline("VNSEM Console => ");

        if (NULL == input) {
            exit(EXIT_SUCCESS);
        }

        if (call_command_for_input(input, machine)) {
            add_history(input);
            break;
        }

        free(input);
    }

    rl_attempted_completion_function = NULL;
}
