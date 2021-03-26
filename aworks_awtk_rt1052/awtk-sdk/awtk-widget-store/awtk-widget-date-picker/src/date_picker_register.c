/**
 * File:   date_picker.c
 * Author: AWTK Develop Team
 * Brief:  日期选择控件。
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
 * 2020-07-12 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "tkc/utils.h"
#include "date_picker_register.h"
#include "base/widget_factory.h"
#include "date_picker/date_edit.h"
#include "date_picker/date_picker.h"

ret_t date_picker_register(void) {
  widget_factory_register(widget_factory(), WIDGET_TYPE_DATE_EDIT, date_edit_create);
  return widget_factory_register(widget_factory(), WIDGET_TYPE_DATE_PICKER, date_picker_create);
}

const char* date_picker_supported_render_mode(void) {
  return "OpenGL|AGGE-BGR565|AGGE-BGRA8888|AGGE-MONO";
}
