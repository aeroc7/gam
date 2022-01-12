/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
    bool          airport_bb_open;
    bool          airport_pavement_open;
    bool          last_was_pave_open;
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
            continue;
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

static bool
apt_dat_check_comment(const char *line) {
    return (strlen(line) > 2) ? (line[0] == '#' && line[1] == '#') : false;
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

/* Land runways */
static void
apt_dat_handle_100(const char *line, airport_info_t *ap_info) {
    ap_info->runways_size += 1;
    ap_info->runways = realloc(ap_info->runways, sizeof(*ap_info->runways) * ap_info->runways_size);
    const size_t rwy_idx = ap_info->runways_size - 1;

    ap_info->runways[rwy_idx].width = apt_dat_str_to_double(line, 1);

    /* Names */
    for (unsigned i = 0; i < 2; ++i) {
        char        *nm = utils_str_split_at(line, 8 + (i * 9));
        const size_t max_sz = sizeof(ap_info->runways[rwy_idx].name[i]);

        strncpy(ap_info->runways[rwy_idx].name[i], nm, max_sz - 1);
        ap_info->runways[rwy_idx].name[i][max_sz - 1] = '\0';

        free(nm);
    }

    /* Lat & lon */
    for (unsigned i = 0; i < 2; ++i) {
        ap_info->runways[rwy_idx].latitude[i] = apt_dat_str_to_double(line, 9 + (i * 9));
        ap_info->runways[rwy_idx].longitude[i] = apt_dat_str_to_double(line, 10 + (i * 9));
    }
}

static void
apt_dat_handle_130(const char *line, airport_info_t *ap_info) {
    double lat_val = apt_dat_str_to_double(line, 1);
    double lon_val = apt_dat_str_to_double(line, 2);
    vector_push(ap_info->boundaries.latitude, &lat_val);
    vector_push(ap_info->boundaries.longitude, &lon_val);
}

static void
apt_dat_handle_110(const char *line, airport_info_t *ap_info, bool new_node) {
    if (new_node) {
        UNUSED(ap_info);
    }

    UNUSED(line);
}

static int
apt_dat_gather_ap_info(const char *line, void *udata) {
    ASSERT(udata != NULL);
    /* Skip if line is a comment */
    if (apt_dat_check_comment(line)) {
        return 1;
    }

    gather_ap_data_t *gapt = (gather_ap_data_t *)udata;
    const long        row_code = apt_dat_handle_rowcode(line);

    if (row_code == 1) {
        gapt->ap_db->airports_size += 1;
    }
    const size_t true_apt_index = gapt->ap_db->airports_size - 1;

    /* Don't bother continuing if we don't have a valid airport. */
    if (row_code != 1 && gapt->has_airport == false) {
        return 1;
    }

    switch (row_code) {
        case 1: /* Land airport*/
            apt_dat_handle_1(line, &gapt->ap_db->airports[true_apt_index]);
            gapt->has_airport = true;
            break;
        case 16: /* Seaplane base */
        case 17: /* Heliport */
            gapt->has_airport = false;
            break;
        case 100: /* Runway */
            apt_dat_handle_100(line, &gapt->ap_db->airports[true_apt_index]);
            break;
        case 110: /* Pavement (taxiway or ramp) header */
            gapt->airport_pavement_open = true;
            gapt->last_was_pave_open = true;
            break;
        case 111: /* Node */
        case 112: /* Node with bezier control point */
            if (gapt->airport_bb_open) {
                apt_dat_handle_130(line, &gapt->ap_db->airports[true_apt_index]);
            } else if (gapt->airport_pavement_open) {
                apt_dat_handle_110(
                    line, &gapt->ap_db->airports[true_apt_index], gapt->last_was_pave_open);
                gapt->last_was_pave_open = false;
            }
            break;
        case 113: /* Node with implicit close of loop */
        case 114: /* Node with Bezier control point, with implicit close of loop */
            /* Close airport boundary reading if open */
            if (gapt->airport_bb_open) {
                apt_dat_handle_130(line, &gapt->ap_db->airports[true_apt_index]);
                gapt->airport_bb_open = false;
            } else if (gapt->airport_pavement_open) {
                apt_dat_handle_110(line, &gapt->ap_db->airports[true_apt_index], false);
                gapt->airport_pavement_open = false;
            } else {
                apt_dat_handle_110(line, &gapt->ap_db->airports[true_apt_index], true);
            }
            break;
        case 130: /* Airport boundary header */
            gapt->airport_bb_open = true;
            break;
        case 1302: /* Airport metadata */
            apt_dat_handle_1302(line, &gapt->ap_db->airports[true_apt_index]);
            break;
    }

    return 1;
}

airport_db_t *
apt_dat_airport_db_create(size_t num_airports) {
    airport_db_t *adb;

    ASSERT(num_airports > 0);

    adb = malloc(sizeof(*adb));
    adb->airports = malloc(sizeof(airport_info_t) * num_airports);
    adb->airports_size = 0;

    for (size_t i = 0; i < num_airports; ++i) {
        adb->airports[i].icao = NULL;
        adb->airports[i].name = NULL;
        adb->airports[i].city = NULL;
        adb->airports[i].country = NULL;
        adb->airports[i].state = NULL;
        adb->airports[i].latitude = 0;
        adb->airports[i].longitude = 0;
        adb->airports[i].runways = NULL;
        adb->airports[i].runways_size = 0;
        adb->airports[i].boundaries.latitude = vector_create(sizeof(double), 2);
        adb->airports[i].boundaries.longitude = vector_create(sizeof(double), 2);
        adb->airports[i].pave_bounds = vector_create(sizeof(airport_bounds_t), 0);
    }

    return adb;
}

airport_db_t *
apt_dat_parse(const char **files, size_t size) {
    size_t num_airports = 0;

    apt_dat_file_read(files, size, (void *)&num_airports, apt_dat_count_airports);

    if (num_airports == 0) {
        log_err("Couldn't find any airports");
        return NULL;
    }

    gather_ap_data_t airport_gather = {.has_airport = false,
        .airport_bb_open = false,
        .airport_pavement_open = false,
        .last_was_pave_open = false};

    airport_gather.ap_db = apt_dat_airport_db_create(num_airports);

    apt_dat_file_read(files, size, (void *)&airport_gather.ap_db, apt_dat_gather_ap_info);

    for (size_t i = 0; i < airport_gather.ap_db->airports_size; ++i) {
        if (strcmp(airport_gather.ap_db->airports[i].icao, "KLAX") == 0) {
            log_msg("Name: %s", airport_gather.ap_db->airports[i].name);
            log_msg("City: %s", airport_gather.ap_db->airports[i].city);
            log_msg("Country: %s", airport_gather.ap_db->airports[i].country);
            log_msg("State: %s", airport_gather.ap_db->airports[i].state);
            log_msg("Lat & lon %lf %lf", airport_gather.ap_db->airports[i].latitude,
                airport_gather.ap_db->airports[i].longitude);
            log_msg("Airport rwy2 name: %s", airport_gather.ap_db->airports[i].runways[1].name[1]);
            log_msg("Airport rwy2 lat & lon: %lf %lf",
                airport_gather.ap_db->airports[i].runways[1].latitude[1],
                airport_gather.ap_db->airports[i].runways[1].longitude[1]);
        }
    }

    return airport_gather.ap_db;
}

/* Will exit if it doesn't find it */
size_t
apt_dat_find_by_icao(const airport_db_t *db, const char *icao) {
    ASSERT(db != NULL);
    ASSERT(icao != NULL);

    for (size_t i = 0; i < db->airports_size; ++i) {
        if (strcmp(icao, db->airports[i].icao) == 0) {
            return i;
        }
    }

    log_err("Couldn't find airport");
    exit(EXIT_FAILURE);
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
        free(db->airports[i].runways);
        db->airports[i].boundaries.latitude = vector_destroy(db->airports[i].boundaries.latitude);
        db->airports[i].boundaries.longitude = vector_destroy(db->airports[i].boundaries.longitude);
        db->airports[i].pave_bounds = vector_destroy(db->airports[i].pave_bounds);
    }

    free(db->airports);
    free(db);

    return NULL;
}