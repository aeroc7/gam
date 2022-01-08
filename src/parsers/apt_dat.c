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

#include "apt_dat.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <utils/log.h>
#include <utils/path_hdlr.h>
#include <utils/utils.h>

#define AIRPORT_ROW_CODE 1

typedef struct gather_ap_data {
    airport_db_t *ap_db;
    bool          has_airport;
} gather_ap_data_t;

static void
apt_dat_file_read(const char **files, size_t files_size, void *user_data,
    int (*on_line)(const char *line, void *udata)) {
    for (size_t i = 0; i < files_size; ++i) {
        char  *new_file_path;
        char  *line_buf = NULL;
        FILE  *fp;
        size_t line_size;

        new_file_path = path_hdlr_convert_to_native(files[i]);
        fp = fopen(new_file_path, "r");

        if (fp == NULL) {
            log_err("Failed to open %s", new_file_path);
            free(new_file_path);
            return;
        }

        while (getline(&line_buf, &line_size, fp) != -1) {
            utils_strip_newline(line_buf);
            if (on_line(line_buf, user_data) == -1) {
                break;
            }
        }

        fclose(fp);
        free(new_file_path);
        free(line_buf);
    }

    return;
}

/* Row code */
static long
apt_dat_handle_rowcode(const char *line) {
    char *row_code_s;
    long  row_code;

    if (strlen(line) == 0) {
        return -1;
    }

    row_code_s = utils_str_split_at(line, 0);
    row_code = strtol(row_code_s, NULL, 10);
    free(row_code_s);

    return row_code;
}

static int
apt_dat_count_airports(const char *line, void *udata) {
    ASSERT(udata != NULL);
    size_t *airport_ctr = (size_t *)udata;
    long    row_code;

    row_code = apt_dat_handle_rowcode(line);

    if (row_code == 1) {
        *airport_ctr += 1;
    }

    return 1;
}

static double
apt_dat_str_to_double(const char *line, unsigned index) {
    char  *splt_str;
    double result;

    splt_str = utils_str_split_at(line, index);
    result = strtod(splt_str, NULL);
    free(splt_str);

    return result;
}

/* Land airport */
static void
apt_dat_handle_1(const char *line, airport_info_t *ap_info) {
    ap_info->icao = utils_str_split_at(line, 4);
    ap_info->name = utils_str_split_after(line, 5);
}

/* Airport Metadata */
static void
apt_dat_handle_1302(const char *line, airport_info_t *ap_info) {
    char *mdata = utils_str_split_at(line, 1);

    if (strcmp(mdata, "city") == 0) {
        ap_info->city = utils_str_split_after(line, 2);
    } else if (strcmp(mdata, "country") == 0) {
        ap_info->country = utils_str_split_after(line, 2);
    } else if (strcmp(mdata, "state") == 0) {
        ap_info->state = utils_str_split_after(line, 2);
    } else if (strcmp(mdata, "datum_lat") == 0) {
        ap_info->latitude = apt_dat_str_to_double(line, 2);
    } else if (strcmp(mdata, "datum_lon") == 0) {
        ap_info->longitude = apt_dat_str_to_double(line, 2);
    }

    free(mdata);
}

static int
apt_dat_gather_ap_info(const char *line, void *udata) {
    ASSERT(udata != NULL);

    gather_ap_data_t *gapt = (gather_ap_data_t *)udata;
    const long        row_code = apt_dat_handle_rowcode(line);

    if (row_code == 1) {
        gapt->ap_db->airports_size += 1;
    }

    const size_t true_apt_index = gapt->ap_db->airports_size - 1;

    switch (row_code) {
        case 1:
            apt_dat_handle_1(line, &gapt->ap_db->airports[true_apt_index]);
            gapt->has_airport = true;
            break;
        case 16:
        case 17:
            gapt->has_airport = false;
            break;
        case 1302:
            if (gapt->has_airport) {
                apt_dat_handle_1302(line, &gapt->ap_db->airports[true_apt_index]);
            }
            break;
    }

    return 1;
}

airport_db_t *
apt_dat_parse(const char **files, size_t size) {
    size_t num_airports = 0;

    apt_dat_file_read(files, size, (void *)&num_airports, apt_dat_count_airports);

    if (num_airports == 0) {
        log_err("Couldn't find any airports");
        return NULL;
    }

    gather_ap_data_t airport_gather = {.has_airport = false};

    airport_gather.ap_db = malloc(sizeof(airport_db_t));
    airport_gather.ap_db->airports = malloc(sizeof(airport_info_t) * num_airports);
    airport_gather.ap_db->airports_size = 0;

    for (size_t i = 0; i < num_airports; ++i) {
        airport_gather.ap_db->airports[i].icao = NULL;
        airport_gather.ap_db->airports[i].name = NULL;
        airport_gather.ap_db->airports[i].city = NULL;
        airport_gather.ap_db->airports[i].country = NULL;
        airport_gather.ap_db->airports[i].state = NULL;
        airport_gather.ap_db->airports[i].latitude = 0;
        airport_gather.ap_db->airports[i].longitude = 0;
    }

    apt_dat_file_read(files, size, (void *)&airport_gather.ap_db, apt_dat_gather_ap_info);

    for (size_t i = 0; i < airport_gather.ap_db->airports_size; ++i) {
        if (strcmp(airport_gather.ap_db->airports[i].icao, "KSEA") == 0) {
            log_msg("Name: %s", airport_gather.ap_db->airports[i].name);
            log_msg("City: %s", airport_gather.ap_db->airports[i].city);
            log_msg("Country: %s", airport_gather.ap_db->airports[i].country);
            log_msg("State: %s", airport_gather.ap_db->airports[i].state);
            log_msg("Lat & lon %lf %lf", airport_gather.ap_db->airports[i].latitude,
                airport_gather.ap_db->airports[i].longitude);
        }
    }

    return airport_gather.ap_db;
}

void *
apt_dat_db_free(airport_db_t *db) {
    ASSERT(db != NULL);
    ASSERT(db->airports != NULL);

    for (size_t i = 0; i < db->airports_size; ++i) {
        free(db->airports[i].name);
        free(db->airports[i].city);
        free(db->airports[i].country);
        free(db->airports[i].state);
        free(db->airports[i].icao);
    }

    free(db->airports);
    free(db);

    return NULL;
}