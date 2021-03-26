#include "tkc/mem.h"
#include "base/bitmap.h"
#include "base/image_manager.h"
#include "diff_image_to_video_types.h"
#include "diff_image_to_video_read_image_data.h"

static int read_video_frame_info(unsigned char *file_data,
                                 unsigned int file_data_length, unsigned int *w,
                                 unsigned int *h, unsigned int *channels,
                                 unsigned int *frame_number_max,
                                 unsigned int *gif_delay,
                                 bitmap_format_t *format) {
  video_info_t video_info;
  if (file_data == NULL || file_data_length < sizeof(video_info_t)) {
    return -1;
  }

  memcpy(&video_info, file_data, sizeof(video_info_t));

  *w = video_info.width;
  *h = video_info.height;
  *channels = video_info.channels;
  *frame_number_max = video_info.frame_number;
  *gif_delay = video_info.delays;

  if (video_info.frame_image_type == IMAGE_FMT_RGBA8888) {
    *format = BITMAP_FMT_RGBA8888;
  } else if (video_info.frame_image_type == IMAGE_FMT_BGRA8888) {
    *format = BITMAP_FMT_BGRA8888;
  } else if (video_info.frame_image_type == IMAGE_FMT_RGB565) {
    *format = BITMAP_FMT_RGB565;
  } else if (video_info.frame_image_type == IMAGE_FMT_BGR565) {
    *format = BITMAP_FMT_BGR565;
  } else if (video_info.frame_image_type == IMAGE_FMT_RGB888) {
    *format = BITMAP_FMT_RGB888;
  } else if (video_info.frame_image_type == IMAGE_FMT_BGR888) {
    *format = BITMAP_FMT_BGR888;
  } else {
    return -1;
  }

  return 0;
}

void diff_to_image_video_image_dispose(void) {

  diff_image_to_video_read_image_dispose();
}

ret_t diff_to_image_video_image_init(unsigned char *file_data,
                                     unsigned int file_data_length,
                                     unsigned int *w, unsigned int *h, 
                                     unsigned int *channels,
                                     unsigned int *frame_number_max,
                                     unsigned int *delays,
                                     bitmap_format_t *format) {
  ret_t ret = RET_OK;
  if (diff_image_to_video_read_image_init(file_data, file_data_length) != 0) {
    return RET_OOM;
  }

  if (read_video_frame_info(file_data, file_data_length, w, h, channels,
                            frame_number_max, delays, format) == 0) {

    return ret;
  }
  return RET_FAIL;
}

ret_t diff_to_image_to_video_image(unsigned char *file_data,
                                   unsigned int file_data_length,
                                   unsigned int frame_number_curr,
                                   bitmap_t *image) {
  int ret = 0;
  unsigned int w = 0;
  unsigned int h = 0;
  unsigned int channels = 0;
  unsigned char *image_data = NULL;

  return_value_if_fail(file_data != NULL && file_data_length > 0 &&
                           image != NULL,
                       RET_BAD_PARAMS);

  if (image->w == 0 || image->h == 0) {
    return RET_FAIL;
  }

  w = image->w;
  h = image->h;
  channels = bitmap_get_bpp(image);

  image_data = bitmap_lock_buffer_for_write(image);

  ret = diff_image_to_video_read_image_data(file_data, file_data_length,
                                            frame_number_curr, image_data,
                                            w, h, channels);

  bitmap_unlock_buffer(image);

  if (ret == 0) {
    image->flags |= BITMAP_FLAG_CHANGED;
    return RET_OK;
  } else {
    return RET_FAIL;
  }
}
