/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CAIRO_MT_H_
#define CAIRO_MT_H_

#include <cairo/cairo.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cairo_mt cairo_mt_t;

cairo_mt_t *
cairo_mt_create(int width, int height);
void
cairo_mt_set_callbacks(cairo_mt_t *cmt, void (*start)(cairo_t *cr), int (*loop)(cairo_t *cr),
    void (*end)(cairo_t *cr));
void
cairo_mt_start(cairo_mt_t *cmt);
/* Called from MAIN thread, shows texture in front buffer */
void
cairo_mt_draw(cairo_mt_t *cmt);
void *
cairo_mt_destroy(cairo_mt_t *cmt);

#ifdef __cplusplus
}
#endif

#endif /* CAIRO_MT_H_ */