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

#ifndef UTILS_H_
#define UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Does nothing more than replace with null-terminator */
void
utils_strip_newline(char *str);
char *
utils_strdup(const char *str);

/* These operate on spaces: " " */
char *
utils_str_split_at(const char *data, unsigned index);
char *
utils_str_split_after(const char *data, unsigned index);

#ifdef __cplusplus
}
#endif

#endif /* UTILS_H_ */