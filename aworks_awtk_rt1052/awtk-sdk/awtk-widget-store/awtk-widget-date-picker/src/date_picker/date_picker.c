/**
 * File:   date_picker.c
 * Author: AWTK Develop Team
 * Brief:  日期选择控件。
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
 * 2020-07-12 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "tkc/utils.h"
#include "tkc/date_time.h"
#include "date_picker.h"

#define DATE_PICKER_FORMAT "%d/%d/%d"
#define DATE_PICKER_STYLE_DAY "day"
#define DATE_PICKER_STYLE_DAY_CURRENT "day_current"

#define DATE_PICKER_CHILD_MON "mon"
#define DATE_PICKER_CHILD_TUE "tue"
#define DATE_PICKER_CHILD_WED "wed"
#define DATE_PICKER_CHILD_THU "thu"
#define DATE_PICKER_CHILD_FRI "fri"
#define DATE_PICKER_CHILD_SAT "sat"
#define DATE_PICKER_CHILD_SUN "sun"
#define DATE_PICKER_CHILD_TODAY "today"
#define DATE_PICKER_CHILD_DAYS "days"
#define DATE_PICKER_CHILD_PREV_YEAR "prev_year"
#define DATE_PICKER_CHILD_PREV_MONTH "prev_month"
#define DATE_PICKER_CHILD_NEXT_YEAR "next_year"
#define DATE_PICKER_CHILD_NEXT_MONTH "next_month"
#define DATE_PICKER_CHILD_YEAR_MONTH "year_month"

static ret_t date_picker_update_view(widget_t* widget);

ret_t date_picker_set_year(widget_t* widget, uint32_t year) {
  date_picker_t* date_picker = DATE_PICKER(widget);
  return_value_if_fail(date_picker != NULL, RET_BAD_PARAMS);

  date_picker->year = year;
  if (date_picker->inited) {
    date_picker_update_view(widget);
  }

  return RET_OK;
}

ret_t date_picker_set_month(widget_t* widget, uint32_t month) {
  date_picker_t* date_picker = DATE_PICKER(widget);
  return_value_if_fail(date_picker != NULL, RET_BAD_PARAMS);

  date_picker->month = month;
  if (date_picker->inited) {
    date_picker_update_view(widget);
  }

  return RET_OK;
}

ret_t date_picker_set_day(widget_t* widget, uint32_t day) {
  date_picker_t* date_picker = DATE_PICKER(widget);
  return_value_if_fail(date_picker != NULL, RET_BAD_PARAMS);

  date_picker->day = day;
  if (date_picker->inited) {
    date_picker_update_view(widget);
  }

  return RET_OK;
}

static ret_t date_picker_get_prop(widget_t* widget, const char* name, value_t* v) {
  date_picker_t* date_picker = DATE_PICKER(widget);
  return_value_if_fail(date_picker != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(DATE_PICKER_PROP_YEAR, name)) {
    value_set_uint32(v, date_picker->year);
    return RET_OK;
  } else if (tk_str_eq(DATE_PICKER_PROP_MONTH, name)) {
    value_set_uint32(v, date_picker->month);
    return RET_OK;
  } else if (tk_str_eq(DATE_PICKER_PROP_DAY, name)) {
    value_set_uint32(v, date_picker->day);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t date_picker_set_prop(widget_t* widget, const char* name, const value_t* v) {
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(DATE_PICKER_PROP_YEAR, name)) {
    date_picker_set_year(widget, value_uint32(v));
    return RET_OK;
  } else if (tk_str_eq(DATE_PICKER_PROP_MONTH, name)) {
    date_picker_set_month(widget, value_uint32(v));
    return RET_OK;
  } else if (tk_str_eq(DATE_PICKER_PROP_DAY, name)) {
    date_picker_set_day(widget, value_uint32(v));
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t date_picker_on_destroy(widget_t* widget) {
  date_picker_t* date_picker = DATE_PICKER(widget);
  return_value_if_fail(widget != NULL && date_picker != NULL, RET_BAD_PARAMS);

  return RET_OK;
}

static ret_t date_picker_on_paint_self(widget_t* widget, canvas_t* c) {
  date_picker_t* date_picker = DATE_PICKER(widget);

  (void)date_picker;

  return RET_OK;
}

static int32_t date_picker_get_days_of_prev_month(widget_t* widget) {
  date_picker_t* date_picker = DATE_PICKER(widget);
  int32_t year = date_picker->year;
  int32_t month = date_picker->month;

  if (month > 1) {
    month--;
  } else {
    month = 12;
    year--;
  }

  return date_time_get_days(year, month);
}

static ret_t date_picker_update_view(widget_t* widget) {
  int32_t i = 0;
  int32_t k = 0;
  int32_t nr = 0;
  widget_t* iter = NULL;
  char name[TK_NAME_LEN + 1];
  const char* tr_format = NULL;
  const char* format = "%d year %d month";
  date_picker_t* date_picker = DATE_PICKER(widget);
  widget_t* widget_days = widget_lookup(widget, DATE_PICKER_CHILD_DAYS, TRUE);
  widget_t* widget_year_month = widget_lookup(widget, DATE_PICKER_CHILD_YEAR_MONTH, TRUE);
  int32_t prev_days = date_picker_get_days_of_prev_month(widget);
  int32_t days = date_time_get_days(date_picker->year, date_picker->month);
  int32_t wday = date_time_get_wday(date_picker->year, date_picker->month, 1);
  return_value_if_fail(widget_days != NULL && widget_year_month != NULL, RET_BAD_PARAMS);

  tr_format = locale_info_tr(widget_get_locale_info(widget), format);
  if (tr_format != format) {
    tk_snprintf(name, TK_NAME_LEN, tr_format, date_picker->year, date_picker->month);
  } else {
    const char* month_name = date_time_get_month_name(date_picker->month);
    const char* tr_month_name = locale_info_tr(widget_get_locale_info(widget), month_name);
    if (tr_month_name != month_name) {
      tk_snprintf(name, TK_NAME_LEN, "%s %d", month_name, date_picker->year);
    } else {
      tk_snprintf(name, TK_NAME_LEN, "%d/%d", date_picker->year, date_picker->month);
    }
  }

  widget_set_text_utf8(widget_year_month, name);

  nr = widget_count_children(widget_days);
  for (i = 0; i < wday; i++) {
    int d = prev_days - wday + i + 1;
    iter = widget_get_child(widget_days, i);
    tk_snprintf(name, TK_NAME_LEN, "%d", d);
    widget_set_enable(iter, FALSE);
    widget_set_name(iter, name);
    widget_set_text_utf8(iter, name);
    widget_use_style(iter, DATE_PICKER_STYLE_DAY);
  }

  for (i = 0; i < days; i++) {
    iter = widget_get_child(widget_days, i + wday);
    tk_snprintf(name, TK_NAME_LEN, "%d", i + 1);
    widget_set_enable(iter, TRUE);
    widget_set_name(iter, name);
    widget_set_text_utf8(iter, name);
    if (date_picker->day == (i + 1)) {
      widget_use_style(iter, DATE_PICKER_STYLE_DAY_CURRENT);
    } else {
      widget_use_style(iter, DATE_PICKER_STYLE_DAY);
    }
  }

  k = wday + days;
  for (i = 0; i < (nr - k); i++) {
    iter = widget_get_child(widget_days, k + i);
    tk_snprintf(name, TK_NAME_LEN, "%d", i + 1);
    widget_set_enable(iter, FALSE);
    widget_set_name(iter, name);
    widget_set_text_utf8(iter, name);
    widget_use_style(iter, DATE_PICKER_STYLE_DAY);
  }

  widget_invalidate_force(widget, NULL);

  return RET_OK;
}

static ret_t date_picker_set_value(widget_t* widget, int year, int month, int day) {
  char new_text[64];
  char old_text[64];
  value_change_event_t evt;
  date_picker_t* date_picker = DATE_PICKER(widget);

  if(date_picker->year == year && date_picker->month == month && date_picker->day == day) {
    return RET_OK;
  }

  tk_snprintf(old_text, sizeof(old_text)-1, DATE_PICKER_FORMAT, 
      date_picker->year, date_picker->month, date_picker->day);
  tk_snprintf(new_text, sizeof(old_text)-1, DATE_PICKER_FORMAT, 
      year, month, day);
  value_change_event_init(&evt, EVT_VALUE_WILL_CHANGE, widget);
  value_set_str(&(evt.old_value), old_text);
  value_set_str(&(evt.new_value), new_text);

  if(widget_dispatch(widget, (event_t*)&evt) != RET_STOP) {
    date_picker->year = year;
    date_picker->month = month;
    date_picker->day = day;
  
    evt.e.type = EVT_VALUE_CHANGED;
    widget_dispatch(widget, (event_t*)&evt);
    widget_invalidate(widget, NULL);
  }

  return RET_OK;
}

static ret_t date_picker_on_button_clicked(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  widget_t* button = WIDGET(e->target);
  const char* name = button->name;
  date_picker_t* date_picker = DATE_PICKER(ctx);
  int year = date_picker->year;
  int month = date_picker->month;
  int day = date_picker->day;
  int save_year = date_picker->year;
  int save_month = date_picker->month;
  int save_day = date_picker->day;

  if (tk_str_eq(name, DATE_PICKER_CHILD_TODAY)) {
    date_time_t dt;
    date_time_init(&dt);
    date_picker->year = dt.year;
    date_picker->month = dt.month;
    date_picker->day = dt.day;
  } else if (tk_str_eq(name, DATE_PICKER_CHILD_PREV_YEAR)) {
    date_picker->year--;
  } else if (tk_str_eq(name, DATE_PICKER_CHILD_PREV_MONTH)) {
    if (date_picker->month > 1) {
      date_picker->month--;
    } else {
      date_picker->month = 12;
      date_picker->year--;
    }
  } else if (tk_str_eq(name, DATE_PICKER_CHILD_NEXT_YEAR)) {
    date_picker->year++;
  } else if (tk_str_eq(name, DATE_PICKER_CHILD_NEXT_MONTH)) {
    if (date_picker->month < 12) {
      date_picker->month++;
    } else {
      date_picker->month = 1;
      date_picker->year++;
    }
  } else {
    int day = tk_atoi(name);
    if (day > 0 && day <= 31) {
      date_picker->day = day;
    }
  }

  year = date_picker->year;
  month = date_picker->month;
  day = date_picker->day;
  date_picker->year = save_year;
  date_picker->month = save_month;
  date_picker->day = save_day;

  date_picker_set_value(widget, year, month, day);
  date_picker_update_view(widget);

  return RET_OK;
}

static ret_t date_picker_on_visit_child(void* ctx, const void* data) {
  widget_t* child = WIDGET(data);

  if (tk_str_eq(widget_get_type(child), WIDGET_TYPE_BUTTON)) {
    widget_on(child, EVT_CLICK, date_picker_on_button_clicked, ctx);
  }

  return RET_OK;
}

static ret_t date_picker_init(widget_t* widget) {
  date_picker_t* date_picker = DATE_PICKER(widget);
  date_picker_update_view(widget);
  widget_foreach(widget, date_picker_on_visit_child, widget);
  date_picker->inited = TRUE;

  return RET_OK;
}

static ret_t date_picker_on_event(widget_t* widget, event_t* e) {
  date_picker_t* date_picker = DATE_PICKER(widget);
  return_value_if_fail(widget != NULL && date_picker != NULL, RET_BAD_PARAMS);

  switch (e->type) {
    case EVT_WINDOW_OPEN: {
      date_picker_init(widget);
      break;
    }
  }
  (void)date_picker;

  return RET_OK;
}

const char* s_date_picker_properties[] = {DATE_PICKER_PROP_YEAR, DATE_PICKER_PROP_MONTH,
                                          DATE_PICKER_PROP_DAY, NULL};

TK_DECL_VTABLE(date_picker) = {.size = sizeof(date_picker_t),
                               .type = WIDGET_TYPE_DATE_PICKER,
                               .clone_properties = s_date_picker_properties,
                               .persistent_properties = s_date_picker_properties,
                               .parent = TK_PARENT_VTABLE(widget),
                               .create = date_picker_create,
                               .on_paint_self = date_picker_on_paint_self,
                               .set_prop = date_picker_set_prop,
                               .get_prop = date_picker_get_prop,
                               .on_event = date_picker_on_event,
                               .on_destroy = date_picker_on_destroy};

widget_t* date_picker_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, TK_REF_VTABLE(date_picker), x, y, w, h);
  date_picker_t* date_picker = DATE_PICKER(widget);
  return_value_if_fail(date_picker != NULL, NULL);

  date_picker->year = 2020;
  date_picker->month = 1;
  date_picker->day = 1;

  return widget;
}

widget_t* date_picker_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, date_picker), NULL);

  return widget;
}
