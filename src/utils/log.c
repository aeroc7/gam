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

#include "log.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

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