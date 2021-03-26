#if defined(LINUX) || defined(WIN32)
#include "../3rd/diff_image_to_video/diff_image_to_video.h"
#include "../tools/common/utils.h"
#include "base/image_manager.h"
#include "image_loader/image_loader_stb.h"

#include <string>
#include <iostream>

using namespace std;
using namespace diff_image_video;

image_format_t str_to_image_format(const char *str) {
  if (tk_str_cmp("RGBA8888", str) == 0 || tk_str_cmp("rgba8888", str) == 0) {
    return IMAGE_FMT_RGBA8888;
  } else if (tk_str_cmp("BGRA8888", str) == 0 ||
             tk_str_cmp("bgra8888", str) == 0) {
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

int main(int argc, char **argv) {
  string image_dir;
  string save_file_path;
  string image_name_format;
  string save_res_file_path;

  unsigned int delays;

  diff_image_to_video video_handle;

  image_format_t frame_image_type = IMAGE_FMT_RGBA8888;

  if (argc < 5) {
    cout << "argvs: image_dir, image_name_format, save_file_path, "
            "frame_image_type, delays, " << endl;
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

  if (video_handle.save_image_data_list(image_dir, image_name_format,
                                        save_file_path, frame_image_type,
                                        delays)) {
    return 0;
  }
  return -2;
}

#endif
