/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef APT_DAT_H_
#define APT_DAT_H_

#include <stdlib.h>
#include <utils/vec.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct runway_info {
    double width;
    char   name[2][4];
    double latitude[2];
    double longitude[2];
} runway_info_t;

typedef struct airport_bounds {
    vector_t *latitude;
    vector_t *longitude;
} airport_bounds_t;

typedef struct airport_info {
    char            *name;
    char            *city;
    char            *country;
    char            *state;
    char            *icao;
    double           latitude;
    double           longitude;

    runway_info_t   *runways;
    size_t           runways_size;

    airport_bounds_t boundaries;
    vector_t        *pave_bounds;
} airport_info_t;

typedef struct airport_db {
    airport_info_t *airports;
    size_t          airports_size;
} airport_db_t;

airport_db_t *
apt_dat_parse(const char **files, size_t size);
void *
apt_dat_db_free(airport_db_t *db);
size_t
apt_dat_find_by_icao(const airport_db_t *db, const char *icao);
void
apt_dat_airport_verify(const airport_info_t *apt);

#ifdef __cplusplus
}
#endif

#endif /* APT_DAT_H_ */