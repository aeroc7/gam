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

static void
log_out(FILE *loc, char *fmt, va_list ap) {
    va_list ap2;
    int     len;
    char   *buf = NULL;

    va_copy(ap2, ap);
    len = vsnprintf(buf, 0, fmt, ap) + 1;
    assert(len > 0);
    buf = malloc((sizeof(char) * (unsigned)len) + 1);
    vsnprintf(buf, (size_t)len, fmt, ap2);
    buf[len] = '\0';
    fprintf(loc, "%s\n", buf);

    free(buf);
    va_end(ap2);
}

void
log_err(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    log_out(stderr, fmt, ap);
    va_end(ap);
}

void
log_msg(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    log_out(stdout, fmt, ap);
    va_end(ap);
}