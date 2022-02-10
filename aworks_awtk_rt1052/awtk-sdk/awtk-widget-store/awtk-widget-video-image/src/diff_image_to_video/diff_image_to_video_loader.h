#ifndef TK_DIFF_IMAGE_TO_VIDEO_LOADER_H
#define TK_DIFF_IMAGE_TO_VIDEO_LOADER_H

#include "awtk.h"

BEGIN_C_DECLS

void diff_to_image_video_image_dispose(void);

ret_t diff_to_image_video_image_init(unsigned char *file_data,
                                     unsigned int file_data_length,
                                     unsigned int *w, unsigned int *h, 
                                     unsigned int *channels,
                                     unsigned int *line_length,
                                     unsigned int *frame_number_max,
                                     unsigned int *delays,
                                     bitmap_format_t *format);

ret_t diff_to_image_to_video_image(unsigned char *file_data,
                                   unsigned int file_data_length,
                                   unsigned int frame_number_curr,
                                   bitmap_t *image);

END_C_DECLS

#endif /*TK_DIFF_IMAGE_TO_VIDEO_LOADER_H*/
