﻿#include "base/window.h"
#include "base/canvas.h"
#include "base/widget.h"
#include "widgets/button.h"
#include "font_dummy.h"
#include "lcd_log.h"
#include "gtest/gtest.h"
#include <stdlib.h>

TEST(Window, basic) {
  value_t v1;
  value_t v2;
  widget_t* w = window_create(NULL, 10, 20, 30, 40);

  value_set_wstr(&v1, L"window");
  ASSERT_EQ(widget_is_normal_window(w), TRUE);
  ASSERT_EQ(widget_set_prop(w, WIDGET_PROP_TEXT, &v1), RET_OK);
  ASSERT_EQ(widget_get_prop(w, WIDGET_PROP_TEXT, &v2), RET_OK);
  ASSERT_EQ(wcscmp(v1.value.wstr, v2.value.wstr), 0);

  value_set_str(&v1, "bottom_to_top");
  ASSERT_EQ(widget_set_prop(w, WIDGET_PROP_ANIM_HINT, &v1), RET_OK);
  ASSERT_EQ(widget_get_prop(w, WIDGET_PROP_ANIM_HINT, &v2), RET_OK);
  ASSERT_EQ(strcmp(value_str(&v2), value_str(&v1)), 0);

  ASSERT_EQ(widget_get_prop(w, WIDGET_PROP_OPEN_ANIM_HINT, &v2), RET_OK);
  ASSERT_EQ(strcmp(value_str(&v2), value_str(&v1)), 0);

  ASSERT_EQ(widget_get_prop(w, WIDGET_PROP_CLOSE_ANIM_HINT, &v2), RET_OK);
  ASSERT_EQ(strcmp(value_str(&v2), value_str(&v1)), 0);

  value_set_str(&v1, "center_scale");
  ASSERT_EQ(widget_set_prop(w, WIDGET_PROP_OPEN_ANIM_HINT, &v1), RET_OK);
  ASSERT_EQ(widget_get_prop(w, WIDGET_PROP_OPEN_ANIM_HINT, &v2), RET_OK);
  ASSERT_EQ(strcmp(value_str(&v2), value_str(&v1)), 0);

  value_set_str(&v1, "center_scale");
  ASSERT_EQ(widget_set_prop(w, WIDGET_PROP_CLOSE_ANIM_HINT, &v1), RET_OK);
  ASSERT_EQ(widget_get_prop(w, WIDGET_PROP_CLOSE_ANIM_HINT, &v2), RET_OK);
  ASSERT_EQ(strcmp(value_str(&v2), value_str(&v1)), 0);

  value_set_str(&v1, "main");
  ASSERT_EQ(widget_set_prop(w, WIDGET_PROP_THEME, &v1), RET_OK);
  ASSERT_EQ(widget_get_prop(w, WIDGET_PROP_THEME, &v2), RET_OK);
  ASSERT_EQ(strcmp(value_str(&v2), value_str(&v1)), 0);

  ASSERT_EQ(widget_set_prop_bool(w, WIDGET_PROP_SINGLE_INSTANCE, TRUE), RET_OK);
  ASSERT_EQ(widget_get_prop_bool(w, WIDGET_PROP_SINGLE_INSTANCE, FALSE), TRUE);

  ASSERT_EQ(widget_set_prop_bool(w, WIDGET_PROP_SINGLE_INSTANCE, FALSE), RET_OK);
  ASSERT_EQ(widget_get_prop_bool(w, WIDGET_PROP_SINGLE_INSTANCE, TRUE), FALSE);

  widget_destroy(w);
}

TEST(Window, focus_key) {
  widget_t* w = window_create(NULL, 10, 20, 30, 40);

  ASSERT_EQ(widget_set_prop_str(w, WIDGET_PROP_MOVE_FOCUS_PREV_KEY, "UP"), RET_OK);
  ASSERT_STREQ(widget_get_prop_str(w, WIDGET_PROP_MOVE_FOCUS_PREV_KEY, 0), "UP");

  ASSERT_EQ(widget_set_prop_str(w, WIDGET_PROP_MOVE_FOCUS_NEXT_KEY, "DOWN"), RET_OK);
  ASSERT_STREQ(widget_get_prop_str(w, WIDGET_PROP_MOVE_FOCUS_NEXT_KEY, 0), "DOWN");

  widget_destroy(w);
}

TEST(Window, cast) {
  widget_t* w = window_create(NULL, 10, 20, 30, 40);
  ASSERT_EQ(w, window_cast(w));
  ASSERT_EQ(w, window_base_cast(w));
  widget_destroy(w);
}

TEST(Window, fullscreen) {
  widget_t* w = window_create(NULL, 10, 20, 30, 40);
  ASSERT_EQ(window_set_fullscreen(w, TRUE), RET_OK);

  ASSERT_EQ(w->x, 0);
  ASSERT_EQ(w->y, 0);
  ASSERT_EQ(w->w, w->parent->w);
  ASSERT_EQ(w->h, w->parent->h);
  ASSERT_EQ(WINDOW(w)->fullscreen, TRUE);
  ASSERT_EQ(WINDOW(w)->fullscreen, widget_get_prop_bool(w, WIDGET_PROP_FULLSCREEN, FALSE));

  widget_destroy(w);
}

TEST(Window, locale_info) {
  widget_t* w = window_create(NULL, 10, 20, 30, 40);

  ASSERT_EQ(widget_get_locale_info(w), locale_info());

  widget_destroy(w);
}

TEST(Window, image_manager) {
  widget_t* w = window_create(NULL, 10, 20, 30, 40);

  ASSERT_EQ(widget_get_image_manager(w), image_manager());

  widget_destroy(w);
}

TEST(Window, assets_manager) {
  widget_t* w = window_create(NULL, 10, 20, 30, 40);

  ASSERT_EQ(widget_get_assets_manager(w), assets_manager());

  widget_destroy(w);
}

TEST(Window, font_manager) {
  widget_t* w = window_create(NULL, 10, 20, 30, 40);

  ASSERT_EQ(widget_get_font_manager(w), font_manager());

  widget_destroy(w);
}

TEST(Window, focused_widget) {
  widget_t* w = window_create(NULL, 0, 0, 300, 400);
  widget_t* b = button_create(w, 0, 0, 60, 30);
  widget_set_focusable(b, TRUE);
  widget_set_name(b, "b1");

  b = button_create(w, 0, 30, 60, 30);
  widget_set_focusable(b, TRUE);
  widget_set_name(b, "b2");

  b = button_create(w, 0, 30, 60, 30);
  widget_set_focusable(b, TRUE);
  widget_set_name(b, "b3");

  widget_focus_first(w);
  ASSERT_STREQ(widget_get_focused_widget(b)->name, "b1");

  widget_focus_next(widget_get_focused_widget(b));
  ASSERT_STREQ(widget_get_focused_widget(b)->name, "b2");

  widget_focus_next(widget_get_focused_widget(b));
  ASSERT_STREQ(widget_get_focused_widget(b)->name, "b3");

  widget_destroy(w);
}

TEST(Window, strongly_focus) {
  value_t v1;
  value_t v2;
  widget_t* w = window_create(NULL, 10, 20, 30, 40);

  value_set_bool(&v1, TRUE);
  ASSERT_EQ(WINDOW_BASE(w)->strongly_focus, FALSE);
  ASSERT_EQ(widget_set_prop(w, WIDGET_PROP_STRONGLY_FOCUS, &v1), RET_OK);

  ASSERT_EQ(widget_get_prop(w, WIDGET_PROP_STRONGLY_FOCUS, &v2), RET_OK);
  ASSERT_EQ(WINDOW_BASE(w)->strongly_focus, TRUE);
  ASSERT_EQ(WINDOW_BASE(w)->strongly_focus, value_bool(&v2));
}

TEST(Window, open_bin) {
  widget_t* win = window_open("main");
  ASSERT_EQ(win != NULL, true);
  widget_destroy(win);
}

TEST(Window, open_xml) {
  widget_t* win = window_open("file://./design/default/ui/main.xml");
  ASSERT_EQ(win != NULL, true);
  widget_destroy(win);
}

TEST(Window, open_local_bin) {
  widget_t* win = window_open("file://./res/assets/default/raw/ui/main.bin");
  ASSERT_EQ(win != NULL, true);
  widget_destroy(win);
}
