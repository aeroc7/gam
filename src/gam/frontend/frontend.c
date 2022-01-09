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

#include "background.h"

static window_inst_t *winst;
static cairo_mt_t    *cmt;

static void
window_loop_cb(window_inst_t *window) {
    UNUSED(window);
    cairo_mt_draw(cmt);
}

static void
mt_start(cairo_t *cr) {
    UNUSED(cr);
}

static void
mt_loop(cairo_t *cr) {
    background_draw(cr);
}

static void
mt_end(cairo_t *cr) {
    UNUSED(cr);
}

void
frontend_init() {
    window_graphics_global_init();

    winst = window_create(GAM_WINDOW_TITLE, GAM_WINDOW_WIDTH, GAM_WINDOW_HEIGHT);

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
    window_destroy(winst);
    window_graphics_global_destroy();
    cmt = cairo_mt_destroy(cmt);
}