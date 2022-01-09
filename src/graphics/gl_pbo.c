/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "gl_pbo.h"

#include <pthread.h>
#include <stdbool.h>
#include <utils/log.h>

struct pbo_hdlr {
    GLsizei width;
    GLsizei height;
    void   *cur_buffer_handle;
    GLuint  pbo_bufs[2];

    struct {
        GLuint back_buf_index;
        GLuint front_buf_index;
        bool   back_buffer_ready;
        bool   back_buffer_done;
    } mt;

    pthread_mutex_t mutex;
};

pbo_hdlr_t *
gl_pbo_create(GLsizei width, GLsizei height) {
    pbo_hdlr_t *pbo;

    pbo = malloc(sizeof(*pbo));

    pbo->width = width;
    pbo->height = height;

    glGenBuffers(2, pbo->pbo_bufs);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo->pbo_bufs[0]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * 4, NULL, GL_DYNAMIC_DRAW);

    pbo->cur_buffer_handle = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    pbo->mt.back_buf_index = 0;
    pbo->mt.front_buf_index = 1;
    pbo->mt.back_buffer_ready = true;
    pbo->mt.back_buffer_done = false;

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo->pbo_bufs[1]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * 4, NULL, GL_DYNAMIC_DRAW);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    pthread_mutex_init(&pbo->mutex, NULL);

    return pbo;
}

void *
gl_pbo_get_back_buffer(pbo_hdlr_t *pbo) {
    ASSERT(pbo != NULL);
    void *buf_ret = NULL;

    pthread_mutex_lock(&pbo->mutex);
    if (pbo->mt.back_buffer_ready) {
        buf_ret = pbo->cur_buffer_handle;
    }
    pthread_mutex_unlock(&pbo->mutex);

    return buf_ret;
}

void
gl_pbo_finish_back_buffer(pbo_hdlr_t *pbo) {
    ASSERT(pbo != NULL);
    pthread_mutex_lock(&pbo->mutex);
    pbo->mt.back_buffer_done = true;
    pbo->mt.back_buffer_ready = false;
    pthread_mutex_unlock(&pbo->mutex);
}

void
gl_pbo_bind_front_buffer(pbo_hdlr_t *pbo) {
    ASSERT(pbo != NULL);

    bool   us_back_buf_done;
    bool   us_back_buf_ready;
    GLuint us_back_index;
    GLuint us_front_index;

    pthread_mutex_lock(&pbo->mutex);
    us_back_buf_done = pbo->mt.back_buffer_done;
    us_back_buf_ready = pbo->mt.back_buffer_ready;
    us_back_index = pbo->mt.back_buf_index;
    us_front_index = pbo->mt.front_buf_index;
    pthread_mutex_unlock(&pbo->mutex);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo->pbo_bufs[us_front_index]);

    if (us_back_buf_done) {
        glTexSubImage2D(
            GL_TEXTURE_2D, 0, 0, 0, pbo->width, pbo->height, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo->pbo_bufs[us_back_index]);
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

        us_back_index = (us_back_index + 1) % 2;
        us_front_index = (us_front_index + 1) % 2;

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo->pbo_bufs[us_back_index]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, pbo->width * pbo->height * 4, NULL, GL_DYNAMIC_DRAW);
        pbo->cur_buffer_handle = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

        us_back_buf_done = false;
        us_back_buf_ready = true;

        pthread_mutex_lock(&pbo->mutex);
        pbo->mt.back_buffer_done = us_back_buf_done;
        pbo->mt.back_buffer_ready = us_back_buf_ready;
        pbo->mt.back_buf_index = us_back_index;
        pbo->mt.front_buf_index = us_front_index;
        pthread_mutex_unlock(&pbo->mutex);
    }

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void *
gl_pbo_destroy(pbo_hdlr_t *pbo) {
    ASSERT(pbo != NULL);
    glDeleteBuffers(2, pbo->pbo_bufs);
    pbo->cur_buffer_handle = NULL;
    pthread_mutex_destroy(&pbo->mutex);
    free(pbo);

    return NULL;
}