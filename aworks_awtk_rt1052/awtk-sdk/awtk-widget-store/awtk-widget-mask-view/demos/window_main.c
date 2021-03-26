#include "awtk.h"
#include "../src/mask_view_register.h"

static bool_t add_dir = TRUE;
static float_t number = 0.0f;

static ret_t on_paint_vg(void* ctx, event_t* e) {
  value_t v_value;
  value_t v_max_value;
  value_t v_min_value;

  float_t w = 0.0f;
  float_t half_w = 0.0f;
  float_t end_h = 0.0f;
  float_t start_h = 0.0f;
  float_t percent = 0.0f;

  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  widget_t* image_value = WIDGET(ctx);
  vgcanvas_t* vg = canvas_get_vgcanvas(c);

  return_value_if_fail(widget_get_prop(image_value, WIDGET_PROP_VALUE, &v_value) == RET_OK, RET_FAIL);
  return_value_if_fail(widget_get_prop(image_value, WIDGET_PROP_MAX, &v_max_value) == RET_OK, RET_FAIL);
  return_value_if_fail(widget_get_prop(image_value, WIDGET_PROP_MIN, &v_min_value) == RET_OK, RET_FAIL);

  w = image_value->w;
  half_w = image_value->w / 2;
  percent = value_float(&v_value) / (value_float(&v_max_value) - value_float(&v_min_value));
  end_h = percent * image_value->h;
  start_h = (1.0f - percent) * image_value->h;

  vgcanvas_save(vg);

  vgcanvas_set_fill_color(vg, color_init(0, 0, 0xff, 0xff));

  vgcanvas_translate(vg, 0, start_h);
  vgcanvas_move_to(vg, 0, 0);
  vgcanvas_bezier_to(vg, half_w, -number, half_w, number, w, 0);
  vgcanvas_line_to(vg, w, end_h);
  vgcanvas_line_to(vg, 0, end_h);
  vgcanvas_line_to(vg, 0, 0);
  vgcanvas_close_path(vg);

  vgcanvas_fill(vg);

  vgcanvas_restore(vg);

  if (add_dir) {
    number++;
  } else {
    number--;
  }

  if (tk_abs(number) >= half_w) {
    add_dir = !add_dir;
  }
  
  return RET_OK;
}

static inline ret_t on_timer(const timer_info_t* timer) {
  widget_t* widget = WIDGET(timer->ctx);

  widget_invalidate(widget, NULL);

  return RET_REPEAT;
}

/**
 * 初始化
 */
ret_t application_init(void) {
  mask_view_register();

  widget_t* win = window_open("main");

  widget_t* canvas = widget_lookup_by_type(win, "canvas", TRUE);
  widget_t* image_value = widget_lookup_by_type(win, "image_value", TRUE);

  widget_on(canvas, EVT_PAINT, on_paint_vg, image_value);

  timer_add(on_timer, canvas, 16);


  return RET_OK;
}

/**
 * 退出
 */
ret_t application_exit(void) {
  log_debug("application_exit\n");
  return RET_OK;
}
