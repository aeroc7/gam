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

#include "path_hdlr.h"

#include <stdlib.h>
#include <string.h>

char *
path_hdlr_join_paths(const char *p1, const char *p2) {
    char  *new_path;
    size_t len1, len2;

    len1 = strlen(p1);
    len2 = strlen(p2);
    new_path = malloc((sizeof(char) * (len1 + len2)) + 1);
    strncpy(new_path, p1, len1);
    new_path[len1] = '\0';
    strncat(new_path, p2, len2);
    new_path[len1 + len2] = '\0';

    return new_path;
}

char *
path_hdlr_convert_to_native(const char *path) {
    size_t len;
    char  *rs;

    len = strlen(path);
    rs = malloc((sizeof(char) * len) + 1);
    strncpy(rs, path, len);
    rs[len] = '\0';

    return rs;
}