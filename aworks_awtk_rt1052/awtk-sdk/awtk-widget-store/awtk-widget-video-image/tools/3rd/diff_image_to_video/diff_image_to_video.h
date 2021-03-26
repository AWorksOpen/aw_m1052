#ifndef __DIFF_IMAGE_TO_VIDEO_H__
#define __DIFF_IMAGE_TO_VIDEO_H__

#include <list>
#include <string>
#include "diff_image_to_video_types.h"

namespace diff_image_video {
class diff_image_to_video {
public:
  diff_image_to_video();
  ~diff_image_to_video();

  bool save_image_data_list(const std::string &image_dir,
                            const std::string &image_name_format,
                            const std::string &save_file_path,
                            image_format_t frame_image_type,
                            unsigned int delays);

  bool save_image_data_list(const std::list<std::string> &file_list,
                            const std::string &image_name_format,
                            const std::string &save_file_path,
                            image_format_t frame_image_type,
                            unsigned int delays);

  static bool read_image_data(const unsigned char *file_data,
                              unsigned int file_data_length,
                              unsigned int frame_number,
                              unsigned char **last_image_data,
                              unsigned int &width, unsigned int &height,
                              unsigned char &channels, unsigned int &delays);
};

}; // namespace diff_image_video

#endif // !__DIFF_IMAGE_TO_VIDEO_H__
