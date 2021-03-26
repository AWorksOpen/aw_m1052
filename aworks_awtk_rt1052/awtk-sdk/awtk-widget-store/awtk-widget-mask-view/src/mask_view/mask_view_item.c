/**
 * File:   mask_view_item.c
 * Author: AWTK Develop Team
 * Brief:  mask_view 控件的采样区域
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
#include "mask_view_item.h"

static ret_t mask_view_item_get_prop(widget_t* widget, const char* name, value_t* v) {
  mask_view_item_t* mask_view_item = MASK_VIEW_ITEM(widget);
  return_value_if_fail(mask_view_item != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(MASK_VIEW_PROP_MASK_VIEW, name)) {
    value_set_bool(v, mask_view_item->mask_view);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t mask_view_item_set_prop(widget_t* widget, const char* name, const value_t* v) {
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(MASK_VIEW_PROP_MASK_VIEW, name)) {
    return mask_view_item_set_mask_view(widget, value_bool(v));
  } 

  return RET_NOT_FOUND;
}

ret_t mask_view_item_set_mask_view(widget_t* widget, bool_t mask_view) {
  mask_view_item_t* mask_view_item = MASK_VIEW_ITEM(widget);
  return_value_if_fail(mask_view_item != NULL, RET_BAD_PARAMS);

  mask_view_item->mask_view = mask_view;

  return RET_OK;
}

bool_t mask_view_item_get_mask_view(widget_t* widget) {
  mask_view_item_t* mask_view_item = MASK_VIEW_ITEM(widget);
  return_value_if_fail(mask_view_item != NULL, RET_BAD_PARAMS);

  return mask_view_item->mask_view;
}

static ret_t mask_view_item_on_destroy(widget_t* widget) {
  mask_view_item_t* mask_view_item = MASK_VIEW_ITEM(widget);
  return_value_if_fail(widget != NULL && mask_view_item != NULL, RET_BAD_PARAMS);

  return RET_OK;
}

static ret_t mask_view_item_on_paint_self(widget_t* widget, canvas_t* c) {
  mask_view_item_t* mask_view_item = MASK_VIEW_ITEM(widget);

  (void)mask_view_item;

  return RET_OK;
}

static ret_t mask_view_item_on_event(widget_t* widget, event_t* e) {
  mask_view_item_t* mask_view_item = MASK_VIEW_ITEM(widget);
  return_value_if_fail(widget != NULL && mask_view_item != NULL, RET_BAD_PARAMS);

  (void)mask_view_item;

  return RET_OK;
}

const char* s_mask_view_item_properties[] = {
  MASK_VIEW_PROP_MASK_VIEW,
  NULL
};

TK_DECL_VTABLE(mask_view_item) = {.size = sizeof(mask_view_item_t),
                          .type = WIDGET_TYPE_MASK_VIEW_ITEM,
                          .clone_properties = s_mask_view_item_properties,
                          .persistent_properties = s_mask_view_item_properties,
                          .parent = TK_PARENT_VTABLE(widget),
                          .create = mask_view_item_create,
                          .on_paint_self = mask_view_item_on_paint_self,
                          .set_prop = mask_view_item_set_prop,
                          .get_prop = mask_view_item_get_prop,
                          .on_event = mask_view_item_on_event,
                          .on_destroy = mask_view_item_on_destroy};

widget_t* mask_view_item_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, TK_REF_VTABLE(mask_view_item), x, y, w, h);
  mask_view_item_t* mask_view_item = MASK_VIEW_ITEM(widget);
  return_value_if_fail(mask_view_item != NULL, NULL);

  mask_view_item->mask_view = FALSE;
  return widget;
}

widget_t* mask_view_item_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, mask_view_item), NULL);

  return widget;
}
