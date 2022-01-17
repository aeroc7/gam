/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ts_queue.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "log.h"

/*
 * Simple queue implementation (thread-safe provided the user uses
 * some form of synchronization across threads).
 */

#define VOIDP_ADD(ptr, nbytes) ((uint8_t *)(ptr) + (nbytes))
#define TS_QUEUE_INIT_SIZE     4

struct ts_queue {
    size_t size;
    size_t capacity;
    size_t data_size;
    void  *data;
    void  *front;
    void  *back;
};

static inline size_t
ts_queue_voidp_diff(void *p1, void *p2) {
    ASSERT(p1 != NULL);
    ASSERT(p2 != NULL);
    ASSERT(p2 >= p1);
    size_t diff;
    diff = (size_t)((uint8_t *)p2 - (uint8_t *)p1);
    return diff;
}

ts_queue_t *
ts_queue_create(size_t data_size, size_t min_size) {
    ASSERT(data_size > 0);
    ts_queue_t *q;

    q = calloc(1, sizeof(*q));

    q->data_size = data_size;
    q->capacity = ((min_size == 0) ? TS_QUEUE_INIT_SIZE : min_size);
    q->data = malloc(data_size * q->capacity);
    q->front = q->back = q->data;

    return q;
}

static void
ts_queue_realloc_data(ts_queue_t *q) {
    ASSERT(q != NULL);
    size_t realloc_size, elems_size;
    void  *new_buf;

    realloc_size = q->capacity * 2;
    elems_size = ts_queue_voidp_diff(q->front, q->back);
    new_buf = malloc(realloc_size * q->data_size);
    memcpy(new_buf, q->front, elems_size);
    free(q->data);

    q->data = new_buf;
    q->front = new_buf;
    q->back = VOIDP_ADD(q->front, elems_size);
    q->capacity = realloc_size;
}

/* Retrieve an address where the next element can go */
static void *
ts_queue_next_elem_loc(ts_queue_t *q) {
    void  *alloc_end;
    size_t alloc_left_bytes;

    alloc_end = VOIDP_ADD(q->data, q->data_size * q->capacity);
    alloc_left_bytes = ts_queue_voidp_diff(q->back, alloc_end);

    /* If there's less than or equal to one more elem, then reallocate*/
    if (alloc_left_bytes <= q->data_size) {
        ts_queue_realloc_data(q);
    }

    return q->back;
}

void
ts_queue_push(ts_queue_t *q, void *elem) {
    ASSERT(q != NULL);
    ASSERT(elem != NULL);

    void *ne_loc = ts_queue_next_elem_loc(q);
    memcpy(ne_loc, elem, q->data_size);
    /* Advance ending pointer */
    q->back = VOIDP_ADD(q->back, q->data_size);
    q->size += 1;
}

void
ts_queue_pop(ts_queue_t *q) {
    ASSERT(q != NULL);
    if (q->front == q->back) {
        return;
    }
    ASSERT(q->size != 0);

    /* Advance starting pointer */
    q->front = VOIDP_ADD(q->front, q->data_size);
    q->size -= 1;
}

int
ts_queue_front(ts_queue_t *q, void *elem) {
    ASSERT(q != NULL);
    ASSERT(elem != NULL);
    if (q->front == q->back) {
        return 1;
    }

    memcpy(elem, q->front, q->data_size);
    return 0;
}

size_t
ts_queue_size(ts_queue_t *q) {
    ASSERT(q != NULL);
    return q->size;
}

size_t
ts_queue_capacity(ts_queue_t *q) {
    ASSERT(q != NULL);
    return q->capacity;
}

void *
ts_queue_destroy(ts_queue_t *q) {
    ASSERT(q != NULL);
    free(q->data);
    free(q);
    return NULL;
}
