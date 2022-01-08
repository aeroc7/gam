/**
 * Copyright 2022 Bennett Anderson
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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