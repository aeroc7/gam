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
#include <utils/log.h>

static window_inst_t   *winst;
static cairo_mt_t      *cmt;
static volatile int32_t quit = 0;

static void
window_loop_cb(window_inst_t *window) {
    UNUSED(window);
    cairo_mt_draw(cmt);
}

static void
mt_start(cairo_t *cr) {
    UNUSED(cr);
}

static int
mt_loop(cairo_t *cr) {
    cairo_pattern_t *pat;

    pat = cairo_pattern_create_linear(0.0, 0.0, 0.0, 720.0);
    cairo_pattern_add_color_stop_rgba(pat, 1, 0, 0, 0, 1);
    cairo_pattern_add_color_stop_rgba(pat, 0, 1, 1, 1, 1);
    cairo_rectangle(cr, 0, 0, 1280, 720);
    cairo_set_source(cr, pat);
    cairo_fill(cr);
    cairo_pattern_destroy(pat);

    pat = cairo_pattern_create_radial(115.2, 102.4, 25.6, 102.4, 102.4, 128.0);
    cairo_pattern_add_color_stop_rgba(pat, 0, 1, 1, 1, 1);
    cairo_pattern_add_color_stop_rgba(pat, 1, 0, 0, 0, 1);
    cairo_set_source(cr, pat);
    cairo_arc(cr, 128.0, 128.0, 76.8, 0, 2 * 3.14159268);
    cairo_fill(cr);
    cairo_pattern_destroy(pat);

    return quit;
}

static void
mt_end(cairo_t *cr) {
    UNUSED(cr);
}

void
frontend_init() {
    window_graphics_global_init();

    winst = window_create("GAM", GAM_WINDOW_WIDTH, GAM_WINDOW_HEIGHT);

    if (glewInit() != GLEW_OK) {
        log_err("Failed to initialize glew.");
        exit(EXIT_FAILURE);
    }

    /* Init MT cairo rendering */
    cmt = cairo_mt_create(GAM_WINDOW_WIDTH, GAM_WINDOW_HEIGHT);
    cairo_mt_set_callbacks(cmt, mt_start, mt_loop, mt_end);
    cairo_mt_start(cmt);

    window_set_window_loop_callback(winst, window_loop_cb);
    window_loop(winst);
}

void
frontend_destroy() {
    quit = 1;
    window_destroy(winst);
    window_graphics_global_destroy();
    cmt = cairo_mt_destroy(cmt);
}