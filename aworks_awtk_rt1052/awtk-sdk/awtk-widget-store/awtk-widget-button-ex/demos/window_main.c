#include "awtk.h"

static ret_t on_changing(void* ctx, event_t* evt) {
  widget_t* slider = WIDGET(evt->target);
  widget_t* win = WIDGET(ctx);
  widget_t* temperature_num = widget_lookup(win, "temperature_num", TRUE);

  char text[32] = {0};
  value_t v;
  value_set_int32(&v, widget_get_value(slider));
  tk_snprintf(text, sizeof(text), "%d%", value_int32(&v));
  widget_set_text_utf8(temperature_num, text);

  return RET_OK;
}

/**
 * 初始化
 */
ret_t application_init(void) {
  button_ex_register();

  widget_t* win = window_open("main");
  if (win) {
    widget_t* slider = widget_lookup(win, "slider", TRUE);
    widget_on(slider, EVT_VALUE_CHANGING, on_changing, win);
  }

  return RET_OK;
}

/**
 * 退出
 */
ret_t application_exit(void) {
  log_debug("application_exit\n");
  return RET_OK;
}
