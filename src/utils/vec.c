/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "vec.h"

#include <stdint.h>
#include <stdlib.h>

#include "log.h"

struct vector {
    size_t size;
    size_t capacity;
    size_t data_size;
    void  *data;
};

vector_t *
vector_create(size_t data_size, size_t init_size) {
    vector_t *vec;

    vec = malloc(sizeof(*vec));

    vec->data_size = data_size;
    vec->capacity = init_size;
    vec->data = malloc(data_size * init_size);
    vec->size = 0;

    return vec;
}

static void
vector_check_reallocate(vector_t *vec) {
    ASSERT(vec != NULL);

    if ((vec->size + 1) < vec->capacity) {
        return;
    }

    if (vec->capacity == 0) {
        vec->capacity += 1;
    }

    vec->capacity = vec->capacity * 2;
    vec->data = realloc(vec->data, vec->capacity * vec->data_size);
}

void *
vector_begin(const vector_t *vec) {
    ASSERT(vec != NULL);
    return vec->data;
}

void *
vector_end(const vector_t *vec) {
    ASSERT(vec != NULL);
    void *end = (uint8_t *)vec->data + (vec->size * vec->data_size);
    return end;
}

void
vector_push(vector_t *vec, void *elem) {
    ASSERT(vec != NULL);
    ASSERT(elem != NULL);
    void *vec_end;

    /* Reallocate if necessary */
    vector_check_reallocate(vec);

    vec_end = vector_end(vec);
    memcpy(vec_end, elem, vec->data_size);

    vec->size += 1;
}

void
vector_get(const vector_t *vec, size_t index, void *data_out) {
    ASSERT(vec != NULL);
    ASSERT(index < vec->size);
    void *index_pos = (uint8_t *)vec->data + (index * vec->data_size);
    memcpy(data_out, index_pos, vec->data_size);
}

size_t
vector_size(const vector_t *vec) {
    ASSERT(vec != NULL);
    return vec->size;
}

void *
vector_destroy(vector_t *vec) {
    ASSERT(vec != NULL);
    free(vec->data);
    free(vec);
    return NULL;
}