/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "window.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdbool.h>
#include <utils/log.h>

static bool global_init_called = false;

typedef struct window_cb_info {
    bool  flag;
    void *udata;
} window_cb_info_t;

struct window_inst {
    GLFWwindow *glfw_window;
    void (*mouse_pos_cb)(double xpos, double ypos, void *udata);
    void (*mouse_button_cb)(bool mouse_down, bool mouse_hold, void *udata);
    void (*window_loop_cb)(window_inst_t *window, void *udata);
    void (*mouse_scroll_cb)(int mouse_scroll, void *udata);
    window_cb_info_t set_mouse_pos_cb;
    window_cb_info_t set_mouse_button_cb;
    window_cb_info_t set_window_loop_cb;
    window_cb_info_t set_mouse_scroll_cb;
};

static void
window_glfw_error_callback(int error, const char *description) {
    UNUSED(error);
    log_err("glfw error: %s", description);
}

static void
window_mouse_pos_callback(GLFWwindow *window, double xpos, double ypos) {
    ASSERT(window != NULL);

    window_inst_t *us = (window_inst_t *)glfwGetWindowUserPointer(window);
    ASSERT(us != NULL);

    if (!us->set_mouse_pos_cb.flag) {
        return;
    }

    if (isnan(xpos) || isnan(ypos)) {
        return;
    }

    us->mouse_pos_cb(xpos, ypos, us->set_mouse_pos_cb.udata);
}

void
window_set_mouse_pos_callback(
    window_inst_t *wind, void (*cb)(double xpos, double ypos, void *ud), void *udata) {
    ASSERT(wind != NULL);
    wind->mouse_pos_cb = cb;
    wind->set_mouse_pos_cb.flag = true;
    wind->set_mouse_pos_cb.udata = udata;
}

static void
window_mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    ASSERT(window != NULL);
    UNUSED(mods);

    window_inst_t *us = (window_inst_t *)glfwGetWindowUserPointer(window);
    ASSERT(us != NULL);

    if (!us->set_mouse_button_cb.flag) {
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        us->mouse_button_cb(true, true, us->set_mouse_button_cb.udata);
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        us->mouse_button_cb(false, false, us->set_mouse_button_cb.udata);
    }
}

void
window_set_mouse_button_callback(
    window_inst_t *wind, void (*cb)(bool mouse_down, bool mouse_hold, void *ud), void *udata) {
    ASSERT(wind != NULL);
    wind->mouse_button_cb = cb;
    wind->set_mouse_button_cb.flag = true;
    wind->set_mouse_button_cb.udata = udata;
}

static void
window_mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    ASSERT(window != NULL);
    UNUSED(xoffset);
    int            scroll;

    window_inst_t *us = (window_inst_t *)glfwGetWindowUserPointer(window);
    ASSERT(us != NULL);

    if (!us->set_mouse_scroll_cb.flag) {
        return;
    }

    if (yoffset > 0) {
        scroll = 1;
    } else if (yoffset < 0) {
        scroll = -1;
    } else {
        scroll = 0;
    }

    us->mouse_scroll_cb(scroll, us->set_mouse_scroll_cb.udata);
}

void
window_set_mouse_scroll_callback(
    window_inst_t *wind, void (*cb)(int mouse_scroll, void *udata), void *udata) {
    ASSERT(wind != NULL);
    wind->mouse_scroll_cb = cb;
    wind->set_mouse_scroll_cb.flag = true;
    wind->set_mouse_scroll_cb.udata = udata;
}

void
window_graphics_global_init() {
    glfwSetErrorCallback(window_glfw_error_callback);
    if (!glfwInit()) {
        log_err("Failed to initialize glfw");
        exit(EXIT_FAILURE);
    }
    global_init_called = true;
}

void
window_loop(window_inst_t *window) {
    ASSERT(window != NULL);

    while (!glfwWindowShouldClose(window->glfw_window)) {
        int win_width, win_height;
        glfwGetFramebufferSize(window->glfw_window, &win_width, &win_height);

        glViewport(0, 0, win_width, win_height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, win_width, win_height, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);

        if (window->set_window_loop_cb.flag) {
            window->window_loop_cb(window, window->set_window_loop_cb.udata);
        }

        glfwSwapBuffers(window->glfw_window);
        glfwPollEvents();
    }
}

void
window_set_window_loop_callback(
    window_inst_t *window, void (*cb)(window_inst_t *window, void *ud), void *udata) {
    ASSERT(window != NULL);
    window->window_loop_cb = cb;
    window->set_window_loop_cb.flag = true;
    window->set_window_loop_cb.udata = udata;
}

window_inst_t *
window_create(const char *title, int w, int h) {
    ASSERT(global_init_called);
    window_inst_t *wind;

    wind = malloc(sizeof(*wind));

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    wind->glfw_window = glfwCreateWindow(w, h, title, NULL, NULL);
    if (wind->glfw_window == NULL) {
        log_err("Failed to create glfw window");
        exit(EXIT_FAILURE);
    }
    wind->set_mouse_pos_cb.flag = false;
    wind->set_mouse_button_cb.flag = false;
    wind->set_window_loop_cb.flag = false;

    glfwSetCursorPosCallback(wind->glfw_window, window_mouse_pos_callback);
    glfwSetMouseButtonCallback(wind->glfw_window, window_mouse_button_callback);
    glfwSetScrollCallback(wind->glfw_window, window_mouse_scroll_callback);

    glfwMakeContextCurrent(wind->glfw_window);
    glfwSwapInterval(1);
    glfwSetWindowUserPointer(wind->glfw_window, wind);

    return wind;
}

void *
window_destroy(window_inst_t *window) {
    ASSERT(window != NULL);
    ASSERT(window->glfw_window != NULL);

    glfwDestroyWindow(window->glfw_window);
    free(window);

    return NULL;
}

void
window_graphics_global_destroy() {
    ASSERT(global_init_called);
    glfwTerminate();
}