/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef HEX_TO_RGB_H_
#define HEX_TO_RGB_H_

#include <log.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void
hex_to_rgb(unsigned hex_col, double *r, double *g, double *b) {
    ASSERT(r != NULL);
    ASSERT(g != NULL);
    ASSERT(b != NULL);

    *r = ((hex_col >> 16) & 0xFF) / 255.0;
    *g = ((hex_col >> 8) & 0xFF) / 255.0;
    *b = (hex_col & 0xFF) / 255.0;
}

#ifdef __cplusplus
}
#endif

#endif /* HEX_TO_RGB_H_ */