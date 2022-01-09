/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef BACKGROUND_H_
#define BACKGROUND_H_

#include <cairo/cairo.h>

#ifdef __cplusplus
extern "C" {
#endif

void
background_draw(cairo_t *cr);

#ifdef __cplusplus
}
#endif

#endif /* BACKGROUND_H_ */