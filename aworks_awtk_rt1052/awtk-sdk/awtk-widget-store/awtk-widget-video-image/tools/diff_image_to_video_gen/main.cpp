#if defined(LINUX) || defined(WIN32)
#include "../3rd/diff_image_to_video/diff_image_to_video_save_image_data.h"
#include "../tools/common/utils.h"
#include "base/image_manager.h"
#include "image_loader/image_loader_stb.h"

#include <string>
#include <iostream>

using namespace std;
using namespace diff_image_video;

/**
 * @enum line_length_model_t
 * @annotation ["scriptable"]
 * @prefix line_length_model_
 * 图片的 line_length 长度对齐模式。
 */
typedef enum _line_length_model_t {
  /**
   * @const line_length_model_w_align
   * 图片的 line_length 长度会根据图片的 w 对齐后 * 图片的 bpp，数据为对齐个数。
   */
  line_length_model_w_align = 0x0,
  /**
   * @const line_length_model_line_length_align
   * 图片的 line_length 长度会根据图片的 w * 图片的 bpp 后对齐，数据为对齐字节数。
   */
  line_length_model_line_length_align,
  /**
   * @const line_length_model_set_line_length
   *  图片的 line_length 长度为数据长度。
   */
  line_length_model_set_line_length,
} line_length_model_t;

unsigned int get_image_line_length_by_w_align(unsigned int w, unsigned int bpp, void* ctx) {
  unsigned int align_number = tk_pointer_to_int(ctx);
  return bpp * TK_ROUND_TO(w, align_number);
}

unsigned int get_image_line_length_by_line_length_align(unsigned int w, unsigned int bpp,
                                                        void* ctx) {
  unsigned int align_number = tk_pointer_to_int(ctx);
  return TK_ROUND_TO(w * bpp, align_number);
}

unsigned int get_image_line_length_by_set_line_length(unsigned int w, unsigned int bpp, void* ctx) {
  unsigned int line_length = tk_pointer_to_int(ctx);
  return line_length;
}

image_format_t str_to_image_format(const char* str) {
  if (tk_str_cmp("RGBA8888", str) == 0 || tk_str_cmp("rgba8888", str) == 0) {
    return IMAGE_FMT_RGBA8888;
  } else if (tk_str_cmp("BGRA8888", str) == 0 || tk_str_cmp("bgra8888", str) == 0) {
    return IMAGE_FMT_BGRA8888;
  } else if (tk_str_cmp("RGB565", str) == 0 || tk_str_cmp("rgb565", str) == 0) {
    return IMAGE_FMT_RGB565;
  } else if (tk_str_cmp("BGR565", str) == 0 || tk_str_cmp("bgr565", str) == 0) {
    return IMAGE_FMT_BGR565;
  } else if (tk_str_cmp("RGB888", str) == 0 || tk_str_cmp("rgb888", str) == 0) {
    return IMAGE_FMT_RGB888;
  } else {
    return IMAGE_FMT_BGR888;
  }
}

int main(int argc, char** argv) {
  string image_dir;
  unsigned int delays;
  string save_file_path;
  string image_name_format;
  string save_res_file_path;
  diff_image_to_video_save_image_data save_handle;
  image_format_t frame_image_type = IMAGE_FMT_RGBA8888;

  if (argc < 5) {
    cout << "argvs: image_dir, image_name_format, save_file_path, "
            "frame_image_type, delays, [line_length] "
         << endl;
    return -1;
  }

  image_dir = argv[1];
  save_file_path = argv[3];
  image_name_format = argv[2];

  frame_image_type = str_to_image_format(argv[4]);

#if _MSC_VER > 1600
  sscanf_s(argv[5], "%d", &delays);
#else
  sscanf(argv[5], "%d", &delays);
#endif

  if (argc >= 7) {
    line_length_model_t model;
    unsigned int line_length_data = 0;
    tk_sscanf(argv[6], "%d:%d", &model, &line_length_data);

    switch (model) {
      case line_length_model_w_align:
        save_handle.set_get_image_line_length_func(get_image_line_length_by_w_align,
                                                   tk_pointer_from_int(line_length_data));
        break;
      case line_length_model_line_length_align:
        save_handle.set_get_image_line_length_func(get_image_line_length_by_line_length_align,
                                                   tk_pointer_from_int(line_length_data));
        break;
      case line_length_model_set_line_length:
        save_handle.set_get_image_line_length_func(get_image_line_length_by_set_line_length,
                                                   tk_pointer_from_int(line_length_data));
        break;
      default:
        break;
    }
  }
  if (save_handle.save_image_data_list(image_dir, image_name_format, save_file_path,
                                       frame_image_type, delays)) {
    return 0;
  }
  return -2;
}

#endif
