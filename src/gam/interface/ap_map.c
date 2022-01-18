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
#include <utils/hex_to_rgb.h>
#include <utils/log.h>

typedef struct bounding_box {
    double lat1;
    double lon1;
    double lat2;
    double lon2;
} bounding_box_t;

struct ap_map {
    bounding_box_t map_bounds;

    double         draw_w;
    double         draw_h;

    double         draw_w_ratio;
    double         draw_h_ratio;

    airport_db_t  *db;
};

static lat2d_t
lat2d_t_create(double lat, double lon) {
    lat2d_t p;
    p.lat = lat;
    p.lon = lon;
    return p;
}

static vec2d_t
vec2d_t_create(double x, double y) {
    vec2d_t p;
    p.x = x;
    p.y = y;
    return p;
}

static double
haversine_formula_meters(double lat1, double lon1, double lat2, double lon2) {
    double dlat = DEG_TO_RAD(lat2 - lat1);
    double dlon = DEG_TO_RAD(lon2 - lon1);

    lat1 = DEG_TO_RAD(lat1);
    lat2 = DEG_TO_RAD(lat2);

    double a = pow(sin(dlat / 2.0), 2) + pow(sin(dlon / 2.0), 2) * cos(lat1) * cos(lat2);
    double c = 2.0 * asin(sqrt(a));
    return EARTH_RADIUS * c * 1000.0;
}

static void
ap_map_bounds_latlon(ap_map_t *ap, size_t ap_index) {
    // Temporary
    ASSERT(ap->db->airports[ap_index].boundaries.latitude != NULL);

    /* Set to values lat/lon could *never* be so we are sure the min/max are accurate */
    ap->map_bounds.lat1 = -1000;
    ap->map_bounds.lon1 = -1000;
    ap->map_bounds.lat2 = 1000;
    ap->map_bounds.lon2 = 1000;

    for (size_t i = 0; i < vector_size(ap->db->airports[ap_index].boundaries.latitude); ++i) {
        const airport_bounds_t *bnds = &ap->db->airports[ap_index].boundaries;

        double                  cur_lat_v;
        double                  cur_lon_v;

        vector_get(bnds->latitude, i, &cur_lat_v);
        vector_get(bnds->longitude, i, &cur_lon_v);

        if (cur_lat_v > ap->map_bounds.lat1) {
            ap->map_bounds.lat1 = cur_lat_v;
        }

        if (cur_lon_v > ap->map_bounds.lon1) {
            ap->map_bounds.lon1 = cur_lon_v;
        }

        if (cur_lat_v < ap->map_bounds.lat2) {
            ap->map_bounds.lat2 = cur_lat_v;
        }

        if (cur_lon_v < ap->map_bounds.lon2) {
            ap->map_bounds.lon2 = cur_lon_v;
        }
    }
}

static void
ap_map_set_draw_dims(ap_map_t *ap, size_t ap_index) {
    ap_map_bounds_latlon(ap, ap_index);

    double bb_avg_lat = (ap->map_bounds.lat1 + ap->map_bounds.lat2) / 2.0;
    double bb_avg_lon = (ap->map_bounds.lon1 + ap->map_bounds.lon2) / 2.0;

    /* Bounding box distance around the airport */
    /* bb_width_m x bb_height_m */
    double bb_width_m =
        haversine_formula_meters(bb_avg_lat, ap->map_bounds.lon1, bb_avg_lat, ap->map_bounds.lon2);
    double bb_height_m =
        haversine_formula_meters(ap->map_bounds.lat1, bb_avg_lon, ap->map_bounds.lat2, bb_avg_lon);

    // Swap width & height
    double width = bb_height_m;
    double height = bb_width_m;
    double nratio;

    if (width >= height) {
        nratio = GAM_UI_APT_DRAW_SIZE_W / width;
    } else {
        nratio = GAM_UI_APT_DRAW_SIZE_H / height;
    }

    width *= nratio;
    height *= nratio;

    ap->draw_w = width;
    ap->draw_h = height;
    ap->draw_w_ratio = ap->draw_w / bb_height_m;
    ap->draw_h_ratio = ap->draw_h / bb_width_m;
}

static double
ap_map_distance_to_x(const ap_map_t *ap, double distance) {
    ASSERT(ap != NULL);
    return ap->draw_w_ratio * distance;
}

static double
ap_map_distance_to_y(const ap_map_t *ap, double distance) {
    ASSERT(ap != NULL);
    return ap->draw_h_ratio * distance;
}

static vec2d_t
ap_map_latlon_project(const ap_map_t *ap, lat2d_t p) {
    vec2d_t local_cords;
    double  avg_lat = (ap->map_bounds.lat1 + p.lat) / 2.0;
    double  avg_lon = (ap->map_bounds.lon1 + p.lon) / 2.0;

    double  d_to_min_lat = haversine_formula_meters(p.lat, avg_lon, ap->map_bounds.lat1, avg_lon);
    double  d_to_min_lon = haversine_formula_meters(avg_lat, p.lon, avg_lat, ap->map_bounds.lon1);

    /* Also flips coordinates over y-axis */
    local_cords.x = ap_map_distance_to_x(ap, d_to_min_lat);
    local_cords.y = ap_map_distance_to_y(ap, d_to_min_lon);

    return local_cords;
}

static double
ap_map_pixels_per_meter(const ap_map_t *ap, size_t ap_index) {
    ASSERT(ap != NULL);
    const airport_info_t *ap_db = &ap->db->airports[ap_index];
    ASSERT(ap_db->runways_size > 0);

    const lat2d_t p1 =
        lat2d_t_create(ap_db->runways[0].latitude[0], ap_db->runways[0].longitude[0]);
    const lat2d_t p2 =
        lat2d_t_create(ap_db->runways[0].latitude[1], ap_db->runways[0].longitude[1]);

    double  real_dist_meters = haversine_formula_meters(p1.lat, p1.lon, p2.lat, p2.lon);

    vec2d_t lp1 = ap_map_latlon_project(ap, p1);
    vec2d_t lp2 = ap_map_latlon_project(ap, p2);

    /* Distance formula */
    double rwy_loc_delta_x2 = pow(lp2.x - lp1.x, 2);
    double rwy_loc_delta_y2 = pow(lp2.y - lp1.y, 2);
    double xy_distance = sqrt(rwy_loc_delta_x2 + rwy_loc_delta_y2);

    return (xy_distance / real_dist_meters);
}

static double
ap_map_meters_to_pixels(const ap_map_t *ap, size_t ap_index, double meters) {
    return ap_map_pixels_per_meter(ap, ap_index) * meters;
}

static void
ap_map_draw_runways(cairo_t *cr, const ap_map_t *ap, size_t ap_index) {
    const airport_info_t *ap_info = &ap->db->airports[ap_index];
    for (size_t i = 0; i < ap_info->runways_size; ++i) {
        const runway_info_t *rwy = &ap->db->airports[ap_index].runways[i];
        vec2d_t              rwy_coords[2];
        double               rwy_width_px;

        for (int j = 0; j < 2; ++j) {
            lat2d_t p = lat2d_t_create(rwy->latitude[j], rwy->longitude[j]);
            rwy_coords[j] = ap_map_latlon_project(ap, p);
        }

        if (rwy->width > 0.0) {
            rwy_width_px = ap_map_meters_to_pixels(ap, ap_index, rwy->width);
        } else {
            rwy_width_px = GAM_UI_APT_RUNWAY_WIDTH_DEFAULT;
        }

        cairo_set_source_rgb(cr, HEX_TO_RGB_INPLACE(GAM_UI_APT_RUNWAY_COLOR));
        cairo_set_line_width(cr, rwy_width_px);
        cairo_move_to(cr, rwy_coords[0].x, rwy_coords[0].y);
        cairo_line_to(cr, rwy_coords[1].x, rwy_coords[1].y);
        cairo_stroke(cr);
    }
}

static void
ap_map_draw_airport_bounds(cairo_t *cr, const ap_map_t *ap, size_t ap_index) {
    const airport_info_t *ap_info = &ap->db->airports[ap_index];
    const size_t          bounds_size = vector_size(ap_info->boundaries.latitude);

    for (size_t i = 1; i < bounds_size; ++i) {
        vec2d_t points[2];
        size_t  j, j_ind;

        for (j = (i - 1), j_ind = 0; j <= i; ++j, ++j_ind) {
            double lat, lon;
            vector_get(ap_info->boundaries.latitude, j, &lat);
            vector_get(ap_info->boundaries.longitude, j, &lon);

            lat2d_t lp = lat2d_t_create(lat, lon);
            points[j_ind] = ap_map_latlon_project(ap, lp);
        }

        cairo_set_source_rgb(cr, HEX_TO_RGB_INPLACE(GAM_UI_APT_BOUNDS_COLOR));
        cairo_set_line_width(cr, 2);
        cairo_move_to(cr, points[0].x, points[0].y);
        cairo_line_to(cr, points[1].x, points[1].y);
        cairo_stroke(cr);
    }
}

static void
ap_map_draw_pave_bounds(cairo_t *cr, const ap_map_t *ap, size_t ap_index) {
    const airport_info_t *ap_info = &ap->db->airports[ap_index];
    const size_t          pave_bounds_size = vector_size(ap_info->pave_bounds);

    for (size_t i = 0; i < pave_bounds_size; ++i) {
        airport_bounds_t pave_sect;
        vector_get(ap_info->pave_bounds, i, &pave_sect);
        const size_t pave_sect_size = vector_size(pave_sect.latitude);

        cairo_set_source_rgb(cr, HEX_TO_RGB_INPLACE(GAM_UI_APT_PAVE_BOUNDS_COLOR));
        cairo_new_sub_path(cr);

        for (size_t j = 0; j < pave_sect_size; ++j) {
            vec2d_t point;
            double  lat, lon;

            vector_get(pave_sect.latitude, j, &lat);
            vector_get(pave_sect.longitude, j, &lon);

            lat2d_t lp = lat2d_t_create(lat, lon);
            point = ap_map_latlon_project(ap, lp);

            if (j == 0) {
                /* Starting position */
                cairo_move_to(cr, point.x, point.y);
            }

            cairo_line_to(cr, point.x, point.y);
        }

        cairo_close_path(cr);
        cairo_fill(cr);
    }
}

static vec2d_t
ap_map_get_centered_xy(ap_map_t *ap) {
    double x_addition, y_addition;

    x_addition = (GAM_WINDOW_WIDTH / 2.0);
    y_addition = (GAM_WINDOW_HEIGHT / 2.0);

    x_addition -= ap->draw_w / 2.0;
    y_addition -= ap->draw_h / 2.0;

    return vec2d_t_create(x_addition, y_addition);
}

void
ap_map_draw(cairo_t *cr, ap_map_t *ap, size_t ap_index) {
    ap_map_set_draw_dims(ap, ap_index);

    cairo_save(cr);

    vec2d_t map_centrd = ap_map_get_centered_xy(ap);
    cairo_translate(cr, map_centrd.x, map_centrd.y);

    ap_map_draw_airport_bounds(cr, ap, ap_index);
    ap_map_draw_pave_bounds(cr, ap, ap_index);
    ap_map_draw_runways(cr, ap, ap_index);

    cairo_restore(cr);
}

ap_map_t *
ap_map_create(airport_db_t *db) {
    ASSERT(db != NULL);
    ap_map_t *ap_mp;

    ap_mp = malloc(sizeof(*ap_mp));
    ap_mp->db = db;

    ap_mp->draw_w = 0.0;
    ap_mp->draw_h = 0.0;
    ap_mp->draw_w_ratio = 0.0;
    ap_mp->draw_h_ratio = 0.0;

    return ap_mp;
}

void *
ap_map_destroy(ap_map_t *apm) {
    free(apm);
    return NULL;
}