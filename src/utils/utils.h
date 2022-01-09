/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UTILS_H_
#define UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Does nothing more than replace with null-terminator */
void
utils_strip_newline(char *str);
char *
utils_strdup(const char *str);

/* These operate on spaces: " " */
char *
utils_str_split_at(const char *data, unsigned index);
char *
utils_str_split_after(const char *data, unsigned index);

long
utils_gettime();

#ifdef __cplusplus
}
#endif

#endif /* UTILS_H_ */