/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "log.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LOG_STATS_FMT "%i:%i:%i [%s:%i]: "

static void
log_out(FILE *loc, int line, const char *file, char *fmt, va_list ap) {
    va_list    ap2;
    int        msg_len, info_len;
    char      *buf = NULL;
    time_t     rtime;
    struct tm *timeinfo;

    va_copy(ap2, ap);

    time(&rtime);
    timeinfo = localtime(&rtime);
    info_len = snprintf(
        NULL, 0, LOG_STATS_FMT, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, file, line);
    assert(info_len > 0);
    msg_len = vsnprintf(NULL, 0, fmt, ap);
    assert(msg_len > 0);
    buf = malloc((sizeof(char) * (unsigned)(info_len + msg_len)) + 1);

    int wbytes = snprintf(buf, (size_t)info_len + 1, LOG_STATS_FMT, timeinfo->tm_hour,
        timeinfo->tm_min, timeinfo->tm_sec, file, line);
    assert(wbytes > 0);
    char *end_buf = buf + wbytes;
    vsnprintf(end_buf, (size_t)msg_len + 1, fmt, ap2);

    fprintf(loc, "%s\n", buf);

    free(buf);
    va_end(ap2);
}

void
log_err_(int line, const char *file, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    log_out(stderr, line, file, fmt, ap);
    va_end(ap);
}

void
log_msg_(int line, const char *file, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    log_out(stdout, line, file, fmt, ap);
    va_end(ap);
}