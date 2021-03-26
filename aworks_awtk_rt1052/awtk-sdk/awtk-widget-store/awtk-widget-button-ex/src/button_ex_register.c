/**
 * File:   button_ex.c
 * Author: AWTK Develop Team
 * Brief:  扩展按钮。
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
 * 2020-07-22 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "tkc/utils.h"
#include "button_ex_register.h"
#include "base/widget_factory.h"
#include "button_ex/button_ex.h"

ret_t button_ex_register(void) {
  return widget_factory_register(widget_factory(), WIDGET_TYPE_BUTTON_EX, button_ex_create);
}

const char* button_ex_supported_render_mode(void) {
  return "OpenGL|AGGE-BGR565|AGGE-BGRA8888|AGGE-MONO";
}
