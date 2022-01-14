/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct window_inst window_inst_t;

void
window_graphics_global_init();
window_inst_t *
window_create(const char *title, int w, int h);
void
window_set_mouse_pos_callback(
    window_inst_t *wind, void (*cb)(double xpos, double ypos, void *ud), void *udata);
void
window_set_mouse_button_callback(
    window_inst_t *wind, void (*cb)(bool mouse_down, bool mouse_hold, void *ud), void *udata);
void
window_loop(window_inst_t *window);
void
window_set_window_loop_callback(
    window_inst_t *window, void (*cb)(window_inst_t *window, void *ud), void *udata);
void *
window_destroy(window_inst_t *window);
void
window_set_mouse_scroll_callback(
    window_inst_t *wind, void (*cb)(int mouse_scroll, void *udata), void *udata);
void
window_graphics_global_destroy();

#ifdef __cplusplus
}
#endif

#endif /* WINDOW_H_ */