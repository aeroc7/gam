/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "background.h"

#include <gam/gam_defs.h>
#include <utils/cairo/round_rect.h>
#include <utils/hex_to_rgb.h>

void
background_draw_enter(cairo_t *cr) {
    /* Background */
    cairo_set_source_rgb(cr, HEX_TO_RGB_INPLACE(GAM_UI_BG_COLOR));
    rounded_rectangle(cr, 0, 0, GAM_WINDOW_WIDTH, GAM_WINDOW_HEIGHT, GAM_UI_GLOBAL_BORDER);
    cairo_fill(cr);

    /* Start clip region */
    cairo_set_source_rgb(cr, HEX_TO_RGB_INPLACE(GAM_UI_PANEL_COLOR));
    rounded_rectangle(cr, GAM_UI_GLOBAL_BORDER, GAM_UI_GLOBAL_BORDER * 3,
        GAM_UI_APT_CONTENT_PANEL_W, GAM_UI_APT_CONTENT_PANEL_H, GAM_UI_APT_CONTENT_PANEL_R);
    cairo_clip(cr);
    cairo_paint(cr);
}

void
background_draw_exit(cairo_t *cr) {
    /* End clip region */
    cairo_reset_clip(cr);
}