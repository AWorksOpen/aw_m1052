
#ifndef __RAW2RGB_H
#define __RAW2RGB_H

#include "aworks.h"
#include "aw_errno.h"

typedef enum {
    COLOR_FILTER_RGGB,
    COLOR_FILTER_GBRG,
    COLOR_FILTER_GRBG,
    COLOR_FILTER_BGGR
} color_filter_t ;


aw_err_t bayer_bilinear(const uint8_t * bayer, uint8_t * rgb, int sx, int sy, int tile);

#endif /* __RAW2RGB_H */
