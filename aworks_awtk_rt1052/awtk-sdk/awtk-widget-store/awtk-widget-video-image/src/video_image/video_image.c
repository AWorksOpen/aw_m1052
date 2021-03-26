/**
 * File:   video_image.c
 * Author: AWTK Develop Team
 * Brief:  帧间差异视频播放控件
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
 * 2020-06-18 Luo ZhiMing <luozhiming@zlg.cn> created
 *
 */

#include "tkc/mem.h"
#include "tkc/utils.h"
#include "video_image.h"
#include "../diff_image_to_video/diff_image_to_video_loader.h"

static ret_t video_image_on_open(void *ctx, event_t *e);

static ret_t video_image_bitmap_dispose(video_image_t *video_image, bitmap_t *bitmap) {
  if (bitmap != NULL) {
    if (bitmap->w != 0 || bitmap->h != 0) {
      if (video_image->dispose_image != NULL) {
        return video_image->dispose_image(video_image->dispose_image_ctx, bitmap);
      } else {
        bitmap_destroy(bitmap);

        bitmap->w = 0;
        bitmap->h = 0;

        diff_to_image_video_image_dispose();
      }
    }
  }
  return RET_OK;
}

static asset_info_t *video_image_load_video_asset(const char *video_name) {
  return_value_if_fail(video_name != NULL, NULL);
  return (asset_info_t *)assets_manager_ref(assets_manager(), ASSET_TYPE_DATA,
                                            video_name);
}

static ret_t video_image_bitmap_init(video_image_t *video_image) {
  asset_info_t *video_asset_info = NULL;
  return_value_if_fail(video_image != NULL && video_image->video_name != NULL,
                       RET_BAD_PARAMS);

  video_image_bitmap_dispose(video_image, &video_image->bitmap);

  if (video_image->video_asset_info != NULL) {
    assets_manager_unref(assets_manager(), video_image->video_asset_info);
  }

  video_asset_info = video_image_load_video_asset(video_image->video_name);
  if (video_asset_info != NULL) {
    uint32_t w = 0;
    uint32_t h = 0;
    ret_t ret = RET_FAIL;
    ret_t ret1 = RET_FAIL;
    uint32_t channels = 0;
    bitmap_format_t format = BITMAP_FMT_NONE;
    video_image->video_asset_info = video_asset_info;

    ret1 = diff_to_image_video_image_init(
        video_asset_info->data, video_asset_info->size, &w, &h, &channels, 
        &video_image->frame_number_max, &video_image->frame_interval, &format);
    
    if (ret1 == RET_OK) {
      if (video_image->init_image != NULL) {
        ret = video_image->init_image(video_image->init_image_ctx, &(video_image->bitmap), w, h, channels, format);
      }
      if (ret != RET_OK) {
        ret = bitmap_init(&(video_image->bitmap), w, h, format, NULL);
      }
    }
#ifdef WITH_NANOVG_GPU
    if (ret == RET_OK && video_image->bitmap.format != BITMAP_FMT_RGBA8888) {
      ret = RET_FAIL;
      video_image_bitmap_dispose(video_image, &video_image->bitmap);
      // assert(!"opengl only supported video's format RGBA8888");
    }
#endif
    if (ret == RET_OK) {
      video_image->frame_number_curr = 0;

      video_image->is_ready = TRUE;
      video_image->is_playing = FALSE;
    } else {
      video_image->is_ready = FALSE;
    }
    return ret;
  }
  video_image->is_ready = FALSE;
  return RET_NOT_FOUND;
}

ret_t video_image_set_video_name(widget_t *widget, const char *video_name) {
  video_image_t *video_image = VIDEO_IMAGE(widget);
  return_value_if_fail(video_image != NULL, RET_BAD_PARAMS);

  video_image->video_name = tk_str_copy(video_image->video_name, video_name);

  return video_image_bitmap_init(video_image);
}

ret_t video_image_set_auto_play(widget_t *widget, bool_t auto_play) {
  video_image_t *video_image = VIDEO_IMAGE(widget);
  return_value_if_fail(video_image != NULL, RET_BAD_PARAMS);

  video_image->auto_play = auto_play;

  if (auto_play) {
    video_image_on_open(widget, NULL);
  }

  return RET_OK;
}

ret_t video_image_set_draw_video_image(widget_t* widget, bool_t draw_video_image) {
  video_image_t *video_image = VIDEO_IMAGE(widget);
  return_value_if_fail(video_image != NULL, RET_BAD_PARAMS);

  video_image->draw_video_image = draw_video_image;
  return RET_OK;
}

ret_t video_image_set_delay_paly(widget_t *widget, uint32_t delay_paly) {
  video_image_t *video_image = VIDEO_IMAGE(widget);
  return_value_if_fail(video_image != NULL, RET_BAD_PARAMS);

  video_image->delay_paly = delay_paly;

  return RET_OK;
}

static ret_t video_image_get_prop(widget_t *widget, const char *name,
                                  value_t *v) {
  video_image_t *video_image = VIDEO_IMAGE(widget);
  return_value_if_fail(video_image != NULL && name != NULL && v != NULL,
                       RET_BAD_PARAMS);

  if (tk_str_eq(VIDEO_IMAGE_PROP_VIDEO_NAME, name)) {
    value_set_str(v, video_image->video_name);
    return RET_OK;
  } else if (tk_str_eq(VIDEO_IMAGE_PROP_AUTO_PLAY, name)) {
    value_set_bool(v, video_image->auto_play);
    return RET_OK;
  } else if (tk_str_eq(VIDEO_IMAGE_PROP_DELAY_PALY, name)) {
    value_set_uint32(v, video_image->delay_paly);
    return RET_OK;
  } else if (tk_str_eq(VIDEO_IMAGE_PROP_DRAW_VIDEO_IMAGE, name)) {
    value_set_bool(v, video_image->draw_video_image);
    return RET_OK;
  } 

  return RET_NOT_FOUND;
}

static ret_t video_image_set_prop(widget_t *widget, const char *name,
                                  const value_t *v) {
  video_image_t *video_image = VIDEO_IMAGE(widget);
  return_value_if_fail(widget != NULL && name != NULL && v != NULL,
                       RET_BAD_PARAMS);

  if (tk_str_eq(VIDEO_IMAGE_PROP_VIDEO_NAME, name)) {
    return video_image_set_video_name(widget, value_str(v));
  } else if (tk_str_eq(VIDEO_IMAGE_PROP_AUTO_PLAY, name)) {
    return video_image_set_auto_play(widget, value_bool(v));
  } else if (tk_str_eq(VIDEO_IMAGE_PROP_DELAY_PALY, name)) {
    return video_image_set_delay_paly(widget, value_uint32(v));
  } else if (tk_str_eq(VIDEO_IMAGE_PROP_DRAW_VIDEO_IMAGE, name)) {
    return video_image_set_draw_video_image(widget, value_bool(v));
  }

  return RET_NOT_FOUND;
}

static ret_t video_image_on_destroy(widget_t *widget) {
  video_image_t *video_image = VIDEO_IMAGE(widget);
  return_value_if_fail(widget != NULL && video_image != NULL, RET_BAD_PARAMS);

  video_image->frame_number_curr = 0;

  if (video_image->video_name != NULL) {
    TKMEM_FREE(video_image->video_name);
  }
  video_image_bitmap_dispose(video_image, &video_image->bitmap);
  memset(&video_image->bitmap, 0x0, sizeof(bitmap_t));

  if (video_image->video_asset_info != NULL) {
    assets_manager_unref(assets_manager(), video_image->video_asset_info);
  }

  return RET_OK;
}

static ret_t video_image_on_paint_self(widget_t *widget, canvas_t *c) {

  video_image_t *video_image = VIDEO_IMAGE(widget);
  rect_t dst = rect_init(0, 0, video_image->bitmap.w, video_image->bitmap.h);

  return_value_if_fail(video_image != NULL && video_image->video_name != NULL,
                       RET_BAD_PARAMS);

  if (video_image->is_ready && video_image->is_playing) {
    if (video_image->frame_number_curr < video_image->frame_number_max) {
      if (video_image->video_asset_info != NULL && !video_image->is_get_frame) {

        if (video_image->to_image_begin != NULL) {
          video_image->to_image_begin(video_image->to_image_begin_ctx, &(video_image->bitmap));
        }

        if(diff_to_image_to_video_image(video_image->video_asset_info->data,
                                     video_image->video_asset_info->size, video_image->frame_number_curr,
                                     &video_image->bitmap) == RET_OK) {
          video_image->is_get_frame = TRUE;
        }

        if (video_image->to_image_end != NULL) {
          video_image->to_image_end(video_image->to_image_end_ctx, &(video_image->bitmap), video_image->is_get_frame);
        }

      }
    }
    if (video_image->draw_video_image) {
      if (video_image->draw_image != NULL) {
        return video_image->draw_image(video_image->draw_image_ctx, &(video_image->bitmap), c);
      } else {
        return canvas_draw_image_ex(c, &(video_image->bitmap), IMAGE_DRAW_DEFAULT,
                                    &dst);
      }
    }
  }

  return RET_OK;
}

static ret_t video_image_on_timer_update(const timer_info_t *info) {
  ret_t ret = RET_REMOVE;
  widget_t *widget = WIDGET(info->ctx);
  video_image_t *video_image = VIDEO_IMAGE(widget);

  if (video_image->frame_number_curr < video_image->frame_number_max) {
    if (video_image->is_playing) {
      video_image->frame_number_curr++;
    }
    video_image->is_playing = TRUE;
    video_image->is_get_frame = FALSE;
    ret = RET_REPEAT;
  } else {
    event_t e = event_init(EVT_ANIM_END, widget);

    widget_dispatch(widget, &e);
    video_image->is_playing = FALSE;
  }
  widget_invalidate(widget, NULL);

  return ret;
}

ret_t video_image_play(widget_t *widget) {
  video_image_t *video_image = VIDEO_IMAGE(widget);
  return_value_if_fail(video_image != NULL, RET_BAD_PARAMS);

  if (video_image->timer_id == TK_INVALID_ID) {
    video_image->timer_id = timer_add(video_image_on_timer_update, widget,
                                      video_image->frame_interval);
  }

  return RET_OK;
}

static ret_t video_image_delay_play(const timer_info_t *info) {
  widget_t *widget = WIDGET(info->ctx);
  video_image_t *video_image = VIDEO_IMAGE(widget);

  video_image->timer_id = TK_INVALID_ID;
  video_image_play(widget);

  return RET_REMOVE;
}

static ret_t video_image_on_open(void *ctx, event_t *e) {
  widget_t *widget = WIDGET(ctx);
  video_image_t *video_image = VIDEO_IMAGE(widget);

  if (video_image->auto_play) {
    if (video_image->delay_paly > 0) {
      video_image->timer_id =
          timer_add(video_image_delay_play, widget, video_image->delay_paly);
    } else {
      video_image_play(widget);
    }
  }

  return RET_REMOVE;
}

const char *s_video_image_properties[] = {VIDEO_IMAGE_PROP_VIDEO_NAME,
                                          VIDEO_IMAGE_PROP_AUTO_PLAY,
                                          VIDEO_IMAGE_PROP_DRAW_VIDEO_IMAGE,
                                          VIDEO_IMAGE_PROP_DELAY_PALY, NULL};

TK_DECL_VTABLE(video_image) = {
    .size = sizeof(video_image_t),
    .type = WIDGET_TYPE_VIDEO_IMAGE,
    .clone_properties = s_video_image_properties,
    .persistent_properties = s_video_image_properties,
    .parent = TK_PARENT_VTABLE(widget),
    .create = video_image_create,
    .on_paint_self = video_image_on_paint_self,
    .set_prop = video_image_set_prop,
    .get_prop = video_image_get_prop,
    .on_destroy = video_image_on_destroy};

widget_t *video_image_create(widget_t *parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t *widget =
      widget_create(parent, TK_REF_VTABLE(video_image), x, y, w, h);
  video_image_t *video_image = VIDEO_IMAGE(widget);
  return_value_if_fail(video_image != NULL, NULL);

  video_image->delay_paly = 0;
  video_image->auto_play = TRUE;
  video_image->video_name = NULL;

  video_image->is_done = FALSE;
  video_image->is_ready = FALSE;
  video_image->is_playing = FALSE;
  video_image->draw_video_image = TRUE;
  video_image->video_asset_info = NULL;
  video_image->timer_id = TK_INVALID_ID;

  memset(&video_image->bitmap, 0x0, sizeof(bitmap_t));

  widget_on(parent, EVT_WINDOW_WILL_OPEN, video_image_on_open, widget);

  return widget;
}

widget_t *video_image_cast(widget_t *widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, video_image), NULL);

  return widget;
}

ret_t video_image_set_init_bitmap_func(widget_t* widget, video_image_init_image_t init_image, void* init_image_ctx) {
  video_image_t *video_image = VIDEO_IMAGE(widget);
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  video_image->init_image = init_image;
  video_image->init_image_ctx = init_image_ctx;

  return RET_OK;
}

ret_t video_image_set_dispose_image_func(widget_t* widget, video_image_dispose_image_t dispose_image, void* dispose_image_ctx) {
  video_image_t *video_image = VIDEO_IMAGE(widget);
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  video_image->dispose_image = dispose_image;
  video_image->dispose_image_ctx = dispose_image_ctx;

  return RET_OK;
}

ret_t video_image_set_to_image_begin_func(widget_t* widget, video_image_to_image_begin_t to_image_begin, void* to_image_begin_ctx) {
  video_image_t *video_image = VIDEO_IMAGE(widget);
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  video_image->to_image_begin = to_image_begin;
  video_image->to_image_begin_ctx = to_image_begin_ctx;

  return RET_OK;
}

ret_t video_image_set_to_image_end_func(widget_t* widget, video_image_to_image_end_t to_image_end, void* to_image_end_ctx) {
  video_image_t *video_image = VIDEO_IMAGE(widget);
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  video_image->to_image_end = to_image_end;
  video_image->to_image_end_ctx = to_image_end_ctx;

  return RET_OK;
}

ret_t video_image_set_draw_image_func(widget_t* widget, video_image_draw_image_t draw_image, void* draw_image_ctx) {
  video_image_t *video_image = VIDEO_IMAGE(widget);
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  video_image->draw_image = draw_image;
  video_image->draw_image_ctx = draw_image_ctx;

  return RET_OK;
}
