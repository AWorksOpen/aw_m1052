#if defined(LINUX) || defined(WIN32)
#include "diff_image_to_video.h"
#include "diff_image_to_video_save_image_data.h"
#include "diff_image_to_video_read_image_data.h"

#ifdef WIN32

#include <Windows.h>

#else

#endif

using namespace std;

namespace diff_image_video {
diff_image_to_video::diff_image_to_video(){};
diff_image_to_video::~diff_image_to_video(){};

bool diff_image_to_video::save_image_data_list(const string &image_dir,
                                               const string &image_name_format,
                                               const string &save_file_path,
                                               image_format_t frame_image_type,
                                               unsigned int delays) {
  diff_image_to_video_save_image_data save_handle;
  return save_handle.save_image_data_list(
      image_dir, image_name_format, save_file_path, frame_image_type, delays);
}

bool diff_image_to_video::save_image_data_list(const list<string> &file_list,
                                               const string &image_name_format,
                                               const string &save_file_path,
                                               image_format_t frame_image_type,
                                               unsigned int delays) {
  diff_image_to_video_save_image_data save_handle;
  return save_handle.save_image_data_list(
      file_list, image_name_format, save_file_path, frame_image_type, delays);
}

bool diff_image_to_video::read_image_data(
    const unsigned char *file_data, unsigned int file_data_length,
    unsigned int frame_number, unsigned char **last_image_data,
    unsigned int &width, unsigned int &height, unsigned char &channels,
    unsigned int &delays) {
  int error = diff_image_to_video_read_image_data(
      file_data, file_data_length, frame_number, last_image_data, &width,
      &height, &channels, &delays);
  if (error == 0) {
    return true;
  } else {
    return false;
  }
}

}; // namespace diff_image_video

#endif
