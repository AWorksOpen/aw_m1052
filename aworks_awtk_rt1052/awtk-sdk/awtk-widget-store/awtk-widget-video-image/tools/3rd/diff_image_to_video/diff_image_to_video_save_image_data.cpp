#if defined(LINUX) || defined(WIN32)
#include "diff_image_to_video_save_image_data.h"
#include "file_name_sort.hpp"
#include "../lz4/lz4_assets.h"
#include "image_loader/image_loader_stb.h"
#include "../3rd/stb/stb_image.h"
#include <math.h>
#include <list>

#ifdef WIN32

#include <Windows.h>
#include <io.h>
#else
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

using namespace std;

namespace diff_image_video {

#define ROUND_TO(size, round_size) ((((size) + round_size - 1) / round_size) * round_size)

#define PUSH_DIFF_RECT_LIST(list, rect) {               \
   if (rect.height > 0 && rect.width > 0)  {            \
      list.push_back(rect);                             \
   }                                                    \
}                                                       \

#define FWRITE(data, _ElementSize, _ElementCount, file, file_point)            \
  fwrite(data, _ElementSize, _ElementCount, file);                             \
  file_point += (_ElementSize * _ElementCount);

diff_image_to_video_save_image_data::diff_image_to_video_save_image_data() {
  file_format_list.push_back("jpg");
  file_format_list.push_back("png");
  file_format_list.push_back("bmp");
  this->ctx = NULL;
  this->get_image_line_length = NULL;
}

diff_image_to_video_save_image_data::~diff_image_to_video_save_image_data() {}

void diff_image_to_video_save_image_data::set_get_image_line_length_func(diff_image_video_get_image_line_length_t get_image_line_length, void* ctx) {
  this->ctx = ctx;
  this->get_image_line_length = get_image_line_length;
}

unsigned int diff_image_to_video_save_image_data::get_line_length(unsigned int w, unsigned int bpp) {
  if (get_image_line_length != NULL) {
    return get_image_line_length(w, bpp, ctx);
  } else {
    return w * bpp;
  }
}

unsigned int
diff_image_to_video_save_image_data::get_channels(image_format_t format) {
  unsigned int channels = 0;
  switch (format) {
  case IMAGE_FMT_RGB888:
    channels = 3;
    break;
  case IMAGE_FMT_BGR888:
    channels = 3;
    break;
  case IMAGE_FMT_RGBA8888:
    channels = 4;
    break;
  case IMAGE_FMT_BGRA8888:
    channels = 4;
    break;
  case IMAGE_FMT_BGR565:
    channels = 2;
    break;
  case IMAGE_FMT_RGB565:
    channels = 2;
    break;
  default:
    break;
  }
  return channels;
}

Rect diff_image_to_video_save_image_data::boundingRect(
    vector<Point> point_list) {
  Rect tmp;
  int xmin = 0, ymin = 0, xmax = -1, ymax = -1;
  vector<Point>::iterator _Last = point_list.end();
  vector<Point>::iterator _First = point_list.begin();
  for (; _First != _Last; ++_First) {
    vector<Point>::reference p = *_First;

    if (p.x > xmax) {
      xmax = p.x;
    }

    if (xmin == 0 || p.x < xmin) {
      xmin = p.x;
    }

    if (p.y > ymax) {
      ymax = p.y;
    }

    if (ymin == 0 || p.y < ymin) {
      ymin = p.y;
    }
  }
  tmp.x = xmin;
  tmp.y = ymin;
  tmp.width = xmax - xmin + 1;
  tmp.height = ymax - ymin + 1;
  return tmp;
}

bool diff_image_to_video_save_image_data::get_diff_rect(
    const unsigned char *image1_data, const unsigned char *image2_data,
    const Rect &rect, unsigned int channels, unsigned int width_length,
    Rect &diff_rect) {
  Point tmp;
  unsigned int x = 0;
  unsigned int y = 0;
  unsigned int i = 0;
  unsigned int p = 0;
  bool is_equal = false;

  const unsigned char *tmp_p1 = NULL;
  const unsigned char *tmp_p2 = NULL;

  vector<Point> diff_point_list;

  if (channels <= 0) {
    return false;
  }
  diff_point_list.reserve(rect.width * rect.height);
  for (x = 0; x < rect.width; x++) {
    for (y = 0; y < rect.height; y++) {
      is_equal = true;
      p = (rect.x + x) * channels + (rect.y + y) * width_length;
      tmp_p1 = (image1_data + p);
      tmp_p2 = (image2_data + p);

      for (i = 0; i < channels; i++) {
        if (tmp_p1[i] != tmp_p2[i]) {
          is_equal = false;
          break;
        }
      }

      if (!is_equal) {
        tmp.x = rect.x + x;
        tmp.y = rect.y + y;
        diff_point_list.push_back(tmp);
      }
    }
  }
  if (diff_point_list.size() > 0) {
    diff_rect = boundingRect(diff_point_list);
    return true;
  } else {
    return false;
  }
}

void diff_image_to_video_save_image_data::get_diff_rect_list(
    const frame_image &image1, const frame_image &image2,
    unsigned int rect_size, list<Rect> &rect_list) {
  unsigned int x = 0;
  unsigned int y = 0;

  Rect tmp;
  Rect diff_rect;
  list<Rect> tmp_rect_list;
  list<Rect> tmp_rect_x_list;

  tmp_rect_list.clear();

  if (image1.data != NULL && image2.data != NULL) {
    if (image1.width == image2.width && image1.height == image2.height) {
      for (y = 0; y < image2.height;) {
        for (x = 0; x < image2.width;) {
          tmp.x = x;
          tmp.y = y;
          tmp.width =
              rect_size + x > image2.width ? image2.width - x : rect_size;
          tmp.height =
              rect_size + y > image2.height ? image2.height - y : rect_size;

          if (get_diff_rect(image1.data, image2.data, tmp, image2.channels,
                            image2.line_length, diff_rect)) {
            tmp_rect_list.push_back(diff_rect);
          }
          x = min(rect_size + x, image2.width);
        }
        y = min(rect_size + y, image2.height);
      }
    }
  }

  list<Rect>::const_iterator rect;
  list<Rect>::const_iterator _Last = tmp_rect_list.end();
  list<Rect>::const_iterator _First = tmp_rect_list.begin();

  if (tmp_rect_list.size() > 0) {
    tmp = *_First;
    _First++;
    for (; _First != _Last; ++_First) {
      rect = _First;

      if (tmp.y == rect->y && tmp.height == rect->height &&
          tmp.width + tmp.x == rect->x) {
        tmp.width += rect->width;
      } else {
        PUSH_DIFF_RECT_LIST(rect_list, tmp);
        tmp = *rect;
      }
    }
    PUSH_DIFF_RECT_LIST(rect_list, tmp);
  }

  if (tmp_rect_x_list.size() > 0) {
    _Last = tmp_rect_x_list.end();
    _First = tmp_rect_x_list.begin();

    tmp = *_First;
    _First++;
    for (; _First != _Last; ++_First) {
      rect = _First;

      if (tmp.x == rect->x && tmp.width == rect->width &&
          tmp.height + tmp.y == rect->y) {
        tmp.height += rect->height;
      } else {
        PUSH_DIFF_RECT_LIST(rect_list, tmp);
        tmp = *rect;
      }
    }
    PUSH_DIFF_RECT_LIST(rect_list, tmp);
  }
}

void diff_image_to_video_save_image_data::get_diff_image_data_list(
    const frame_image &image, const list<Rect> &rect_list,
    list<image_diff_data_t> &image_diff_data_list) {
  image_diff_data_t diff_data;
  list<Rect>::const_iterator rect;
  list<Rect>::const_iterator _Last = rect_list.end();
  list<Rect>::const_iterator _First = rect_list.begin();

  if (image.data != NULL && image.width > 0 && image.height > 0 &&
      image.channels > 0) {
    for (; _First != _Last; ++_First) {
      rect = _First;

      unsigned int line_length = ROUND_TO(rect->width * image.channels, 4);
      unsigned int size = line_length * rect->height;
      unsigned int width_length = image.line_length;
      unsigned int data_width_length = image.channels * rect->width;

      if (width_length >= data_width_length &&
          image.height >= rect->height + rect->y) {
        diff_data.rect.x = rect->x;
        diff_data.rect.y = rect->y;
        diff_data.rect.width = rect->width;
        diff_data.rect.height = rect->height;
        diff_data.rect.line_length = line_length;
        diff_data.data = new unsigned char[size];
        memset(diff_data.data, 0x0, size);

        for (size_t y = 0; y < diff_data.rect.height; y++) {
          memcpy(diff_data.data + y * line_length,
                 image.data + diff_data.rect.x * image.channels +
                     (diff_data.rect.y + y) * width_length,
                 data_width_length);
        }

        image_diff_data_list.push_back(diff_data);
      }
    }
  }
}

unsigned char *
diff_image_to_video_save_image_data::get_diff_image_data_list_to_data(
    list<image_diff_data_t> &image_diff_data_list, unsigned int channels,
    bool is_delete_image_data, unsigned int &image_data_length) {
  unsigned int length = 0;
  unsigned char *data = NULL;
  unsigned int image_data_list_point = 0;
  list<image_diff_data_t>::iterator diff_data;
  list<image_diff_data_t>::iterator _Last = image_diff_data_list.end();
  list<image_diff_data_t>::iterator _First = image_diff_data_list.begin();

  if (image_diff_data_list.size() <= 0) {
    return NULL;
  }

  for (; _First != _Last; ++_First) {
    diff_data = _First;
    image_data_length +=
        (diff_data->rect.height * diff_data->rect.line_length +
         sizeof(image_diff_rect_t));
  }

  data = new unsigned char[image_data_length];

  _Last = image_diff_data_list.end();
  _First = image_diff_data_list.begin();
  for (; _First != _Last; ++_First) {
    length = 0;
    diff_data = _First;

    memcpy(data + image_data_list_point, &diff_data->rect,
           sizeof(image_diff_rect_t));
    image_data_list_point += sizeof(image_diff_rect_t);

    length = diff_data->rect.height * diff_data->rect.line_length;
    memcpy(data + image_data_list_point, diff_data->data, length);
    image_data_list_point += length;

    if (is_delete_image_data) {
      delete diff_data->data;
      diff_data->data = NULL;
    }
  }

  return data;
}

unsigned char *
diff_image_to_video_save_image_data::get_diff_image_data_list_to_lz4_data(
    list<image_diff_data_t> &image_diff_data_list, unsigned int channels,
    bool is_delete_image_data, unsigned int &image_data_length,
    unsigned int &image_data_lz4_length) {
  unsigned char *data_for_image_diff_data_list = NULL;
  unsigned char *data_for_image_diff_data_list_lz4 = NULL;

  data_for_image_diff_data_list = get_diff_image_data_list_to_data(
      image_diff_data_list, channels, is_delete_image_data, image_data_length);

  data_for_image_diff_data_list_lz4 = new unsigned char[image_data_length];

  image_data_lz4_length = lz4_compress_doubleBuffer_buffer(
      data_for_image_diff_data_list_lz4, image_data_length,
      data_for_image_diff_data_list, image_data_length);

  delete data_for_image_diff_data_list;
  data_for_image_diff_data_list = NULL;

  return data_for_image_diff_data_list_lz4;
}

#ifdef WIN32
void diff_image_to_video_save_image_data::get_dir_file_list(
    const string &dir, const string &file_format, list<string> &file_list) {
  string new_file;
  string path = dir;
  path.append("\\*.");
  path.append(file_format);
  WIN32_FIND_DATAA info;
  memset(&info, 0, sizeof(WIN32_FIND_DATAA));
  HANDLE hfile = FindFirstFileA(path.c_str(), &info);
  if (hfile == NULL || hfile == INVALID_HANDLE_VALUE)
    return;

  do {
    if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      if (strcmp(info.cFileName, ".") != 0 &&
          strcmp(info.cFileName, "..") != 0) {
        string new_dir = dir;
        new_dir.append("\\");
        new_dir.append(info.cFileName);

        get_dir_file_list(new_dir, file_format, file_list);
      }
    } else {
      new_file = dir;
      new_file.append("\\");
      new_file.append(info.cFileName);
      if (_access(new_file.c_str(), 0) != -1) {
        file_list.push_back(new_file);
      }
    }
  } while (FindNextFileA(hfile, &info));
}
#else
void diff_image_to_video_save_image_data::get_dir_file_list(
    const string &dir, const string &file_format, list<string> &file_list) {
  DIR *dp;
  string format;
  string childpath;
  struct stat statbuf;
  struct dirent *entry;
  if ((dp = opendir(dir.c_str())) != NULL) {
    while ((entry = readdir(dp)) != NULL) {
      if (stat(entry->d_name, &statbuf) >= 0 && S_ISDIR(statbuf.st_mode)) {
        if (strcmp(".", entry->d_name) == 0 ||
            strcmp("..", entry->d_name) == 0) {
          continue;
        }

        childpath = dir;
        childpath.append("/");
        childpath.append(entry->d_name);
        get_dir_file_list(childpath, file_format, file_list);
      } else {
        childpath = dir;
        childpath.append("/");
        childpath.append(entry->d_name);

        format = childpath.substr(childpath.find_last_of('.') + 1);

        if (format == file_format) {
          file_list.push_back(childpath);
        }
      }
    }

    closedir(dp);
  }
}
#endif // WINDOW

unsigned char *diff_image_to_video_save_image_data::get_image_file_data(
    const std::string &image_file, unsigned int &data_length) {
  long size = 0;
  FILE *in_File;
  unsigned char *buff = NULL;

#ifdef WIN32
  fopen_s(&in_File, image_file.c_str(), "rb");
#else
  in_File = fopen(image_file.c_str(), "rb");
#endif // WINDOW

  if (in_File == NULL)
    return NULL;

  fseek(in_File, 0, SEEK_END);
  size = ftell(in_File);
  fseek(in_File, 0, SEEK_SET);

  buff = new unsigned char[size];
  fread(buff, size, 1, in_File);
  fclose(in_File);

  data_length = size;

  return buff;
}

void diff_image_to_video_save_image_data::load_image(
    const string &image_file, image_format_t frame_image_type,
    frame_image &frame) {
  bitmap_t image;

  unsigned int buff_length = 0;

  bool require_bgra = false;
  bool enable_bgr565 = false;
  bool enable_rgb565 = false;

  unsigned char *image_data = NULL;
  unsigned char *buff = get_image_file_data(image_file, buff_length);

  if (buff != NULL) {
    frame.channels = get_channels(frame_image_type);
    enable_bgr565 = frame_image_type == IMAGE_FMT_BGR565;
    enable_rgb565 = frame_image_type == IMAGE_FMT_RGB565;
    require_bgra = frame_image_type == IMAGE_FMT_BGRA8888;
    if (stb_load_image(0, buff, buff_length, &image, require_bgra,
                       enable_bgr565, enable_rgb565) == RET_OK) {
      unsigned char* dst_data = NULL;
      unsigned int src_line_length = bitmap_get_line_length(&image);
      frame.width = image.w;
      frame.height = image.h;
      frame.line_length = get_line_length(frame.width, frame.channels);
      frame.dispose();
      if (frame.channels != bitmap_get_bpp(&image)) {
        printf(" image not support change target color type ! \r\n");
        assert(" image not support change target color type !");
        return;
      }
      frame.data =
          new unsigned char[frame.height * frame.line_length];
      memset(frame.data, 0x0, frame.height * frame.line_length);
#ifdef TK_GRAPHIC_BUFFER_H
      image_data = bitmap_lock_buffer_for_read(&image);
#else
      image_data = (unsigned char *)(image.data);
#endif
      dst_data = frame.data;
      for (int h = 0; h < frame.height; h++) {
        memcpy(dst_data, image_data, src_line_length);
        dst_data += frame.line_length;
        image_data += src_line_length;
      }

#ifdef TK_GRAPHIC_BUFFER_H
      bitmap_unlock_buffer(&image);
#endif

      bitmap_destroy(&image);
    }
    delete buff;
  }
}

void diff_image_to_video_save_image_data::get_dir_file_list_sort(
    const string &dir, const string &image_name_format,
    list<string> &file_list) {
  list<string>::const_iterator _Last = file_format_list.end();
  list<string>::const_iterator _First = file_format_list.begin();

  for (; _First != _Last; ++_First) {
    get_dir_file_list(dir, *_First, file_list);
  }

  file_list.sort(FileNameSort(image_name_format));
}

unsigned char *diff_image_to_video_save_image_data::get_lz4_date_to_image_date(
    unsigned char *data, unsigned int image_data_length,
    unsigned int &lz4_data_length) {
  unsigned char *lz4_data = new unsigned char[image_data_length];

  memset(lz4_data, 0, sizeof(unsigned char) * image_data_length);

  lz4_data_length = lz4_compress_doubleBuffer_buffer(
      lz4_data, image_data_length, data, image_data_length);

  return lz4_data;
}

unsigned int diff_image_to_video_save_image_data::save_first_frame(
    const frame_image &frame, FILE *save_file, unsigned int &file_point,
    frame_info_t &frame_info) {
  unsigned char *lz4_data = NULL;

  frame_info.start_point = file_point;
  frame_info.data_decompress_length =
      frame.width * frame.height * frame.channels;
  frame_info.compress_type = compress_type_lz4;

  lz4_data = get_lz4_date_to_image_date(
      frame.data, frame_info.data_decompress_length, frame_info.data_length);

  fwrite(lz4_data, sizeof(unsigned char), frame_info.data_length, save_file);

  delete lz4_data;
  lz4_data = NULL;

  file_point += frame_info.data_length;

  return frame_info.data_length;
}

void diff_image_to_video_save_image_data::save_all_frame(
    const list<string> &file_list, image_format_t frame_image_type,
    FILE *save_file, unsigned int &file_point, unsigned int &frame_curr,
    video_head_t &head, frame_image &frame_info) {
  unsigned int image_data_length = 0;
  unsigned int image_diff_lz4_length = 0;
  unsigned int image_data_lz4_length = 0;

  unsigned char *lz4_diff_data = NULL;
  unsigned char *lz4_image_data = NULL;

  frame_image image;
  frame_image last_image;

  list<Rect> rect_list;
  list<image_diff_data_t> image_diff_data_list;

  string file_path;
  list<string>::const_iterator _Last = file_list.end();
  list<string>::const_iterator _First = ++file_list.begin();

  last_image = frame_info;

  for (; _First != _Last; ++_First) {
    file_path = *_First;

    rect_list.clear();
    image_diff_data_list.clear();
    image_data_length = 0;

    load_image(file_path, frame_image_type, image);
    if (image.data == NULL) {
      break;
    }

    get_diff_rect_list(last_image, image, RECT_SIZE, rect_list);

    if (rect_list.size() > 0) {
      get_diff_image_data_list(image, rect_list, image_diff_data_list);

      lz4_diff_data = get_diff_image_data_list_to_lz4_data(
          image_diff_data_list, image.channels, true, image_data_length,
          image_diff_lz4_length);

      lz4_image_data = get_lz4_date_to_image_date(
          image.data, image.width * image.height * image.channels,
          image_data_lz4_length);
    } else {
      image_data_lz4_length = 0;
      image_diff_lz4_length = 0;
    }

    head.frame_info_list[frame_curr].start_point = file_point;

    if (image_data_lz4_length > image_diff_lz4_length) {
      head.frame_info_list[frame_curr].data_length = image_diff_lz4_length;
      head.frame_info_list[frame_curr].compress_type = compress_type_diff_lz4;
      head.frame_info_list[frame_curr].data_decompress_length =
          image_data_length;
      FWRITE(lz4_diff_data, sizeof(unsigned char), image_diff_lz4_length,
             save_file, file_point);
    } else if (image_data_lz4_length == 0 && image_diff_lz4_length == 0) {
      head.frame_info_list[frame_curr].data_length = 0;
      head.frame_info_list[frame_curr].compress_type = compress_type_no_change;
      head.frame_info_list[frame_curr].data_decompress_length = 0;
    } else {
      head.frame_info_list[frame_curr].compress_type = compress_type_lz4;
      head.frame_info_list[frame_curr].data_length = image_data_lz4_length;
      head.frame_info_list[frame_curr].data_decompress_length =
          image.width * image.height * image.channels;

      FWRITE(lz4_image_data, sizeof(unsigned char), image_data_lz4_length,
             save_file, file_point);
    }

    frame_curr++;
    last_image = image;

    printf("frame_curr:%u, image_data_length:%u, diff_length:%u, "
           "lz4_length:%u, rect_list:%u, type:%s \n",
           frame_curr - 1, image_data_length, image_diff_lz4_length,
           image_data_lz4_length, (unsigned int)rect_list.size(),
           image_data_lz4_length > image_diff_lz4_length ? "diff" : image_data_lz4_length == 0 && image_diff_lz4_length == 0 ? "no_change" : "lz4");

    delete lz4_diff_data;
    lz4_diff_data = NULL;
    delete lz4_image_data;
    lz4_image_data = NULL;
  }
}

bool diff_image_to_video_save_image_data::save_image_data_list(
    const string &image_dir, const string &image_name_format,
    const string &save_file_path, image_format_t frame_image_type,
    unsigned int delays) {
  list<string> file_list;
  get_dir_file_list_sort(image_dir, image_name_format, file_list);

  return save_image_data_list(file_list, image_name_format, save_file_path,
                              frame_image_type, delays);
}

bool diff_image_to_video_save_image_data::save_image_data_list(
    const list<string> &file_list, const string &image_name_format,
    const string &save_file_path, image_format_t frame_image_type,
    unsigned int delays) {
  video_head_t head;
  bool is_su = false;
  unsigned int frame_curr = 0;
  unsigned int file_point = 0;
  FILE *save_file = NULL;

  frame_image last_image;

#ifdef WIN32
  fopen_s(&save_file, save_file_path.c_str(), "wb+");
#else
  save_file = fopen(save_file_path.c_str(), "wb+");
#endif // WINDOW

  if (save_file == NULL) {
    return is_su;
  }

  if (file_list.size() > 0) {
    load_image(*file_list.begin(), frame_image_type, last_image);
    if (last_image.data == NULL) {
      goto error;
    }
    head.video_info.delays = delays;
    head.video_info.width = last_image.width;
    head.video_info.height = last_image.height;
    head.video_info.channels = last_image.channels;
    head.video_info.frame_image_type = frame_image_type;
    head.video_info.line_length = last_image.line_length;
    head.video_info.frame_number = (unsigned int)file_list.size();
    head.video_info.time_length = delays * (unsigned int)file_list.size();
    head.frame_info_list = new frame_info_t[head.video_info.frame_number];
    printf("image.width:%d, image.height:%d, image.channels:%d, image.line_length:%d, files_count:%d \r\n", 
            last_image.width, last_image.height, last_image.channels, last_image.line_length, (unsigned int)file_list.size());
    memset(head.frame_info_list, 0,
           sizeof(frame_info_t) * head.video_info.frame_number);

    FWRITE(&head.video_info, sizeof(video_info_t), 1, save_file, file_point);
    FWRITE(head.frame_info_list, sizeof(frame_info_t),
           head.video_info.frame_number, save_file, file_point);

    save_first_frame(last_image, save_file, file_point,
                     head.frame_info_list[0]);
    frame_curr++;

    save_all_frame(file_list, frame_image_type, save_file, file_point,
                   frame_curr, head, last_image);

    fseek(save_file, sizeof(video_info_t), SEEK_SET);

    fwrite(head.frame_info_list, sizeof(frame_info_t),
           head.video_info.frame_number, save_file);

    delete head.frame_info_list;
    head.frame_info_list = NULL;
    is_su = true;
  }
error:
  fclose(save_file);

  return is_su;
}

}; // namespace diff_image_video
#endif
