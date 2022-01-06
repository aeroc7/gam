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

#include "scenery_packs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils/log.h>
#include <utils/path_hdlr.h>

#define SCENERY_INI_PATH_EXT "Custom Scenery/scenery_packs.ini"
#define SCENERY_PACK_KEY     "SCENERY_PACK"

static scenery_packs_data_t *
scenery_packs_get_data(const char *native_path) {
    FILE  *fp;
    char  *line_buf = NULL;
    size_t line_size;

    fp = fopen(native_path, "r");
    if (!fp) {
        log_err("Failed to open file %s", native_path);
        fclose(fp);
        return NULL;
    }

    while (getline(&line_buf, &line_size, fp) != -1) {
        if (strstr(line_buf, SCENERY_PACK_KEY) == line_buf) {
            const size_t offset = strlen(SCENERY_PACK_KEY);
            const char  *path = line_buf + offset;
            log_msg("Scenery pack: %s", path);
        }
    }

    free(line_buf);
    fclose(fp);

    return NULL;
}

scenery_packs_data_t *
scenery_packs_parse(const char *xp_path) {
    char *new_path, *native_path;

    new_path = path_hdlr_join_paths(xp_path, SCENERY_INI_PATH_EXT);
    native_path = path_hdlr_convert_to_native(new_path);

    scenery_packs_get_data(native_path);

    free(new_path);
    free(native_path);

    return NULL;
}

scenery_packs_data_t *
scenery_packs_free(scenery_packs_data_t *data) {
    ASSERT(data != NULL);
    ASSERT(data->paths != NULL);

    for (size_t i = 0; i < data->paths_size; ++i) {
        free(data->paths[i]);
    }

    free(data->paths);
    free(data);

    return NULL;
}