/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "frontend.h"

#include <GL/glew.h>
#include <gam/gam_defs.h>
#include <graphics/cairo_mt.h>
#include <graphics/window.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <utils/log.h>

#include "ap_map.h"
#include "background.h"

static window_inst_t *winst = NULL;
static cairo_mt_t    *cmt = NULL;

typedef struct mt_udata {
    airport_db_t *db;
    ap_map_t     *ap_map;

    struct {
        double mouse_x;
        double mouse_y;
        bool   mouse_click;
    } mt;

    pthread_mutex_t mutex;
} mt_udata_t;

static void
window_loop_cb(window_inst_t *window, void *udata) {
    UNUSED(window);
    UNUSED(udata);
    cairo_mt_draw(cmt);
}

static void
window_mouse_position_callback(double xpos, double ypos, void *udata) {
    UNUSED(udata);
    log_msg("%lf %lf", xpos, ypos);
}

static void
mt_start(cairo_t *cr, void *udata) {
    UNUSED(cr);
    ASSERT(udata != NULL);
    mt_udata_t *mtdata = (mt_udata_t *)udata;
    mtdata->ap_map = ap_map_create(mtdata->db);
}

static void
mt_loop(cairo_t *cr, void *udata) {
    ASSERT(udata != NULL);
    mt_udata_t *mtdata = (mt_udata_t *)udata;
    background_draw(cr);

    airport_db_t *db = mtdata->db;
    ASSERT(db->airports_size > 0);

    size_t ap_index = apt_dat_find_by_icao(db, "KSEA");
    ap_map_draw(cr, mtdata->ap_map, ap_index);
}

static void
mt_end(cairo_t *cr, void *udata) {
    UNUSED(cr);
    ASSERT(udata != NULL);
    mt_udata_t *mtdata = (mt_udata_t *)udata;

    mtdata->ap_map = ap_map_destroy(mtdata->ap_map);
    pthread_mutex_destroy(&mtdata->mutex);
    free(mtdata);
}

void
frontend_init(airport_db_t *db) {
    mt_udata_t *mtdata;
    window_graphics_global_init();

    mtdata = malloc(sizeof(*mtdata));
    pthread_mutex_init(&mtdata->mutex, NULL);
    mtdata->db = db;

    winst = window_create(GAM_WINDOW_TITLE, GAM_WINDOW_WIDTH, GAM_WINDOW_HEIGHT);
    window_set_mouse_pos_callback(winst, window_mouse_position_callback, mtdata);

    if (glewInit() != GLEW_OK) {
        log_err("Failed to initialize glew.");
        exit(EXIT_FAILURE);
    }

    /* Init MT cairo rendering */
    cmt = cairo_mt_create(GAM_WINDOW_WIDTH, GAM_WINDOW_HEIGHT);
    cairo_mt_set_callbacks(cmt, mt_start, mt_loop, mt_end);
    cairo_mt_start(cmt, mtdata);

    window_set_window_loop_callback(winst, window_loop_cb, NULL);
    window_loop(winst);
}

void
frontend_destroy() {
    window_destroy(winst);
    window_graphics_global_destroy();
    cmt = cairo_mt_destroy(cmt);
}