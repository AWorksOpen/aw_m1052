/**
 * File:   label_rotate.c
 * Author: AWTK Develop Team
 * Brief:  可旋转 label 控件
 *
 * Copyright (c) 2020 - 2020 Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2020-06-16 Luo ZhiMing <luozhiming@zlg.cn> created
 *
 */

#include "tkc/mem.h"
#include "tkc/utils.h"
#include "label_rotate_register.h"
#include "base/widget_factory.h"
#include "label_rotate/label_rotate.h"

ret_t label_rotate_register(void) {
  return widget_factory_register(widget_factory(), WIDGET_TYPE_LABEL_ROTATE, label_rotate_create);
}

const char* label_rotate_supported_render_mode(void) {
  return "OpenGL|AGGE-BGR565|AGGE-BGRA8888|AGGE-MONO";
}