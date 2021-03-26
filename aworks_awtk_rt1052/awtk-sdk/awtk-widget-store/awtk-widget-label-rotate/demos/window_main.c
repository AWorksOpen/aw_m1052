#include "awtk.h"

/**
 * 初始化
 */
ret_t application_init(void) {
  label_rotate_register();

  widget_t* win = window_open("main");

  return RET_OK;
}

/**
 * 退出
 */
ret_t application_exit(void) {
  log_debug("application_exit\n");
  return RET_OK;
}
