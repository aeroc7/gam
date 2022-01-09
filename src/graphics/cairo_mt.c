/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cairo_mt.h"

#include <GL/glew.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utils/log.h>
#include <utils/utils.h>

#include "gl_pbo.h"

struct cairo_mt {
    int              width;
    int              height;
    unsigned         gl_tex_id;

    cairo_surface_t *surface;
    cairo_t         *cr;
    pbo_hdlr_t      *pbo;

    void (*start)(cairo_t *cr);
    int (*loop)(cairo_t *cr);
    void (*end)(cairo_t *cr);
    bool            callbacks_set;

    pthread_t       thread;
    pthread_mutex_t mutex;
    bool            thread_started;
};

static GLuint
cairo_mt_gen_gl_texture(int width, int height) {
    GLuint tex_id;

    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    return tex_id;
}

cairo_mt_t *
cairo_mt_create(int width, int height) {
    cairo_mt_t *cmt;

    cmt = malloc(sizeof(*cmt));

    cmt->width = width;
    cmt->height = height;
    cmt->gl_tex_id = cairo_mt_gen_gl_texture(width, height);
    cmt->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cmt->cr = cairo_create(cmt->surface);
    cmt->pbo = gl_pbo_create(width, height);

    pthread_mutex_init(&cmt->mutex, NULL);

    cmt->callbacks_set = false;
    cmt->thread_started = false;

    return cmt;
}

static void *
cairo_mt_thread(void *arg) {
    ASSERT(arg != NULL);
    cairo_mt_t *cmt = (cairo_mt_t *)arg;

    cmt->start(cmt->cr);

    while (1) {
        long time_start = utils_gettime();

        /* Clear surface */
        cairo_set_source_rgb(cmt->cr, 0, 0, 0);
        cairo_paint(cmt->cr);

        if (cmt->loop(cmt->cr) == 1) {
            break;
        }

        cairo_surface_flush(cmt->surface);
        const int stride = cairo_image_surface_get_stride(cmt->surface);
        void     *surf_data = cairo_image_surface_get_data(cmt->surface);
        void     *pbo_buffer = gl_pbo_get_back_buffer(cmt->pbo);

        if (surf_data != NULL && pbo_buffer != NULL) {
            memcpy(pbo_buffer, surf_data, (size_t)(stride * cmt->height));
            gl_pbo_finish_back_buffer(cmt->pbo);
        }

        long time_end = utils_gettime();
        long time_micros = (time_end - time_start) / 1000;
        ASSERT(time_micros < 33000l);
        usleep((unsigned)(33000l - time_micros));
    }

    cmt->end(cmt->cr);

    pthread_exit(NULL);
}

void
cairo_mt_start(cairo_mt_t *cmt) {
    ASSERT(cmt != NULL);
    ASSERT(cmt->callbacks_set);
    cmt->thread_started = true;

    pthread_create(&cmt->thread, NULL, cairo_mt_thread, (void *)cmt);
}

void
cairo_mt_set_callbacks(cairo_mt_t *cmt, void (*start)(cairo_t *cr), int (*loop)(cairo_t *cr),
    void (*end)(cairo_t *cr)) {
    ASSERT(cmt != NULL);

    cmt->start = start;
    cmt->loop = loop;
    cmt->end = end;
    cmt->callbacks_set = true;
}

void
cairo_mt_draw(cairo_mt_t *cmt) {
    ASSERT(cmt != NULL);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cmt->gl_tex_id);
    glClearColor(1.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    gl_pbo_bind_front_buffer(cmt->pbo);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2i(0, 0);
    glTexCoord2f(0, 1);
    glVertex2i(0, cmt->height);
    glTexCoord2f(1, 1);
    glVertex2i(cmt->width, cmt->height);
    glTexCoord2f(1, 0);
    glVertex2i(cmt->width, 0);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void *
cairo_mt_destroy(cairo_mt_t *cmt) {
    ASSERT(cmt != NULL);
    if (cmt->thread_started) {
        pthread_join(cmt->thread, NULL);
    }

    cairo_destroy(cmt->cr);
    cairo_surface_destroy(cmt->surface);
    gl_pbo_destroy(cmt->pbo);
    pthread_mutex_destroy(&cmt->mutex);
    free(cmt);

    return NULL;
}