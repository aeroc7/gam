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