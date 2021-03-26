/**
 * File:   mask_view.c
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
#include "mask_view.h"
#include "base/layout.h"
#include "mask_view_item.h"
#include "base/window_manager.h"

static ret_t mask_view_init(widget_t* widget, bool_t get_children_now);

ret_t mask_view_fg_interval(widget_t* widget) {
  mask_view_t* mask_view = MASK_VIEW(widget);
  return_value_if_fail(mask_view != NULL, RET_BAD_PARAMS);

  mask_view->last_fg_time = 0;

  return RET_OK;
}

ret_t mask_view_bg_interval(widget_t* widget) {
  mask_view_t* mask_view = MASK_VIEW(widget);
  return_value_if_fail(mask_view != NULL, RET_BAD_PARAMS);

  mask_view->last_bg_time = 0;

  return RET_OK;
}

ret_t mask_view_set_fg_interval_time(widget_t* widget, int32_t fg_interval_time) {
  mask_view_t* mask_view = MASK_VIEW(widget);
  return_value_if_fail(mask_view != NULL, RET_BAD_PARAMS);

  mask_view->fg_interval_time = fg_interval_time;

  return RET_OK;
}

ret_t mask_view_set_bg_interval_time(widget_t* widget, int32_t bg_interval_time) {
  mask_view_t* mask_view = MASK_VIEW(widget);
  return_value_if_fail(mask_view != NULL, RET_BAD_PARAMS);

  mask_view->bg_interval_time = bg_interval_time;

  return RET_OK;
}

ret_t mask_view_update_children_list(widget_t* widget) {
  mask_view_t* mask_view = MASK_VIEW(widget);
  return_value_if_fail(mask_view != NULL, RET_BAD_PARAMS);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)

  if (tk_str_eq(iter->vt->type, WIDGET_TYPE_MASK_VIEW_ITEM)) {
    if (mask_view_item_get_mask_view(iter)) {
      darray_push(&(mask_view->fg_mask_views), iter);
    } else {
      darray_push(&(mask_view->bg_rect_views), iter);
    }
  }

  WIDGET_FOR_EACH_CHILD_END();

  return RET_OK;
}

static ret_t mask_view_get_prop(widget_t* widget, const char* name, value_t* v) {
  mask_view_t* mask_view = MASK_VIEW(widget);
  return_value_if_fail(mask_view != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(MASK_VIEW_PROP_FG_INTERVAL_TIME, name)) {
     value_set_int32(v, mask_view->fg_interval_time);
     return RET_OK;
  } else if (tk_str_eq(MASK_VIEW_PROP_BG_INTERVAL_TIME, name)) {
     value_set_int32(v, mask_view->bg_interval_time);
     return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t mask_view_set_prop(widget_t* widget, const char* name, const value_t* v) {
  mask_view_t* mask_view = MASK_VIEW(widget);
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(MASK_VIEW_PROP_FG_INTERVAL_TIME, name)) {
     mask_view_set_fg_interval_time(widget, value_int32(v));
     return RET_OK;
  } else if (tk_str_eq(MASK_VIEW_PROP_BG_INTERVAL_TIME, name)) {
     mask_view_set_bg_interval_time(widget, value_int32(v));
     return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t mask_view_on_destroy(widget_t* widget) {
  mask_view_t* mask_view = MASK_VIEW(widget);
  return_value_if_fail(widget != NULL && mask_view != NULL, RET_BAD_PARAMS);

  if (mask_view->bg_alpha_data != NULL) {
    TKMEM_FREE(mask_view->bg_alpha_data);
    mask_view->bg_alpha_data = NULL;
  }

  if (mask_view->bg_canvas != NULL)  {
    canvas_offline_destroy(mask_view->bg_canvas);
    mask_view->bg_canvas = NULL;
  }

  if (mask_view->fg_mask_canvas != NULL)  {
    canvas_offline_destroy(mask_view->fg_mask_canvas);
    mask_view->fg_mask_canvas = NULL;
  }
  
  bitmap_destroy(&(mask_view->online_bitmap));

  darray_deinit(&(mask_view->bg_rect_views));
  darray_deinit(&(mask_view->fg_mask_views));

  return RET_OK;
}

static ret_t mask_view_on_paint_self(widget_t* widget, canvas_t* c) {
  (void)c;
  (void)widget;

  return RET_OK;
}

static ret_t mask_view_update_bg_data_and_interval(mask_view_t* mask_view) {

  uint8_t offset_bit = 0;
  uint32_t bg_bitmap_bpp = 0;
  uint32_t mask_bitmap_bpp = 0;
  bitmap_t* bg_bitmap = canvas_offline_get_bitmap(mask_view->bg_canvas);
  bitmap_t* mask_bitmap = canvas_offline_get_bitmap(mask_view->fg_mask_canvas);

  uint8_t* bg_alpha_buff = mask_view->bg_alpha_data;
  uint8_t* bg_buff = bitmap_lock_buffer_for_write(bg_bitmap);
  uint8_t* mask_buff = bitmap_lock_buffer_for_write(mask_bitmap);
  uint8_t* bg_end_buff = bg_buff + bg_bitmap->h * bg_bitmap->line_length;

  bg_bitmap_bpp = bitmap_get_bpp(bg_bitmap);
  mask_bitmap_bpp = bitmap_get_bpp(mask_bitmap);

  offset_bit = mask_bitmap_bpp == 2 ? 0xf8 : 0xff;

  if (bg_bitmap->format == BITMAP_FMT_RGBA8888 || bg_bitmap->format == BITMAP_FMT_BGRA8888) {
    while (bg_buff != bg_end_buff) {
      bg_alpha_buff[0] = bg_buff[3];

      bg_buff[3] = (bg_alpha_buff[0] * (mask_buff[1] & offset_bit)) >> 8;

      bg_alpha_buff++;
      bg_buff += bg_bitmap_bpp;
      mask_buff += mask_bitmap_bpp;
    }
  } else if (bg_bitmap->format == BITMAP_FMT_ARGB8888 || bg_bitmap->format == BITMAP_FMT_ABGR8888) {
    while (bg_buff != bg_end_buff) {
      bg_alpha_buff[0] = bg_buff[0];

      bg_buff[0] = (bg_alpha_buff[0] * (mask_buff[1] & offset_bit)) >> 8;

      bg_alpha_buff++;
      bg_buff += bg_bitmap_bpp;
      mask_buff += mask_bitmap_bpp;
    }
  }

  bitmap_unlock_buffer(bg_bitmap);
  bitmap_unlock_buffer(mask_bitmap);

  return RET_OK;
}

static ret_t mask_view_update_fg_mask_and_interval(mask_view_t* mask_view) {

  uint8_t offset_bit = 0;
  uint32_t bg_bitmap_bpp = 0;
  uint32_t mask_bitmap_bpp = 0;
  bitmap_t* bg_bitmap = canvas_offline_get_bitmap(mask_view->bg_canvas);
  bitmap_t* mask_bitmap = canvas_offline_get_bitmap(mask_view->fg_mask_canvas);

  uint8_t* bg_alpha_buff = mask_view->bg_alpha_data;
  uint8_t* bg_buff = bitmap_lock_buffer_for_write(bg_bitmap);
  uint8_t* mask_buff = bitmap_lock_buffer_for_write(mask_bitmap);
  uint8_t* bg_end_buff = bg_buff + bg_bitmap->h * bg_bitmap->line_length;

  bg_bitmap_bpp = bitmap_get_bpp(bg_bitmap);
  mask_bitmap_bpp = bitmap_get_bpp(mask_bitmap);

  offset_bit = mask_bitmap_bpp == 2 ? 0xf8 : 0xff;

  if (bg_bitmap->format == BITMAP_FMT_RGBA8888 || bg_bitmap->format == BITMAP_FMT_BGRA8888) {
    while (bg_buff != bg_end_buff) {

      bg_buff[3] = (bg_alpha_buff[0] * (mask_buff[1] & offset_bit)) >> 8;

      bg_alpha_buff++;
      bg_buff += bg_bitmap_bpp;
      mask_buff += mask_bitmap_bpp;
    }
  } else if (bg_bitmap->format == BITMAP_FMT_ARGB8888 || bg_bitmap->format == BITMAP_FMT_ABGR8888) {
    while (bg_buff != bg_end_buff) {

      bg_buff[0] = (bg_alpha_buff[0] * (mask_buff[1] & offset_bit)) >> 8;

      bg_alpha_buff++;
      bg_buff += bg_bitmap_bpp;
      mask_buff += mask_bitmap_bpp;
    }
  }

  bitmap_unlock_buffer(bg_bitmap);
  bitmap_unlock_buffer(mask_bitmap);

  return RET_OK;
}

#define CHILD_PAINT_WITH_CANVAS_OFFLINE(rect_views, c) {                \
  uint32_t i = 0;                                                       \
  uint32_t size = rect_views.size;                                      \
  canvas_offline_begin_draw(c);                                         \
  for (i = 0; i < size; i++) {                                          \
    void* item = darray_get(&rect_views, i);                            \
    if (item != NULL) {                                                 \
      widget_t* child = WIDGET(item);                                   \
      widget_paint(child, c);                                           \
    }                                                                   \
  }                                                                     \
  canvas_offline_end_draw(c);                                           \
}                                                                       \

static ret_t mask_view_check_or_clear_drity_rect(canvas_t* c, rect_t* last_drity_rect, darray_t* rect_views) {
  uint32_t i = 0;
  xy_t l1, r1, t1 ,b1;
  xy_t l2, r2, t2 ,b2;
  uint32_t size = rect_views->size;
  rect_t curr_drity_rect = {0, 0, 0, 0};

  for (i = 0; i < size; i++) {
    void* item = darray_get(rect_views, i);
    if (item != NULL) {
      widget_t* child = WIDGET(item);
      rect_t child_rect = {child->x, child->y, child->w, child->h};
      rect_merge(&curr_drity_rect, &child_rect);
    }
  }

  l1 = last_drity_rect->x;
  t1 = last_drity_rect->y;
  r1 = last_drity_rect->x + last_drity_rect->w;
  b1 = last_drity_rect->y + last_drity_rect->h;

  l2 = curr_drity_rect.x;
  t2 = curr_drity_rect.y;
  r2 = curr_drity_rect.x + curr_drity_rect.w;
  b2 = curr_drity_rect.y + curr_drity_rect.h;
  
  if ((l2 < l1 || t2 < t1 || r2 > r1 || b2 > b1) && last_drity_rect->w != 0 && last_drity_rect->h != 0) {
    canvas_offline_begin_draw(c);
    canvas_set_fill_color(c, color_init(0x0, 0x0, 0x0, 0xff));
    canvas_fill_rect(c, last_drity_rect->x, last_drity_rect->y, last_drity_rect->w, last_drity_rect->h);
    canvas_offline_end_draw(c);
  }

  last_drity_rect->x = curr_drity_rect.x;
  last_drity_rect->y = curr_drity_rect.y;
  last_drity_rect->w = curr_drity_rect.w;
  last_drity_rect->h = curr_drity_rect.h;

  return RET_OK;
}

static ret_t mask_view_on_paint_children(widget_t* widget, canvas_t* c) {
  bool_t is_fg_update = FALSE;
  bool_t is_bg_update = FALSE;
  uint64_t curr_time = timer_manager()->get_time();
  mask_view_t* mask_view = MASK_VIEW(widget);
  rect_t rect = rect_init(0, 0, mask_view->online_bitmap.w, mask_view->online_bitmap.h);
  return_value_if_fail(mask_view != NULL && widget->w > 0 && widget->h > 0, RET_BAD_PARAMS);
  
  if (mask_view->bg_interval_time == 0 || mask_view->last_bg_time == 0 ||
     (curr_time - mask_view->last_bg_time) > mask_view->bg_interval_time) {
      mask_view_check_or_clear_drity_rect(mask_view->bg_canvas, &(mask_view->last_fg_mask_drity_rect), &(mask_view->bg_rect_views));

      CHILD_PAINT_WITH_CANVAS_OFFLINE(mask_view->bg_rect_views, mask_view->bg_canvas);
      canvas_offline_flush_bitmap(mask_view->bg_canvas);

      is_bg_update = TRUE;
      mask_view->last_bg_time = curr_time;
  }

  if (mask_view->fg_interval_time == 0 || mask_view->last_fg_time == 0 ||
     (curr_time - mask_view->last_fg_time) > mask_view->fg_interval_time) {
      CHILD_PAINT_WITH_CANVAS_OFFLINE(mask_view->fg_mask_views, mask_view->fg_mask_canvas);
      canvas_offline_flush_bitmap(mask_view->fg_mask_canvas);

      is_fg_update = TRUE;
      mask_view->last_fg_time = curr_time;
  }

  if (is_bg_update) {
    mask_view_update_bg_data_and_interval(mask_view);
  } else if (is_fg_update) {
    mask_view_update_fg_mask_and_interval(mask_view);
  }

  mask_view->online_bitmap.flags |= BITMAP_FLAG_CHANGED;
  return canvas_draw_image_ex(c, &(mask_view->online_bitmap), IMAGE_DRAW_DEFAULT, &rect);
}

static ret_t mask_view_on_event(widget_t* widget, event_t* e) {
  (void)e;
  (void)widget;

  return RET_OK;
}

static ret_t mask_view_widget_on_layout_children(widget_t* widget) {
  ret_t ret = RET_OK;
  ret = widget_layout_children_default(widget);

  if (ret == RET_OK) {
    mask_view_on_destroy(widget);
    return mask_view_init(widget, TRUE);
  } else {
    return ret;
  }
}

static ret_t mask_view_get_children(widget_t* widget) {
  mask_view_t* mask_view = MASK_VIEW(widget);
  if (widget != NULL && mask_view != NULL && widget->children != NULL) {
    darray_deinit(&(mask_view->bg_rect_views));
    darray_deinit(&(mask_view->fg_mask_views));

    darray_init(&mask_view->bg_rect_views, widget->children->size, NULL, NULL);
    darray_init(&mask_view->fg_mask_views, widget->children->size, NULL, NULL);

    mask_view_update_children_list(widget);
  }
  return RET_OK;
}

static ret_t mask_view_get_children_idle_func(const idle_info_t* idle) {
  widget_t* widget = WIDGET(idle->ctx);
  return mask_view_get_children(widget);
}

static ret_t mask_view_online_bitmap_init(mask_view_t* mask_view) {
  bitmap_t* bitmap = canvas_offline_get_bitmap(mask_view->bg_canvas);
  uint8_t* buff = bitmap_lock_buffer_for_write(bitmap);

  mask_view->online_bitmap.w = bitmap->w;
  mask_view->online_bitmap.h = bitmap->h;
  mask_view->online_bitmap.flags = bitmap->flags;
  mask_view->online_bitmap.format = bitmap->format;
  mask_view->online_bitmap.image_manager = image_manager();
  bitmap_set_line_length(&(mask_view->online_bitmap), bitmap->line_length);

  mask_view->online_bitmap.buffer = GRAPHIC_BUFFER_CREATE_WITH_DATA(buff, bitmap->w, bitmap->h,
                                                  (bitmap_format_t)(bitmap->format));

  mask_view->online_bitmap.should_free_data = mask_view->online_bitmap.buffer != NULL;
  bitmap_unlock_buffer(bitmap);

  mask_view->bg_alpha_data = TKMEM_CALLOC(sizeof(uint8_t), bitmap->w * bitmap->h);

  ENSURE(mask_view->bg_alpha_data != NULL);

  return RET_OK;
}

static ret_t mask_view_init(widget_t* widget, bool_t get_children_now) {

  mask_view_t* mask_view = MASK_VIEW(widget);
  bitmap_format_t fg_format = BITMAP_FMT_RGBA8888;
  bitmap_format_t bg_format = BITMAP_FMT_RGBA8888;

#ifndef WITH_NANOVG_GPU
  canvas_t* c = NULL;
  native_window_t* native_window = NULL;
  native_window = (native_window_t*)widget_get_prop_pointer(window_manager(), WIDGET_PROP_NATIVE_WINDOW);
  c = native_window_get_canvas(native_window);
  fg_format = lcd_get_desired_bitmap_format(c->lcd);
  if (bitmap_get_bpp_of_format(fg_format) == 4) {
    bg_format = fg_format;
  }
#endif 
  return_value_if_fail(mask_view != NULL && widget->w > 0 && widget->h > 0, RET_BAD_PARAMS);

  mask_view->bg_interval_time = 0;
  mask_view->fg_interval_time = 0;

  mask_view->bg_canvas = canvas_offline_create(widget->w, widget->h, bg_format);
  mask_view->fg_mask_canvas = canvas_offline_create(widget->w, widget->h, fg_format);

  memset(&(mask_view->last_fg_mask_drity_rect), 0x0, sizeof(rect_t));

  mask_view_online_bitmap_init(mask_view);

  if (get_children_now) {
    return mask_view_get_children(widget);
  } else {
    return idle_add(mask_view_get_children_idle_func, widget) != TK_INVALID_ID ? RET_OK : RET_FAIL;
  }
}

const char* s_mask_view_properties[] = {
  MASK_VIEW_PROP_FG_INTERVAL_TIME,
  MASK_VIEW_PROP_BG_INTERVAL_TIME,
  NULL
};

TK_DECL_VTABLE(mask_view) = {.size = sizeof(mask_view_t),
                          .type = WIDGET_TYPE_MASK_VIEW,
                          .clone_properties = s_mask_view_properties,
                          .persistent_properties = s_mask_view_properties,
                          .parent = TK_PARENT_VTABLE(widget),
                          .create = mask_view_create,
                          .on_paint_self = mask_view_on_paint_self,
                          .on_paint_children = mask_view_on_paint_children,
                          .on_layout_children = mask_view_widget_on_layout_children,
                          .set_prop = mask_view_set_prop,
                          .get_prop = mask_view_get_prop,
                          .on_event = mask_view_on_event,
                          .on_destroy = mask_view_on_destroy};

widget_t* mask_view_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {

  bitmap_format_t fg_format = BITMAP_FMT_RGBA8888;
  bitmap_format_t bg_format = BITMAP_FMT_RGBA8888;
  widget_t* widget = widget_create(parent, TK_REF_VTABLE(mask_view), x, y, w, h);
  mask_view_t* mask_view = MASK_VIEW(widget);
  return_value_if_fail(mask_view != NULL, NULL);

  mask_view_init(widget, FALSE);

  return widget;
}

widget_t* mask_view_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, mask_view), NULL);

  return widget;
}
