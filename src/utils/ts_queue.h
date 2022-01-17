/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef TS_QUEUE_H_
#define TS_QUEUE_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ts_queue ts_queue_t;

ts_queue_t *
ts_queue_create(size_t data_size, size_t min_size);
void
ts_queue_push(ts_queue_t *q, void *elem);
void
ts_queue_pop(ts_queue_t *q);
int
ts_queue_front(ts_queue_t *q, void *elem);
size_t
ts_queue_size(ts_queue_t *q);
size_t
ts_queue_capacity(ts_queue_t *q);
void *
ts_queue_destroy(ts_queue_t *q);

#ifdef __cplusplus
}
#endif

#endif /* TS_QUEUE_H_ */