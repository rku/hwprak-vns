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

#include "globals.h"
#include "list.h"

void list_init(list *l)
{
    l->length = 0;
    l->head = NULL;
}

void list_destroy(list *l)
{
    while (l->length) {
        list_remove(l, l->head);
    }
}

void list_insert(list *l, list_item *at, void *payload, free_function *f)
{
    list_item *new_item = (list_item*)malloc(sizeof(list_item));

    new_item->payload = payload;
    new_item->free_func = f;

    if (NULL == at) {
        /* insert at the beginning */
        new_item->next = l->head;
        new_item->prev = NULL;
        l->head = new_item;
    } else {
        /* insert after "at" */
        new_item->next = at->next;
        new_item->prev = at;
        at->next = new_item;
    }

    l->length++;
}

void list_remove(list *l, list_item *item)
{
    if (NULL != item->prev) {
        item->prev->next = item->next;
        item->next->prev = item->prev;
    }

    if (NULL != item->free_func) {
        item->free_func(item->payload);
    }

    free(item);
    l->length--;
}
