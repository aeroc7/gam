/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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