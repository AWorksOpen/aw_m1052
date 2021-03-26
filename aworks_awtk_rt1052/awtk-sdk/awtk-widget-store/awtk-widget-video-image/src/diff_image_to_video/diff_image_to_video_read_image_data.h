﻿#ifndef __DIFF_IMAGE_TO_VIDEO_READ_IMAGE_DATA_H__
#define __DIFF_IMAGE_TO_VIDEO_READ_IMAGE_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

int diff_image_to_video_read_image_init(const unsigned char *file_data,
                                        unsigned int file_data_length);

void diff_image_to_video_read_image_dispose(void);

int diff_image_to_video_read_image_data(
    const unsigned char *file_data, unsigned int file_data_length,
    unsigned int frame_number, unsigned char *last_image_data,
    unsigned int width, unsigned int height, unsigned int channels);

#ifdef __cplusplus
}
#endif

#endif // !__DIFF_IMAGE_TO_VIDEO_READ_IMAGE_DATA_H__
