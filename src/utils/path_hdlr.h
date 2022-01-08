/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PATH_HDLR_H_
#define PATH_HDLR_H_

#ifdef __cplusplus
extern "C" {
#endif

char *
path_hdlr_join_paths(const char *p1, const char *p2);
char *
path_hdlr_convert_to_native(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* PATH_HDLR_H_ */