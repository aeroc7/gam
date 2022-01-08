/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <gam/gam.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils/log.h>

int
main() {
    if (gam_init() != 0) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}