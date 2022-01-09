/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef GL_PBO_H_
#define GL_PBO_H_

#include <GL/glew.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pbo_hdlr pbo_hdlr_t;

pbo_hdlr_t *
gl_pbo_create(GLsizei width, GLsizei height);
void *
gl_pbo_get_back_buffer(pbo_hdlr_t *pbo);
void
gl_pbo_finish_back_buffer(pbo_hdlr_t *pbo);
void
gl_pbo_bind_front_buffer(pbo_hdlr_t *pbo);
void *
gl_pbo_destroy(pbo_hdlr_t *pbo);
#ifdef __cplusplus
}
#endif

#endif /* GL_PBO_H_ */