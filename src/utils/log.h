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

#ifdef __cplusplus
}
#endif

#endif /* LOG_H_ */