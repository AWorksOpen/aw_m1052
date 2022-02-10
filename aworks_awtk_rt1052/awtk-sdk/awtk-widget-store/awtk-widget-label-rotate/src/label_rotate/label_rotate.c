/**
 * File:   label_rotate.c
 * Author: AWTK Develop Team
 * Brief:  可旋转 label 控件
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
 * 2020-06-16 Luo ZhiMing <luozhiming@zlg.cn> created
 *
 */



#include "tkc/mem.h"
#include "tkc/wstr.h"
#include "base/font.h"
#include "tkc/utils.h"
#include "label_rotate.h"
#include "base/widget_vtable.h"
#include "base/widget_factory.h"
#include "base/window_manager.h"

static int ox = 0;
static int oy = 0;
static lcd_draw_text_t label_draw_text = NULL;

static ret_t label_rotate_lcd_draw_text(lcd_t* lcd, const wchar_t* str, uint32_t nr, xy_t x, xy_t y) {
  return label_draw_text(lcd, str, nr, x - ox, y - oy);
}

static rect_t *label_rotate_canvas_fix_rect(const rect_t *r, rect_t *o) {
  if (r != NULL) {
    *o = *r;

    if (o->w < 0) {
      o->w = -o->w;
      o->x = o->x - o->w + 1;
    }

    if (o->h < 0) {
      o->h = -o->h;
      o->y = o->y - o->h + 1;
    }

    return o;
  } else {
    return NULL;
  }
}

static void rotate_image(xy_t sx, xy_t sy, wh_t sw, wh_t sh, xy_t *dx, xy_t *dy,
                         label_rotate_orientation_t o) {
  switch (o) {
  case LABEL_ROTATE_ORIENTATION_90:
    *dx = sw - sy - 1;
    *dy = sx;
    break;
  case LABEL_ROTATE_ORIENTATION_270:
    *dx = sy;
    *dy = sh - sx - 1;
    break;
  case LABEL_ROTATE_ORIENTATION_180:
    *dx = sw - sx - 1;
    *dy = sh - sy - 1;
    break;
  default:
    *dx = sx;
    *dy = sy;
    break;
  }
}

static ret_t label_rotate_special_draw_glyph_impl(widget_t *widget, canvas_t *c,
                                              glyph_t *glyph, rect_t *src,
                                              xy_t x, xy_t y) {

  xy_t i = 0;
  xy_t j = 0;
  xy_t dx = 0;
  xy_t dy = 0;
  wh_t sx = src->x;
  wh_t sy = src->y;
  wh_t sw = src->w;
  wh_t sh = src->h;
  color_t color = c->lcd->text_color;
  uint8_t color_alpha = color.rgba.a;
  label_rotate_t *label_rotate = LABEL_ROTATE(widget);
  uint8_t global_alpha = c->lcd->global_alpha;
  const uint8_t *src_p = glyph->data + glyph->w * sy + sx;

  for (j = 0; j < sh; j++) {
    const uint8_t *s = src_p;
    for (i = 0; i < sw; i++, s++) {
      rotate_image(x + i, y + j, widget->w, widget->h, &dx, &dy,
                   label_rotate->orientation);
      uint8_t a =
          global_alpha > TK_OPACITY_ALPHA ? *s : ((*s * global_alpha) >> 8);
      a = (a * color_alpha) >> 8;
      if (a >= TK_OPACITY_ALPHA) {
        color.rgba.a = color_alpha;
        canvas_set_stroke_color(c, color);
        canvas_draw_hline(c, dx, dy, 1);
      } else if (a >= TK_TRANSPARENT_ALPHA) {
        color.rgba.a = a;
        canvas_set_stroke_color(c, color);
        canvas_draw_hline(c, dx, dy, 1);
      }
    }
    src_p += glyph->w;
  }

  return RET_OK;
}

static ret_t label_rotate_special_draw_glyph(widget_t *widget, canvas_t *c,
                                         glyph_t *g, xy_t x, xy_t y) {
  rect_t src;
  src.x = 0;
  src.y = 0;
  src.w = g->w;
  src.h = g->h;

  return label_rotate_special_draw_glyph_impl(widget, c, g, &src, x, y);
}

static ret_t label_rotate_special_draw_text(widget_t *widget, canvas_t *c,
                                        const wchar_t *str, uint32_t nr, xy_t x,
                                        xy_t y) {
  glyph_t g;
  uint32_t i = 0;
  label_rotate_t *label_rotate = LABEL_ROTATE(widget);
  font_vmetrics_t vmetrics = font_get_vmetrics(c->font, c->font_size);
  int32_t baseline = vmetrics.ascent;

  for (i = 0; i < nr; i++) {
    wchar_t chr = str[i];
    if (font_get_glyph(c->font, chr, c->font_size, &g) == RET_OK) {
      xy_t xx = x + g.x;
      xy_t yy = y + g.y + baseline;

      if(g.data != NULL) {
        label_rotate_special_draw_glyph(widget, c, &g, xx, yy);
      }

      x += g.advance + 1;
    } else {
      x += 4;
    }
  }
  return RET_OK;
}

static int32_t label_rotate_measure_text_height(canvas_t *c, const wchar_t *str,
                                            uint32_t nr) {
  glyph_t g;
  uint32_t i = 0;
  uint32_t text_h = 0;
  font_vmetrics_t vmetrics = font_get_vmetrics(c->font, c->font_size);
  int32_t baseline = vmetrics.ascent;
  return_value_if_fail(c != NULL && str != NULL && c->font != NULL, 0);

  for (i = 0; i < nr; i++) {
    wchar_t chr = str[i];
    if (chr == ' ') {
      text_h += 4;
    } else if (font_get_glyph(c->font, chr, c->font_size, &g) == RET_OK) {
      text_h += (g.h - g.y - baseline - 1);
    }
  }
  return text_h;
}

static int32_t label_rotate_text_max_width(canvas_t *c, const wchar_t *str,
                                       uint32_t nr) {
  glyph_t g;
  uint32_t w = 0;
  uint32_t i = 0;
  uint32_t text_w = 0;
  return_value_if_fail(c != NULL && str != NULL && c->font != NULL, 0);

  for (i = 0; i < nr; i++) {
    wchar_t chr = str[i];
    if (font_get_glyph(c->font, chr, c->font_size, &g) == RET_OK) {
      w = g.advance + 1;
    } else {
      w = 4;
    } 

    if (w > text_w) {
      text_w = w;
    }
  }
  return text_w;
}

static ret_t label_rotate_text_in_rect(widget_t *widget, canvas_t *c,
                                   const wchar_t *str, uint32_t nr,
                                   const rect_t *r_in) {
  int x = 0;
  int y = 0;
  rect_t r_fix;
  int32_t text_w = 0;
  int32_t height = 0;
  label_rotate_t *label_rotate = LABEL_ROTATE(widget);
  rect_t *r = label_rotate_canvas_fix_rect(r_in, &r_fix);
  return_value_if_fail(c != NULL && str != NULL && r != NULL, RET_BAD_PARAMS);

  height = canvas_get_font_height(c);
  text_w = canvas_measure_text(c, str, nr);

  switch (c->text_align_v) {
  case ALIGN_V_TOP:
    y = r->y;
    break;
  case ALIGN_V_BOTTOM:
    y = r->y + (r->h - height);
    break;
  default:
    y = r->y + ((r->h - height) >> 1);
    break;
  }

  switch (c->text_align_h) {
  case ALIGN_H_LEFT:
    x = r->x;
    break;
  case ALIGN_H_RIGHT:
    x = r->x + (r->w - text_w);
    break;
  default:
    x = r->x + ((r->w - text_w) >> 1);
    break;
  }

  return label_rotate_special_draw_text(widget, c, str, nr, x, y);
}

static ret_t label_rotate_draw_text(widget_t *widget, canvas_t *c, wstr_t *text) {
  wh_t w = 0;
  wh_t h = 0;
  rect_t r_text;
  int32_t margin = 0;
  int32_t margin_left = 0;
  int32_t margin_right = 0;
  int32_t margin_top = 0;
  int32_t margin_bottom = 0;
  style_t *style = widget->astyle;
  label_rotate_t *label_rotate = LABEL_ROTATE(widget);
  return_value_if_fail(widget->astyle != NULL, RET_BAD_PARAMS);

  margin = style_get_int(style, STYLE_ID_MARGIN, 0);
  margin_top = style_get_int(style, STYLE_ID_MARGIN_TOP, margin);
  margin_left = style_get_int(style, STYLE_ID_MARGIN_LEFT, margin);
  margin_right = style_get_int(style, STYLE_ID_MARGIN_RIGHT, margin);
  margin_bottom = style_get_int(style, STYLE_ID_MARGIN_BOTTOM, margin);

  switch (label_rotate->orientation) {
  case LABEL_ROTATE_ORIENTATION_90:
  case LABEL_ROTATE_ORIENTATION_270:
    w = widget->h - margin_left - margin_right;
    h = widget->w - margin_top - margin_bottom;
    break;
  case LABEL_ROTATE_ORIENTATION_180:
  default:
    w = widget->w - margin_left - margin_right;
    h = widget->h - margin_top - margin_bottom;
    break;
  }

  r_text = rect_init(margin_left, margin_top, w, h);

  if (text == NULL) {
    text = &(widget->text);
  }

  label_rotate_text_in_rect(widget, c, text->str, text->size, &r_text);

  return RET_OK;
}

static ret_t label_rotate_set_text_style(widget_t* widget, canvas_t* c) {
  style_t* style = widget->astyle;
  color_t trans = color_init(0, 0, 0, 0);
  color_t tc = style_get_color(style, STYLE_ID_TEXT_COLOR, trans);
  const char* font_name = style_get_str(style, STYLE_ID_FONT_NAME, NULL);
  uint16_t font_size = style_get_int(style, STYLE_ID_FONT_SIZE, TK_DEFAULT_FONT_SIZE);
  align_h_t align_h = (align_h_t)style_get_int(style, STYLE_ID_TEXT_ALIGN_H, ALIGN_H_CENTER);
  align_v_t align_v = (align_v_t)style_get_int(style, STYLE_ID_TEXT_ALIGN_V, ALIGN_V_MIDDLE);

  canvas_set_text_color(c, tc);
  canvas_set_font(c, font_name, font_size);
  canvas_set_text_align(c, align_h, align_v);

  return RET_OK;
}

static ret_t label_rotate_on_paint_self(widget_t *widget, canvas_t *c) {
  ret_t ret = RET_OK;
  vgcanvas_t* vg = NULL;
  float_t rotation = 0.0f;
  float_t anchor_x = widget->w * 0.5f;
  float_t anchor_y = widget->h * 0.5f;
  label_rotate_t *label_rotate = LABEL_ROTATE(widget);

  if (widget->text.size > 0 && style_is_valid(widget->astyle)) {
    wstr_t str_text = widget->text;
    label_rotate_set_text_style(widget, c);
    if (c->lcd->type == LCD_VGCANVAS) {
      vg = canvas_get_vgcanvas(c);
      vgcanvas_save(vg);
      switch (label_rotate->orientation) {
      case LABEL_ROTATE_ORIENTATION_90:
        rotation = TK_D2R(90);
        break;
      case LABEL_ROTATE_ORIENTATION_270:
        rotation = TK_D2R(270);
        break;
      case LABEL_ROTATE_ORIENTATION_180:
        rotation = TK_D2R(180);
        break;
      default:
        break;
      }
      ox = c->ox;
      oy = c->oy;
      vgcanvas_translate(vg, c->ox, c->oy);
      vgcanvas_translate(vg, anchor_x, anchor_y);
      vgcanvas_rotate(vg, rotation);
      vgcanvas_translate(vg, -anchor_x, -anchor_y);

      label_draw_text = c->lcd->draw_text;
      c->lcd->draw_text = label_rotate_lcd_draw_text;
      ret = widget_paint_helper(widget, c, NULL, &str_text);
      c->lcd->draw_text = label_draw_text;
      vgcanvas_restore(vg);
      return ret;
    } else {
      return label_rotate_draw_text(widget, c, &str_text);
    }
  }

  return RET_OK;
}

static const char *
label_rotate_orientation_to_string(label_rotate_orientation_t orientation) {
  return LABEL_ROTATE_DIRECTION_STRING_LIST[(uint32_t)orientation];
}

static label_rotate_orientation_t
label_rotate_orientation_from_string(const char *str) {
  uint32_t i = 0;
  for (; i < LABEL_ROTATE_ORIENTATION_COUNT; i++) {
    if (tk_str_cmp(str, LABEL_ROTATE_DIRECTION_STRING_LIST[i]) == 0) {
      return (label_rotate_orientation_t)i;
    }
  }
  return LABEL_ROTATE_ORIENTATION_0;
}

ret_t label_rotate_set_length(widget_t *widget, int32_t length) {
  label_rotate_t *label_rotate = LABEL_ROTATE(widget);
  return_value_if_fail(label_rotate != NULL, RET_BAD_PARAMS);
  label_rotate->length = length;

  return widget_invalidate_force(widget, NULL);
}

ret_t label_rotate_set_orientation(widget_t *widget,
                               label_rotate_orientation_t orientation) {
  label_rotate_t *label_rotate = LABEL_ROTATE(widget);
  return_value_if_fail(label_rotate != NULL, RET_BAD_PARAMS);
  label_rotate->orientation = orientation;

  return widget_invalidate_force(widget, NULL);
}

static ret_t label_rotate_get_prop(widget_t *widget, const char *name, value_t *v) {
  label_rotate_t *label_rotate = LABEL_ROTATE(widget);
  return_value_if_fail(label_rotate != NULL && name != NULL && v != NULL,
                       RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_LENGTH)) {
    value_set_int(v, label_rotate->length);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_ORIENTATION)) {
    value_set_str(v, label_rotate_orientation_to_string(label_rotate->orientation));
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t label_rotate_set_prop(widget_t *widget, const char *name,
                               const value_t *v) {
  return_value_if_fail(widget != NULL && name != NULL && v != NULL,
                       RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_LENGTH)) {
    return label_rotate_set_length(widget, tk_roundi(value_float(v)));
  } else if (tk_str_eq(name, WIDGET_PROP_ORIENTATION)) {
    return label_rotate_set_orientation(
        widget, label_rotate_orientation_from_string(value_str(v)));
  }

  return RET_NOT_FOUND;
}

static const char *const s_label_rotate_properties[] = {
    WIDGET_PROP_LENGTH, WIDGET_PROP_ORIENTATION, NULL};

TK_DECL_VTABLE(label_rotate) = {.size = sizeof(label_rotate_t),
                            .type = WIDGET_TYPE_LABEL_ROTATE,
                            .parent = TK_PARENT_VTABLE(widget),
                            .clone_properties = s_label_rotate_properties,
                            .persistent_properties = s_label_rotate_properties,
                            .create = label_rotate_create,
                            .set_prop = label_rotate_set_prop,
                            .get_prop = label_rotate_get_prop,
                            .on_paint_self = label_rotate_on_paint_self};

widget_t *label_rotate_create(widget_t *parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t *widget = widget_create(parent, TK_REF_VTABLE(label_rotate), x, y, w, h);
  label_rotate_t *label_rotate = LABEL_ROTATE(widget);
  return_value_if_fail(label_rotate != NULL, NULL);

  label_rotate->length = -1;
  label_rotate->orientation = LABEL_ROTATE_ORIENTATION_0;

  return widget;
}

widget_t *label_rotate_cast(widget_t *widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, label_rotate), NULL);

  return widget;
}
