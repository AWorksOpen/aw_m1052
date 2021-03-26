#include "awtk.h"
#include "table_view_register.h"
#include "table_row/table_row.h"
#include "table_client/table_client.h"

static ret_t on_quit(void* ctx, event_t* e) {
  tk_quit();

  return RET_OK;
}

static table_row_t* table_row_of(widget_t* child) {
  widget_t* iter = child;
  while(iter != NULL && !tk_str_eq(widget_get_type(iter), WIDGET_TYPE_TABLE_ROW)) {
    iter = iter->parent;
  }

  return TABLE_ROW(iter);
}

static ret_t on_value_changed(void* ctx, event_t* e) {
  table_row_t* row = table_row_of(WIDGET(e->target));

  log_debug("changed: %d value=%d\n", row->index, widget_get_value(WIDGET(e->target)));

  return RET_OK;
}

static ret_t on_color_changed(void* ctx, event_t* e) {
  table_row_t* row = table_row_of(WIDGET(e->target));

  log_debug("changed: %d value=%s\n", row->index, combo_box_get_text(WIDGET(e->target)));

  return RET_OK;
}

static ret_t on_remove_clicked(void* ctx, event_t* e) {
  table_row_t* row = table_row_of(WIDGET(e->target));

  log_debug("clicked: %d \n", row->index);

  return RET_OK;
}

static ret_t on_rows_clicked(void* ctx, event_t* e) {
  widget_t* target = WIDGET(e->target);
  widget_t* client = WIDGET(ctx);
  int rows = tk_atoi(target->name);

  table_client_set_rows(client, rows);

  return RET_OK;
}

static ret_t on_create_row(void* ctx, uint32_t index, widget_t* row) {
  widget_child_on(row, "value", EVT_VALUE_CHANGED, on_value_changed, NULL);
  widget_child_on(row, "color", EVT_VALUE_CHANGED, on_color_changed, NULL);
  widget_child_on(row, "remove", EVT_CLICK, on_remove_clicked, NULL);

  return RET_OK;
}

static ret_t on_load_data(void* ctx, uint32_t index, widget_t* row) {
  char name[32];
  tk_snprintf(name, sizeof(name), "name:%u", index);

  widget_set_text_utf8(widget_lookup(row, "name", TRUE), name);
  widget_set_value(widget_lookup(row, "value", TRUE),index%100);
  widget_set_value(widget_lookup(row, "color", TRUE), 0);

  return RET_OK;
}

/**
 * 初始化
 */
ret_t application_init(void) {
  table_view_register();

  widget_t* win = window_open("main");
  widget_t* client = widget_lookup(win, "table_client", TRUE);

  widget_child_on(win, "quit", EVT_CLICK, on_quit, NULL); 
  widget_child_on(win, "0", EVT_CLICK, on_rows_clicked, client); 
  widget_child_on(win, "10", EVT_CLICK, on_rows_clicked, client); 
  widget_child_on(win, "100", EVT_CLICK, on_rows_clicked, client); 
  widget_child_on(win, "10000000", EVT_CLICK, on_rows_clicked, client); 
  widget_child_on(win, "50000000", EVT_CLICK, on_rows_clicked, client); 

  table_client_set_on_load_data(client, on_load_data, client);
  table_client_set_on_create_row(client, on_create_row, client);

  return RET_OK;
}

/**
 * 退出
 */
ret_t application_exit(void) {
  log_debug("application_exit\n");
  return RET_OK;
}
