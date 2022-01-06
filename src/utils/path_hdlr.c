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
path_hdlr_convert_to_native(const char *path) {
    size_t len;
    char  *rs;

    len = strlen(path);
    rs = malloc((sizeof(char) * len) + 1);
    strncpy(rs, path, len);
    rs[len] = '\0';

    return rs;
}