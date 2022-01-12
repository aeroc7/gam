/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef VEC_H_
#define VEC_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vector vector_t;

vector_t *
vector_create(size_t data_size, size_t init_size);
void *
vector_begin(const vector_t *vec);
void *
vector_end(const vector_t *vec);
void
vector_push(vector_t *vec, void *elem);
void
vector_get(const vector_t *vec, size_t index, void *data_out);
size_t
vector_size(const vector_t *vec);
void *
vector_destroy(vector_t *vec);

#ifdef __cplusplus
}
#endif

#endif /* VEC_H_ */