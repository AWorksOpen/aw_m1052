#ifndef __DIFF_IMAGE_TO_VIDEO_SAVE_IMAGE_DATA_H__
#define __DIFF_IMAGE_TO_VIDEO_SAVE_IMAGE_DATA_H__

#include <list>
#include <vector>
#include <string>
#include "frame_image.hpp"
#include "../../../src/diff_image_to_video/diff_image_to_video_types.h"

typedef unsigned int (*diff_image_video_get_image_line_length_t)(unsigned int w, unsigned int bpp, void* ctx);

namespace diff_image_video {

#define RECT_SIZE 16

class diff_image_to_video_save_image_data {
private:
  void* ctx;
  std::list<std::string> file_format_list;
  diff_image_video_get_image_line_length_t get_image_line_length;
public:
  diff_image_to_video_save_image_data();
  ~diff_image_to_video_save_image_data();

  void set_get_image_line_length_func(diff_image_video_get_image_line_length_t get_image_line_length, void* ctx);

  unsigned int get_line_length(unsigned int w, unsigned int bpp);

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

private:
  unsigned int get_channels(image_format_t format);

  Rect boundingRect(std::vector<Point> point_list);

  bool get_diff_rect(const unsigned char *image1_data,
                     const unsigned char *image2_data, const Rect &rect,
                     unsigned int channels, unsigned int width_length,
                     Rect &diff_rect);

  void get_diff_rect_list(const frame_image &image1, const frame_image &image2,
                          unsigned int rect_size, std::list<Rect> &rect_list);

  void
  get_diff_image_data_list(const frame_image &image,
                           const std::list<Rect> &rect_list,
                           std::list<image_diff_data_t> &image_diff_data_list);

  unsigned char *get_diff_image_data_list_to_data(
      std::list<image_diff_data_t> &image_diff_data_list, unsigned int channels,
      bool is_delete_image_data, unsigned int &image_data_length);

  unsigned char *get_diff_image_data_list_to_lz4_data(
      std::list<image_diff_data_t> &image_diff_data_list, unsigned int channels,
      bool is_delete_image_data, unsigned int &image_data_length,
      unsigned int &image_data_lz4_length);

  unsigned char *get_lz4_date_to_image_date(unsigned char *data,
                                            unsigned int image_data_length,
                                            unsigned int &lz4_data_length);

  void get_dir_file_list(const std::string &dir, const std::string &file_format,
                         std::list<std::string> &file_list);

  void get_dir_file_list_sort(const std::string &dir,
                              const std::string &image_name_format,
                              std::list<std::string> &file_list);

  unsigned char *get_image_file_data(const std::string &image_file,
                                     unsigned int &data_length);

  void load_image(const std::string &image_file,
                  image_format_t frame_image_type, frame_image &frame);

  unsigned int save_first_frame(const frame_image &frame, FILE *save_file,
                                unsigned int &file_point,
                                frame_info_t &frame_info);

  void save_all_frame(const std::list<std::string> &file_list,
                      image_format_t frame_image_type, FILE *save_file,
                      unsigned int &file_point, unsigned int &frame_curr,
                      video_head_t &head, frame_image &frame_info);
};

}; // namespace diff_image_video

#endif // !__DIFF_IMAGE_TO_VIDEO_SAVE_IMAGE_DATA_H__
