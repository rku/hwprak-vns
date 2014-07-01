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
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "globals.h"
#include "utils.h"

char *util_basename(char *path)
{
    char *basename;

    if (NULL != (basename = rindex(path, '/'))) {
        return basename + 1;
    }

    return path;
}

void util_str_toupper(char *str)
{
    char *p = str;

    while (*p) {
        *p = toupper(*p);
        p++;
    }
}

/**
 * Convert str to uint8_t and store the result in *result*.
 * Returns TRUE on success or FALSE on error. On error the
 * value of *result* is unspecified.
 */
int util_strtouint8(char *str, uint8_t *result)
{
    char *end;
    long int res;

    res = strtol(str, &end, 0);
    *result = (res & 0xff);

    if (*end == '\0' && *str != '\0' && res == *result) {
        return TRUE;
    }

    return FALSE;
}

void util_perror(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
}
