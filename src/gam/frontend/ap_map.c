/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ap_map.h"

#include <float.h>
#include <gam/gam_defs.h>
#include <math.h>
#include <utils/constants.h>
#include <utils/log.h>

struct ap_map {
    double        max_x;
    double        max_y;
    double        min_x;
    double        min_y;

    airport_db_t *db;
};

static vec2d_t
equirectangular_proj(double lat, double lon, double lat_ref) {
    ASSERT(!isnan(lat));
    ASSERT(!isnan(lon));
    ASSERT(!isnan(lat_ref));
    vec2d_t coords;

    coords.x = EARTH_RADIUS * lon * cos(DEG_TO_RAD(lat_ref));
    coords.y = EARTH_RADIUS * lat;

    coords.x = fabs(coords.x);
    coords.y = fabs(coords.y);

    ASSERT(!isnan(coords.x));

    return coords;
}

static vec2d_t
global_to_local_xy(const ap_map_t *ap, vec2d_t rwy) {
    ASSERT(!isnan(rwy.x));
    ASSERT(!isnan(rwy.y));

    double difx = rwy.x - ap->min_x;
    double dify = rwy.y - ap->min_y;

    rwy.x = difx * ((GAM_WINDOW_WIDTH) / (ap->max_x - ap->min_x));
    rwy.y = dify * ((GAM_WINDOW_HEIGHT) / (ap->max_y - ap->min_y));

    return rwy;
}

static vec4d_t
rwy_coords_convert_combine(const ap_map_t *ap, vec2d_t rwy1, vec2d_t rwy2) {
    ASSERT(!isnan(rwy1.x));
    ASSERT(!isnan(rwy1.y));
    ASSERT(!isnan(rwy2.x));
    ASSERT(!isnan(rwy2.y));
    vec4d_t ret;

    rwy1.x = fabs(rwy1.x);
    rwy1.y = fabs(rwy1.y);
    rwy2.x = fabs(rwy2.x);
    rwy2.y = fabs(rwy2.y);

    rwy1 = global_to_local_xy(ap, rwy1);
    rwy2 = global_to_local_xy(ap, rwy2);

    ret.a = rwy1;
    ret.b = rwy2;

    return ret;
}

static double
haversine_formula(double lat1, double lon1, double lat2, double lon2) {
    double dlat = DEG_TO_RAD(lat2 - lat1);
    double dlon = DEG_TO_RAD(lon2 - lon1);

    lat1 = DEG_TO_RAD(lat1);
    lat2 = DEG_TO_RAD(lat2);

    double a = pow(sin(dlat / 2.0), 2) + pow(sin(dlon / 2.0), 2) * cos(lat1) * cos(lat2);
    double c = 2.0 * asin(sqrt(a));
    return EARTH_RADIUS * c;
}

static double
pixels_per_meter(const ap_map_t *ap, lat2d_t p1, lat2d_t p2, double lat_ref) {
    double  real_distance_meters = haversine_formula(p1.lat, p1.lon, p2.lat, p2.lon) * 1000.0;
    vec2d_t global_coords1 = equirectangular_proj(p1.lat, p1.lon, lat_ref);
    vec2d_t global_coords2 = equirectangular_proj(p2.lat, p2.lon, lat_ref);

    vec4d_t rwy_local_p = rwy_coords_convert_combine(ap, global_coords1, global_coords2);

    /* Distance formula */
    double rwy_loc_delta_x2 = pow(rwy_local_p.b.x - rwy_local_p.a.x, 2);
    double rwy_loc_delta_y2 = pow(rwy_local_p.b.y - rwy_local_p.a.y, 2);
    double xy_distance = sqrt(rwy_loc_delta_x2 + rwy_loc_delta_y2);

    return (xy_distance / real_distance_meters);
}

ap_map_t *
ap_map_create(airport_db_t *db) {
    ASSERT(db != NULL);
    ap_map_t *ap_mp;

    ap_mp = malloc(sizeof(*ap_mp));
    ap_mp->db = db;

    ap_mp->max_x = 0.0;
    ap_mp->max_y = 0.0;
    ap_mp->min_x = DBL_MAX;
    ap_mp->min_y = DBL_MAX;

    return ap_mp;
}

void
ap_map_draw(cairo_t *cr, ap_map_t *ap, size_t ap_index) {
    vec2d_t rw[2];

    for (size_t i = 0; i < ap->db->airports[ap_index].runways_size; ++i) {
        const runway_info_t *rwy = &ap->db->airports[ap_index].runways[i];

        for (int j = 0; j < 2; ++j) {
            rw[j] = equirectangular_proj(
                rwy->latitude[j], rwy->longitude[j], ap->db->airports[ap_index].latitude);

            if (rw[j].x > ap->max_x) {
                ap->max_x = rw[j].x;
            }

            if (rw[j].y > ap->max_y) {
                ap->max_y = rw[j].y;
            }

            if (rw[j].x < ap->min_x) {
                ap->min_x = rw[j].x;
            }

            if (rw[j].y < ap->min_y) {
                ap->min_y = rw[j].y;
            }
        }

        lat2d_t p1 = {.lat = rwy->latitude[0], .lon = rwy->longitude[0]};
        lat2d_t p2 = {.lat = rwy->latitude[1], .lon = rwy->longitude[1]};
        double  pixels_p_meter = pixels_per_meter(ap, p1, p2, ap->db->airports[ap_index].latitude);

        /* Translate to x, y screen coordinates */
        vec4d_t subd = rwy_coords_convert_combine(ap, rw[0], rw[1]);

        /* Draw airport lines */
        cairo_set_source_rgb(cr, 0, 0.25, 0.75);
        cairo_set_line_width(cr, pixels_p_meter * rwy->width);
        cairo_move_to(cr, subd.a.x, subd.a.y);
        cairo_line_to(cr, subd.b.x, subd.b.y);
        cairo_stroke(cr);
    }
}

void *
ap_map_destroy(ap_map_t *apm) {
    free(apm);
    return NULL;
}