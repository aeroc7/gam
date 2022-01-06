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

#include "apt_dat.h"

#include <stdio.h>
#include <utils/path_hdlr.h>

#define AIRPORT_ROW_CODE 1

static size_t
apt_dat_num_airports(const char **files, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        char  *new_file_path;
        char  *line_buf = NULL;
        FILE  *fp;
        size_t line_size;

        new_file_path = path_hdlr_convert_to_native(files[i]);
        fp = fopen(new_file_path, "r");

        if (fp == NULL) {
        }

        fclose(fp);
        free(new_file_path);
    }

    return 0;
}

airport_db_t *
apt_dat_parse(const char **files, size_t size) {
    // size_t num_airports;

    // num_airports = apt_dat_num_airports(files, size);
}