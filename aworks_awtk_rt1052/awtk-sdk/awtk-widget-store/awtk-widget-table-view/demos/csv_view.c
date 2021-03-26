#include "awtk.h"

#include "csv_file.h"
#include "table_view_register.h"
#include "table_row/table_row.h"
#include "table_client/table_client.h"

#define COL_NAME 0
#define COL_CHINESE 1
#define COL_MATH 2
#define COL_ENGLISH 3
#define COL_MEMO 4

static ret_t csv_view_on_quit(void* ctx, event_t* e) {
  tk_quit();

  return RET_OK;
}

static ret_t csv_view_on_save(void* ctx, event_t* e) {
  csv_file_t* csv = (csv_file_t*)ctx;
  csv_file_save(csv, csv->filename);

  return RET_OK;
}

static ret_t csv_view_on_reload(void* ctx, event_t* e) {
  csv_file_t* csv = (csv_file_t*)ctx;
  widget_t* win = widget_get_window(WIDGET(e->target));
  widget_t* client = widget_lookup_by_type(win, WIDGET_TYPE_TABLE_CLIENT, TRUE);
  assert(client != NULL);

  csv_file_reload(csv);
  table_client_reload(client);

  return RET_OK;
}

static table_row_t* table_row_of(widget_t* child) {
  widget_t* iter = child;
  while(iter != NULL && !tk_str_eq(widget_get_type(iter), WIDGET_TYPE_TABLE_ROW)) {
    iter = iter->parent;
  }

  return TABLE_ROW(iter);
}

static ret_t csv_view_on_load_data(void* ctx, uint32_t index, widget_t* row) {
  csv_file_t* csv = (csv_file_t*)ctx;
  char str[32];
  tk_snprintf(str, sizeof(str), "%u", index);

  widget_set_text_utf8(widget_lookup(row, "index", TRUE), str);
  widget_set_text_utf8(widget_lookup(row, "name", TRUE), csv_file_get(csv, index, COL_NAME));
  widget_set_text_utf8(widget_lookup(row, "chinese", TRUE), csv_file_get(csv, index, COL_CHINESE));
  widget_set_text_utf8(widget_lookup(row, "math", TRUE), csv_file_get(csv, index, COL_MATH));
  widget_set_text_utf8(widget_lookup(row, "english", TRUE), csv_file_get(csv, index, COL_ENGLISH));
  widget_set_text_utf8(widget_lookup(row, "memo", TRUE), csv_file_get(csv, index, COL_MEMO));

  return RET_OK;
}

static ret_t csv_view_on_memo_changed(void* ctx, event_t* e) {
  str_t str;
  csv_file_t* csv = (csv_file_t*)ctx;
  widget_t* target = WIDGET(e->target);
  table_row_t* row = table_row_of(WIDGET(e->target));

  str_init(&str, 0);
  str_from_wstr(&str, target->text.str);

  csv_file_set(csv, row->index, COL_MEMO, str.str);
  str_reset(&str);

  return RET_OK;
}

static ret_t csv_view_on_create_row(void* ctx, uint32_t index, widget_t* row) {
  widget_child_on(row, "memo", EVT_VALUE_CHANGED, csv_view_on_memo_changed, ctx);

  return RET_OK;
}

/**
 * 初始化
 */
ret_t application_init(void) {
  table_view_register();
  widget_t* win = window_open("csv_view");
  csv_file_t* csv = csv_file_create("data/scores.csv", ',');  
  widget_t* client = widget_lookup(win, "table_client", TRUE);

  widget_child_on(win, "quit", EVT_CLICK, csv_view_on_quit, NULL); 
  widget_child_on(win, "save", EVT_CLICK, csv_view_on_save, csv); 
  widget_child_on(win, "reload", EVT_CLICK, csv_view_on_reload, csv); 

  table_client_set_rows(client, csv_file_get_rows(csv));
  table_client_set_on_load_data(client, csv_view_on_load_data, csv);
  table_client_set_on_create_row(client, csv_view_on_create_row, csv);

  return RET_OK;
}

/**
 * 退出
 */
ret_t application_exit(void) {
  log_debug("application_exit\n");
  return RET_OK;
}
