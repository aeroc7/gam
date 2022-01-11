/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AP_MAP_H_
#define AP_MAP_H_

#include <cairo/cairo.h>
#include <parsers/apt_dat.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ap_map ap_map_t;

typedef struct vec2d {
    double x;
    double y;
} vec2d_t;

typedef struct vec4d {
    vec2d_t a;
    vec2d_t b;
} vec4d_t;

typedef struct lat2d {
    double lat;
    double lon;
} lat2d_t;

ap_map_t *
ap_map_create(airport_db_t *db);
void *
ap_map_destroy(ap_map_t *apm);
void
ap_map_draw(cairo_t *cr, ap_map_t *ap, size_t index);

#ifdef __cplusplus
}
#endif

#endif /* AP_MAP_H_ */