/**
 * File:   mask_view_register.c
 * Author: AWTK Develop Team
 * Brief:  前景为蒙板裁剪裁剪背景色的功能
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
 * 2020-07-22 Luo ZhiMing <luozhiming@zlg.cn> created
 *
 */

#include "tkc/mem.h"
#include "tkc/utils.h"
#include "mask_view_register.h"
#include "base/widget_factory.h"
#include "mask_view/mask_view.h"
#include "mask_view/mask_view_item.h"

ret_t mask_view_register(void) {
  widget_factory_register(widget_factory(), WIDGET_TYPE_MASK_VIEW, mask_view_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_MASK_VIEW_ITEM, mask_view_item_create);
  return RET_OK;
}

const char* mask_view_supported_render_mode(void) {
  return "OpenGL|AGGE-BGR565|AGGE-BGRA8888|AGGE-MONO";
}