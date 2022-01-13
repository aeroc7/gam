/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "utils.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "log.h"

#define MAX_SPLIT_LEN 512

void
utils_strip_newline(char *str) {
    size_t len;

    len = strlen(str);
    for (size_t i = 0; i < len; ++i) {
        if (str[i] == '\n') {
            str[i] = '\0';
        }
    }
}

char *
utils_strdup(const char *str) {
    size_t len;
    char  *str_copy;

    len = strlen(str);
    str_copy = malloc(len + 1);
    strncpy(str_copy, str, len);
    str_copy[len] = '\0';

    return str_copy;
}

char *
utils_str_split_at(const char *data, unsigned index) {
    size_t   len = strlen(data);
    char     str_arr[MAX_SPLIT_LEN];
    char    *str, *token;
    unsigned ctr = 0;

    ASSERT(len < MAX_SPLIT_LEN);

    strncpy(str_arr, data, sizeof(str_arr) - 1);
    str_arr[strlen(data)] = '\0';

    str = str_arr;

    while ((token = strtok_r(str, " ", &str))) {
        if (ctr == index) {
            return utils_strdup(token);
        }

        ++ctr;
    }

    return NULL;
}

char *
utils_str_split_after(const char *data, unsigned index) {
    unsigned ctr = 0;
    bool     was_char = false;

    for (size_t i = 0; i < strlen(data); ++i) {
        if (ctr == index) {
            if (isspace(data[i])) {
                continue;
            }

            return utils_strdup(data + i);
        }

        const bool is_space = isspace(data[i]);

        if (is_space && was_char) {
            ctr += 1;
            was_char = false;
        } else if (is_space) {
            continue;
        } else {
            was_char = true;
        }
    }

    return NULL;
}

long
utils_gettime() {
    struct timespec tp;
    VRET0(clock_gettime(CLOCK_MONOTONIC, &tp));
    return ((long)tp.tv_sec * 1000000000L) + tp.tv_nsec;
}