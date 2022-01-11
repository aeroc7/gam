/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LOG_H_
#define LOG_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void
log_msg_(char *fmt, ...);
void
log_err_(char *fmt, ...);

#define log_msg(...)     log_msg_(__VA_ARGS__)
#define log_err(...)     log_err_(__VA_ARGS__)

#define UNUSED(a)        (void)(a)
#define COND_UNLIKELY(x) __builtin_expect(x, 0)

#define ASSERT(x)                               \
    do {                                        \
        if (COND_UNLIKELY(!(x))) {              \
            log_err("Assertion %s failed", #x); \
            abort();                            \
        }                                       \
    } while (0)

#define VRET0(x) ASSERT(x() == 0)
#define VRET1(x) ASSERT(x() == 1)

#ifdef __cplusplus
}
#endif

#endif /* LOG_H_ */