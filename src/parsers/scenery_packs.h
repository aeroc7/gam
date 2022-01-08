/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SCENERY_PACKS_H_
#define SCENERY_PACKS_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct scenery_packs_data scenery_packs_data_t;

scenery_packs_data_t *
scenery_packs_parse(const char *xp_path);
scenery_packs_data_t *
scenery_packs_free(scenery_packs_data_t *data);
size_t
scenery_packs_get_data(scenery_packs_data_t *data, char ***ret);

#ifdef __cplusplus
}
#endif

#endif /* SCENERY_PACKS_H_ */