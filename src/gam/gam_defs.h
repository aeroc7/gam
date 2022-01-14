/**
 * Copyright 2022 Bennett Anderson
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef GAM_DEFS_H_
#define GAM_DEFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define GAM_WINDOW_TITLE                "GAM"
#define GAM_WINDOW_WIDTH                738
#define GAM_WINDOW_HEIGHT               520
#define GAM_WINDOW_RENDER_FPS_TGT       120 /* FPS */

#define GAM_UI_BG_COLOR                 0x242424
#define GAM_UI_PANEL_COLOR              0x2f2f2f
#define GAM_UI_MAIN_TEXT_COLOR          0xffffff
#define GAM_UI_GLOBAL_BORDER            15 /* Pixels */

#define GAM_UI_APT_RUNWAY_COLOR         0x0973D6
#define GAM_UI_APT_BOUNDS_COLOR         0x475159
#define GAM_UI_APT_PAVE_BOUNDS_COLOR    0x495B6B

#define GAM_UI_APT_RUNWAY_WIDTH_DEFAULT 2.0 /* Pixels */

#define GAM_UI_APT_DRAW_SIZE_W          (GAM_WINDOW_WIDTH / 2.0)
#define GAM_UI_APT_DRAW_SIZE_H          (GAM_WINDOW_HEIGHT / 2.0)
#define GAM_UI_APT_CONTENT_PANEL_W      (GAM_WINDOW_WIDTH - (GAM_UI_GLOBAL_BORDER * 2))
#define GAM_UI_APT_CONTENT_PANEL_H      (GAM_WINDOW_HEIGHT - (GAM_UI_GLOBAL_BORDER * 4))
#define GAM_UI_APT_CONTENT_PANEL_R      10

#ifdef __cplusplus
}
#endif

#endif /* GAM_DEFS_H_ */