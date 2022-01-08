/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "gam.h"

#include <parsers/apt_dat.h>
#include <parsers/scenery_packs.h>
#include <utils/log.h>

int
gam_init() {
    scenery_packs_data_t *scen_data;
    char                **file_data = NULL;
    size_t                file_data_size;

    scen_data = scenery_packs_parse(USER_XPLANE_ROOT);

    file_data_size = scenery_packs_get_data(scen_data, NULL);
    scenery_packs_get_data(scen_data, &file_data);

    airport_db_t *db = apt_dat_parse((const char **)file_data, file_data_size);

    db = apt_dat_db_free(db);

    scenery_packs_free(scen_data);

    return 0;
}