﻿/**
 * File:   window_animator_slide_down.c
 * Author: AWTK Develop Team
 * Brief:  slide_down window animator
 *
 * Copyright (c) 2018 - 2021  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2019-03-28 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "window_animators/window_animator_common.h"
#include "window_animators/window_animator_slide.h"
#include "window_animators/window_animator_slide_down.h"

static const window_animator_vtable_t s_window_animator_slide_down_vt = {
    .overlap = TRUE,
    .type = "slide_down",
    .desc = "slide_down",
    .size = sizeof(window_animator_slide_t),
    .init = window_animator_slide_init,
    .draw_prev_window = window_animator_slide_draw_prev,
    .draw_curr_window = window_animator_to_bottom_draw_curr};

window_animator_t* window_animator_slide_down_create(bool_t open, tk_object_t* args) {
  return window_animator_slide_create(open, args, &s_window_animator_slide_down_vt);
}
