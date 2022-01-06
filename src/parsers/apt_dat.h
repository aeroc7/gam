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

#ifndef APT_DAT_H_
#define APT_DAT_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct airport_info {
    char  *name;
    char  *city;
    char  *country;
    char  *state;
    char  *icao;
    double latitude;
    double longitude;
} airport_info_t;

typedef struct airport_db {
    airport_info_t *airports;
    size_t          airports_size;
} airport_db_t;

airport_db_t *
apt_dat_parse(const char **files, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* APT_DAT_H_ */