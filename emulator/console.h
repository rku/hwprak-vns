/**
 * This file is part of hwprak-vns.
 * Copyright 2013-2015 (c) René Küttner <rene@spaceshore.net>
 *
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

#ifndef CONSOLE_H
#define CONSOLE_H 1

#include "vnsem.h"

typedef struct _console_command {
    char *name;
    void (*func)(int, char**, vnsem_machine*);
    char *description;
    int minargs;
    int maxargs;
    char *usage;
} console_command;

void vnsem_console(vnsem_machine *machine);
const console_command *find_command(const char *name);

#endif /* CONSOLE_H */
