#include "awtk.h"
#include "../src/date_picker_register.h"
#include "../src/date_picker/date_edit.h"

static ret_t on_close(void* ctx, event_t* e) {
  tk_quit();

  return RET_OK;
}

static ret_t on_lang_changed(void* ctx, event_t* e) {
  widget_t* combo_box = WIDGET(e->target);
  char country[3];
  char language[3];
  const char* str = combo_box_get_text(combo_box);

  tk_strncpy(language, str, 2);
  tk_strncpy(country, str + 3, 2);
  locale_info_change(locale_info(), language, country);

  return RET_OK;
}

static ret_t on_date_changed(void* ctx, event_t* e) {
  value_change_event_t* evt = (value_change_event_cast(e));

  log_debug("%s => %s\n", value_str(&(evt->old_value)), value_str(&(evt->new_value)));

  return RET_OK;
}

/**
 * 初始化
 */
ret_t application_init(void) {
  date_picker_register();

  widget_t* win = window_open("main");
  widget_t* lang = widget_lookup(win, "lang", TRUE);

  /*test set default lange to en_US*/
  combo_box_set_value(lang, 0);
  locale_info_change(locale_info(), "en", "US");
  
  /*test set default lange to zh_CN*/
  //combo_box_set_value(lang, 1);
  //locale_info_change(locale_info(), "zh", "CN");

  widget_child_on(win, "close", EVT_CLICK, on_close, NULL); 
  widget_child_on(win, "lang", EVT_VALUE_CHANGED, on_lang_changed, NULL); 
  widget_child_on(win, "d1", EVT_VALUE_CHANGED, on_date_changed, NULL); 
  widget_child_on(win, "d2", EVT_VALUE_CHANGED, on_date_changed, NULL); 
  widget_child_on(win, "d3", EVT_VALUE_CHANGED, on_date_changed, NULL); 
  widget_child_on(win, "d4", EVT_VALUE_CHANGED, on_date_changed, NULL); 

  return RET_OK;
}

/**
 * 退出
 */
ret_t application_exit(void) {
  log_debug("application_exit\n");
  return RET_OK;
}
