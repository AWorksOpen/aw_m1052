/**
 * File:   button_ex.h
 * Author: AWTK Develop Team
 * Brief:  button_ex
 *
 * Copyright (c) 2018 - 2019  Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2018-07-05 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "base/timer.h"
#include "button_ex.h"

static ret_t button_ex_on_paint_self(widget_t* widget, canvas_t* c) {
  return widget_paint_helper(widget, c, NULL, NULL);
}

static ret_t button_ex_on_timer(const timer_info_t* info) {
  widget_t* widget = WIDGET(info->ctx);
  button_ex_t* button_ex = BUTTON_EX(widget);
  return_value_if_fail(widget != NULL && button_ex != NULL, RET_BAD_PARAMS);

  if (!button_ex->dragged) {
    widget_set_state(widget, WIDGET_STATE_PRESSED);
  }
  button_ex->timer_id = TK_INVALID_ID;

  return RET_REMOVE;
}

static ret_t button_ex_remove_timer(widget_t* widget) {
  button_ex_t* button_ex = BUTTON_EX(widget);
  return_value_if_fail(button_ex != NULL, RET_BAD_PARAMS);

  if (button_ex->timer_id != TK_INVALID_ID) {
    timer_remove(button_ex->timer_id);
    button_ex->timer_id = TK_INVALID_ID;
  }

  return RET_OK;
}

static ret_t button_ex_on_event(widget_t* widget, event_t* e) {
  uint16_t type = e->type;
  button_ex_t* button_ex = BUTTON_EX(widget);
  char anim_params[100];
  return_value_if_fail(button_ex != NULL, RET_BAD_PARAMS);

  switch (type) {
    case EVT_POINTER_DOWN: {
      pointer_event_t* evt = (pointer_event_t*)e;
      button_ex->down.x = evt->x;
      button_ex->down.y = evt->y;
      button_ex->timer_id = timer_add(button_ex_on_timer, widget, 30);
      widget_invalidate_force(widget, NULL);
      break;
    }
    case EVT_POINTER_DOWN_ABORT: {
      button_ex_remove_timer(widget);
      widget_invalidate_force(widget, NULL);
      widget_set_state(widget, WIDGET_STATE_NORMAL);
      break;
    }
    case EVT_POINTER_UP: {
      button_ex_remove_timer(widget);
      widget_invalidate_force(widget, NULL);
      widget_set_state(widget, WIDGET_STATE_NORMAL);

      if (!button_ex->dragged) {
        pointer_event_t evt = *(pointer_event_t*)e;

        evt.e = event_init(EVT_CLICK, widget);
        evt.e.size = sizeof(evt);
        widget_dispatch(widget, (event_t*)&evt);

        tk_snprintf(anim_params, sizeof(anim_params), "h(to=%d,duration=500,easing=cubic_out)",
                    button_ex->new_h);
        widget_create_animator(widget, anim_params);
      }
      button_ex->dragged = FALSE;
      break;
    }
    case EVT_POINTER_MOVE: {
      pointer_event_t* evt = (pointer_event_t*)e;
      uint32_t dy = tk_abs(evt->y - button_ex->down.y);

      if (evt->pressed && dy > TK_DRAG_THRESHOLD) {
        button_ex->dragged = TRUE;
        button_ex_remove_timer(widget);
        widget_set_state(widget, WIDGET_STATE_NORMAL);
      }

      break;
    }
    case EVT_POINTER_LEAVE:
      button_ex_remove_timer(widget);
      widget_set_state(widget, WIDGET_STATE_NORMAL);
      break;
    case EVT_POINTER_ENTER:
      if (button_ex->dragged) {
        widget_set_state(widget, WIDGET_STATE_NORMAL);
      } else {
        widget_set_state(widget, WIDGET_STATE_OVER);
      }
      break;
    case EVT_BLUR: {
      tk_snprintf(anim_params, sizeof(anim_params), "h(to=%d,duration=500,easing=cubic_out)",
                  button_ex->old_h);
      widget_create_animator(widget, anim_params);
    }
    default:
      break;
  }

  return RET_OK;
}

ret_t button_ex_set_new_h(widget_t* widget, int32_t new_h) {
  button_ex_t* button_ex = BUTTON_EX(widget);
  return_value_if_fail(button_ex != NULL, RET_BAD_PARAMS);

  button_ex->new_h = new_h;

  return RET_OK;
}

static ret_t button_ex_get_prop(widget_t* widget, const char* name, value_t* v) {
  button_ex_t* button_ex = BUTTON_EX(widget);
  return_value_if_fail(button_ex != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_NEW_H)) {
    value_set_int32(v, button_ex->new_h);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t button_ex_set_prop(widget_t* widget, const char* name, const value_t* v) {
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_NEW_H)) {
    return button_ex_set_new_h(widget, value_int32(v));
  }

  return RET_NOT_FOUND;
}

static ret_t button_ex_on_destroy(widget_t* widget) {
  button_ex_t* button_ex = BUTTON_EX(widget);
  return_value_if_fail(button_ex != NULL, RET_BAD_PARAMS);

  if (button_ex->timer_id != TK_INVALID_ID) {
    timer_remove(button_ex->timer_id);
    button_ex->timer_id = TK_INVALID_ID;
  }

  return RET_OK;
}

static ret_t button_ex_on_paint_children(widget_t* widget, canvas_t* c) {
  rect_t r_save;
  rect_t r = rect_init(c->ox, c->oy, widget->w, widget->h);

  button_ex_t* button_ex = BUTTON_EX(widget);
  canvas_get_clip_rect(c, &r_save);
  r = rect_intersect(&r, &r_save);
  canvas_set_clip_rect(c, &r);
  if (button_ex->on_paint_children) {
    button_ex->on_paint_children(widget, c);
  } else {
    widget_on_paint_children_default(widget, c);
  }
  canvas_set_clip_rect(c, &r_save);

  return RET_OK;
}

TK_DECL_VTABLE(button_ex) = {.size = sizeof(button_ex_t),
                             .type = WIDGET_TYPE_BUTTON_EX,
                             .parent = TK_PARENT_VTABLE(widget),
                             .create = button_ex_create,
                             .on_event = button_ex_on_event,
                             .set_prop = button_ex_set_prop,
                             .get_prop = button_ex_get_prop,
                             .on_paint_self = button_ex_on_paint_self,
                             .on_paint_children = button_ex_on_paint_children,
                             .on_destroy = button_ex_on_destroy};

widget_t* button_ex_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, TK_REF_VTABLE(button_ex), x, y, w, h);
  button_ex_t* button_ex = BUTTON_EX(widget);
  return_value_if_fail(button_ex != NULL, NULL);

  button_ex->timer_id = TK_INVALID_ID;
  button_ex->old_h = h;

  return widget;
}

widget_t* button_ex_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, button_ex), NULL);

  return widget;
}
