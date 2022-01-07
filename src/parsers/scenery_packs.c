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
#include <utils/utils.h>

#define SCENERY_INI_PATH_EXT "Custom Scenery/scenery_packs.ini"
#define SCENERY_APT_DAT_EXT  "Earth nav data/apt.dat"
#define SCENERY_PACK_KEY     "SCENERY_PACK"

struct scenery_packs_data {
    char **paths;
    size_t paths_size;
};

static scenery_packs_data_t *
scenery_packs_get_file_data(const char *xp_path, const char *native_path) {
    FILE                 *fp;
    char                 *line_buf = NULL;
    size_t                line_size;
    scenery_packs_data_t *scenery_paths;
    size_t                paths_alloc_sz = 15;

    scenery_paths = malloc(sizeof(*scenery_paths));
    scenery_paths->paths = malloc(paths_alloc_sz * sizeof(char *));
    scenery_paths->paths_size = 0;

    fp = fopen(native_path, "r");
    if (!fp) {
        log_err("Failed to open file %s", native_path);
        fclose(fp);
        return NULL;
    }

    while (getline(&line_buf, &line_size, fp) != -1) {
        if (strstr(line_buf, SCENERY_PACK_KEY) == line_buf) {
            ASSERT(strlen(line_buf) > (strlen(SCENERY_PACK_KEY) + 1));

            if ((scenery_paths->paths_size + 1) == paths_alloc_sz) {
                paths_alloc_sz += 10;
                scenery_paths->paths =
                    realloc(scenery_paths->paths, paths_alloc_sz * sizeof(char *));
            }

            const size_t offset = strlen(SCENERY_PACK_KEY);
            char        *path = line_buf + offset + 1;
            utils_strip_newline(path);

            /* Check if path is empty */
            if (strlen(path) == 0) {
                continue;
            }

            /* Assemble absolute paths */
            char *npath = path_hdlr_join_paths(xp_path, path);
            char *full_path = path_hdlr_join_paths(npath, SCENERY_APT_DAT_EXT);

            scenery_paths->paths[scenery_paths->paths_size] =
                path_hdlr_convert_to_native(full_path);
            scenery_paths->paths_size += 1;

            free(full_path);
            free(npath);
        }
    }

    free(line_buf);
    fclose(fp);

    return scenery_paths;
}

scenery_packs_data_t *
scenery_packs_parse(const char *xp_path) {
    char                 *new_path, *native_path;
    scenery_packs_data_t *ret;

    new_path = path_hdlr_join_paths(xp_path, SCENERY_INI_PATH_EXT);
    native_path = path_hdlr_convert_to_native(new_path);
    ret = scenery_packs_get_file_data(xp_path, native_path);

    free(new_path);
    free(native_path);

    return ret;
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

size_t
scenery_packs_get_data(scenery_packs_data_t *data, char ***ret) {
    ASSERT(data != NULL);

    if (ret == NULL) {
        return data->paths_size;
    }

    *ret = data->paths;
    return data->paths_size;
}