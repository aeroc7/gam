/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ROUND_RECT_H_
#define ROUND_RECT_H_

#include <cairo/cairo.h>

#ifdef __cplusplus
extern "C" {
#endif

static void
rounded_rectangle(cairo_t *cr, int x, int y, int w, int h, int r) {
    cairo_new_sub_path(cr);
    cairo_arc(cr, x + r, y + r, r, M_PI, 3 * M_PI / 2);
    cairo_arc(cr, x + w - r, y + r, r, 3 * M_PI / 2, 2 * M_PI);
    cairo_arc(cr, x + w - r, y + h - r, r, 0, M_PI / 2);
    cairo_arc(cr, x + r, y + h - r, r, M_PI / 2, M_PI);
    cairo_close_path(cr);
}

#ifdef __cplusplus
}
#endif

#endif /* ROUND_RECT_H_ */