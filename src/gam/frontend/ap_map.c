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

static double
haversine_formula_meters(double lat1, double lon1, double lat2, double lon2) {
    double dlat = DEG_TO_RAD(lat2 - lat1);
    double dlon = DEG_TO_RAD(lon2 - lon1);

    lat1 = DEG_TO_RAD(lat1);
    lat2 = DEG_TO_RAD(lat2);

    double a = pow(sin(dlat / 2.0), 2) + pow(sin(dlon / 2.0), 2) * cos(lat1) * cos(lat2);
    double c = 2.0 * asin(sqrt(a));
    return EARTH_RADIUS * c;
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

    for (size_t i = 0; i < ap->db->airports[ap_index].boundaries.size; ++i) {
        const airport_bounds_t *bnds = &ap->db->airports[ap_index].boundaries;

        if (bnds->latitude[i] > ap->map_bounds.lat1) {
            ap->map_bounds.lat1 = bnds->latitude[i];
        }

        if (bnds->longitude[i] > ap->map_bounds.lon1) {
            ap->map_bounds.lon1 = bnds->longitude[i];
        }

        if (bnds->latitude[i] < ap->map_bounds.lat2) {
            ap->map_bounds.lat2 = bnds->latitude[i];
        }

        if (bnds->longitude[i] < ap->map_bounds.lon2) {
            ap->map_bounds.lon2 = bnds->longitude[i];
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

    /* Map real distance to pixel values */
    double w_h_ratio = bb_width_m / bb_height_m;
    ap->draw_w = (int)(w_h_ratio * GAM_WINDOW_HEIGHT);
    ap->draw_h = GAM_WINDOW_HEIGHT;

    ap->draw_w_ratio = ap->draw_w / bb_width_m;
    ap->draw_h_ratio = ap->draw_h / bb_height_m;
}

static double
ap_map_distance_to_x(ap_map_t *ap, double distance) {
    ASSERT(ap != NULL);
    return ap->draw_w_ratio * distance;
}

static double
ap_map_distance_to_y(ap_map_t *ap, double distance) {
    ASSERT(ap != NULL);
    return ap->draw_h_ratio * distance;
}

void
ap_map_draw(cairo_t *cr, ap_map_t *ap, size_t ap_index) {
    ap_map_set_draw_dims(ap, ap_index);

    for (size_t i = 0; i < ap->db->airports[ap_index].runways_size; ++i) {
        const runway_info_t *rwy = &ap->db->airports[ap_index].runways[i];
        double               d_to_min_lat[2];
        double               d_to_min_lon[2];

        for (int j = 0; j < 2; ++j) {
            double avg_lat = (ap->map_bounds.lat1 + rwy->latitude[j]) / 2.0;
            double avg_lon = (ap->map_bounds.lon1 + rwy->longitude[j]) / 2.0;

            d_to_min_lat[j] =
                haversine_formula_meters(rwy->latitude[j], avg_lon, ap->map_bounds.lat1, avg_lon);
            d_to_min_lon[j] =
                haversine_formula_meters(avg_lat, rwy->longitude[j], avg_lat, ap->map_bounds.lon1);
        }

        cairo_set_source_rgb(cr, 0, 0.25, 0.75);
        cairo_move_to(cr, ap_map_distance_to_x(ap, d_to_min_lat[0]),
            ap_map_distance_to_y(ap, d_to_min_lon[0]));
        cairo_line_to(cr, ap_map_distance_to_x(ap, d_to_min_lat[1]),
            ap_map_distance_to_y(ap, d_to_min_lon[1]));
        cairo_stroke(cr);
    }
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