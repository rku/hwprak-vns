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

#ifndef LIST_H
#define LIST_H 1

typedef void free_function(void *);

typedef struct _list_item {
    struct _list_item *next;
    struct _list_item *prev;
    void *payload;
    free_function *free_func;
} list_item;

typedef struct _list {
    list_item *head;
    unsigned int length;
} list;

void list_init(list *l);
void list_destroy(list *l);
void list_insert(list *l, list_item *at, void *payload, free_function *f);
void list_remove(list *l, list_item *item);

#endif /* LIST_H */
