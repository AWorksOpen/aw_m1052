/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
#ifndef __VIDEODEV2_H
#define __VIDEODEV2_H
#include "aw_time.h"
/* V4L2：在Linux系统的意思是“Video4Linux2”
 * 在 AWorks系统改为 V4AW：Video4AWorks*/


/*  四字符代码 (FOURCC) */
#define v4aw_fourcc(a, b, c, d)\
    ((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))
#define v4aw_fourcc_be(a, b, c, d)  (v4l2_fourcc(a, b, c, d) | (1 << 31))

/* see also http://vektor.theorem.ca/graphics/ycbcr/ */
enum v4aw_colorspace {
    /* SMPTE 170M: used for broadcast NTSC/PAL SDTV */
    V4AW_COLORSPACE_SMPTE170M     = 1,

    /* Obsolete pre-1998 SMPTE 240M HDTV standard, superseded by Rec 709 */
    V4AW_COLORSPACE_SMPTE240M     = 2,

    /* Rec.709: used for HDTV */
    V4AW_COLORSPACE_REC709        = 3,

    /*
     * Deprecated, do not use. No driver will ever return this. This was
     * based on a misunderstanding of the bt878 datasheet.
     */
    V4AW_COLORSPACE_BT878         = 4,

    /*
     * NTSC 1953 colorspace. This only makes sense when dealing with
     * really, really old NTSC recordings. Superseded by SMPTE 170M.
     */
    V4AW_COLORSPACE_470_SYSTEM_M  = 5,

    /*
     * EBU Tech 3213 PAL/SECAM colorspace. This only makes sense when
     * dealing with really old PAL/SECAM recordings. Superseded by
     * SMPTE 170M.
     */
    V4AW_COLORSPACE_470_SYSTEM_BG = 6,

    /*
     * Effectively shorthand for V4L2_COLORSPACE_SRGB, V4L2_YCBCR_ENC_601
     * and V4L2_QUANTIZATION_FULL_RANGE. To be used for (Motion-)JPEG.
     */
    V4AW_COLORSPACE_JPEG          = 7,

    /* For RGB colorspaces such as produces by most webcams. */
    V4AW_COLORSPACE_SRGB          = 8,

    /* AdobeRGB colorspace */
    V4AW_COLORSPACE_ADOBERGB      = 9,

    /* BT.2020 colorspace, used for UHDTV. */
    V4AW_COLORSPACE_BT2020        = 10,
};

/* 像素点格式结构体*/
struct v4aw_pix_format {
    uint32_t           width;          /* 宽度*/
    uint32_t           height;         /* 高度*/
    uint32_t           pixelformat;    /* 像素点格式*/
    uint32_t           field;      /* enum v4l2_field */
    uint32_t           bytesperline;   /* for padding, zero if unused */
    uint32_t           sizeimage;
    uint32_t           colorspace; /* enum v4l2_colorspace */
    uint32_t           priv;       /* private data, depends on pixelformat */
    uint32_t           flags;      /* format flags (V4L2_PIX_FMT_FLAG_*) */
    uint32_t           ycbcr_enc;  /* enum v4l2_ycbcr_encoding */
    uint32_t           quantization;   /* enum v4l2_quantization */
};

/* RGB格式*/
#define V4AW_PIX_FMT_RGB332  v4aw_fourcc('R', 'G', 'B', '1') /*  8  RGB-3-3-2     */
#define V4AW_PIX_FMT_RGB444  v4aw_fourcc('R', '4', '4', '4') /* 16  xxxxrrrr ggggbbbb */
#define V4AW_PIX_FMT_ARGB444 v4aw_fourcc('A', 'R', '1', '2') /* 16  aaaarrrr ggggbbbb */
#define V4AW_PIX_FMT_XRGB444 v4aw_fourcc('X', 'R', '1', '2') /* 16  xxxxrrrr ggggbbbb */
#define V4AW_PIX_FMT_RGB555  v4aw_fourcc('R', 'G', 'B', 'O') /* 16  RGB-5-5-5     */
#define V4AW_PIX_FMT_ARGB555 v4aw_fourcc('A', 'R', '1', '5') /* 16  ARGB-1-5-5-5  */
#define V4AW_PIX_FMT_XRGB555 v4aw_fourcc('X', 'R', '1', '5') /* 16  XRGB-1-5-5-5  */
#define V4AW_PIX_FMT_RGB565  v4aw_fourcc('R', 'G', 'B', 'P') /* 16  RGB-5-6-5     */
#define V4AW_PIX_FMT_RGB555X v4aw_fourcc('R', 'G', 'B', 'Q') /* 16  RGB-5-5-5 BE  */
#define V4AW_PIX_FMT_ARGB555X v4aw_fourcc_be('A', 'R', '1', '5') /* 16  ARGB-5-5-5 BE */
#define V4AW_PIX_FMT_XRGB555X v4aw_fourcc_be('X', 'R', '1', '5') /* 16  XRGB-5-5-5 BE */
#define V4AW_PIX_FMT_RGB565X v4aw_fourcc('R', 'G', 'B', 'R') /* 16  RGB-5-6-5 BE  */
#define V4AW_PIX_FMT_BGR666  v4aw_fourcc('B', 'G', 'R', 'H') /* 18  BGR-6-6-6     */
#define V4AW_PIX_FMT_BGR24   v4aw_fourcc('B', 'G', 'R', '3') /* 24  BGR-8-8-8     */
#define V4AW_PIX_FMT_RGB24   v4aw_fourcc('R', 'G', 'B', '3') /* 24  RGB-8-8-8     */
#define V4AW_PIX_FMT_BGR32   v4aw_fourcc('B', 'G', 'R', '4') /* 32  BGR-8-8-8-8   */
#define V4AW_PIX_FMT_ABGR32  v4aw_fourcc('A', 'R', '2', '4') /* 32  BGRA-8-8-8-8  */
#define V4AW_PIX_FMT_XBGR32  v4aw_fourcc('X', 'R', '2', '4') /* 32  BGRX-8-8-8-8  */
#define V4AW_PIX_FMT_RGB32   v4aw_fourcc('R', 'G', 'B', '4') /* 32  RGB-8-8-8-8   */
#define V4AW_PIX_FMT_ARGB32  v4aw_fourcc('B', 'A', '2', '4') /* 32  ARGB-8-8-8-8  */
#define V4AW_PIX_FMT_XRGB32  v4aw_fourcc('B', 'X', '2', '4') /* 32  XRGB-8-8-8-8  */

/* 灰色格式*/
#define V4AW_PIX_FMT_GREY    v4aw_fourcc('G', 'R', 'E', 'Y') /*  8  Greyscale     */
#define V4AW_PIX_FMT_Y4      v4aw_fourcc('Y', '0', '4', ' ') /*  4  Greyscale     */
#define V4AW_PIX_FMT_Y6      v4aw_fourcc('Y', '0', '6', ' ') /*  6  Greyscale     */
#define V4AW_PIX_FMT_Y10     v4aw_fourcc('Y', '1', '0', ' ') /* 10  Greyscale     */
#define V4AW_PIX_FMT_Y12     v4aw_fourcc('Y', '1', '2', ' ') /* 12  Greyscale     */
#define V4AW_PIX_FMT_Y16     v4aw_fourcc('Y', '1', '6', ' ') /* 16  Greyscale     */

/* 亮度和色度格式 */
#define V4AW_PIX_FMT_YVU410  v4aw_fourcc('Y', 'V', 'U', '9') /*  9  YVU 4:1:0     */
#define V4AW_PIX_FMT_YVU420  v4aw_fourcc('Y', 'V', '1', '2') /* 12  YVU 4:2:0     */
#define V4AW_PIX_FMT_YUYV    v4aw_fourcc('Y', 'U', 'Y', 'V') /* 16  YUV 4:2:2     */
#define V4AW_PIX_FMT_YYUV    v4aw_fourcc('Y', 'Y', 'U', 'V') /* 16  YUV 4:2:2     */
#define V4AW_PIX_FMT_YVYU    v4aw_fourcc('Y', 'V', 'Y', 'U') /* 16 YVU 4:2:2 */
#define V4AW_PIX_FMT_UYVY    v4aw_fourcc('U', 'Y', 'V', 'Y') /* 16  YUV 4:2:2     */
#define V4AW_PIX_FMT_VYUY    v4aw_fourcc('V', 'Y', 'U', 'Y') /* 16  YUV 4:2:2     */
#define V4AW_PIX_FMT_YUV422P v4aw_fourcc('4', '2', '2', 'P') /* 16  YVU422 planar */
#define V4AW_PIX_FMT_YUV411P v4aw_fourcc('4', '1', '1', 'P') /* 16  YVU411 planar */
#define V4AW_PIX_FMT_Y41P    v4aw_fourcc('Y', '4', '1', 'P') /* 12  YUV 4:1:1     */
#define V4AW_PIX_FMT_YUV444  v4aw_fourcc('Y', '4', '4', '4') /* 16  xxxxyyyy uuuuvvvv */
#define V4AW_PIX_FMT_YUV555  v4aw_fourcc('Y', 'U', 'V', 'O') /* 16  YUV-5-5-5     */
#define V4AW_PIX_FMT_YUV565  v4aw_fourcc('Y', 'U', 'V', 'P') /* 16  YUV-5-6-5     */
#define V4AW_PIX_FMT_YUV32   v4aw_fourcc('Y', 'U', 'V', '4') /* 32  YUV-8-8-8-8   */
#define V4AW_PIX_FMT_YUV410  v4aw_fourcc('Y', 'U', 'V', '9') /*  9  YUV 4:1:0     */
#define V4AW_PIX_FMT_YUV420  v4aw_fourcc('Y', 'U', '1', '2') /* 12  YUV 4:2:0     */
#define V4AW_PIX_FMT_HI240   v4aw_fourcc('H', 'I', '2', '4') /*  8  8-bit color   */
#define V4AW_PIX_FMT_HM12    v4aw_fourcc('H', 'M', '1', '2') /*  8  YUV 4:2:0 16x16 macroblocks */
#define V4AW_PIX_FMT_M420    v4aw_fourcc('M', '4', '2', '0') /* 12  YUV 4:2:0 2 lines y, 1 line uv interleaved */

/* 两个平面 -- 一个Y，一个Cr + Cb交错  */
#define V4AW_PIX_FMT_NV12    v4aw_fourcc('N', 'V', '1', '2') /* 12  Y/CbCr 4:2:0  */
#define V4AW_PIX_FMT_NV21    v4aw_fourcc('N', 'V', '2', '1') /* 12  Y/CrCb 4:2:0  */
#define V4AW_PIX_FMT_NV16    v4aw_fourcc('N', 'V', '1', '6') /* 16  Y/CbCr 4:2:2  */
#define V4AW_PIX_FMT_NV61    v4aw_fourcc('N', 'V', '6', '1') /* 16  Y/CrCb 4:2:2  */
#define V4AW_PIX_FMT_NV24    v4aw_fourcc('N', 'V', '2', '4') /* 24  Y/CbCr 4:4:4  */
#define V4AW_PIX_FMT_NV42    v4aw_fourcc('N', 'V', '4', '2') /* 24  Y/CrCb 4:4:4  */

/* Bayer formats - see http://www.siliconimaging.com/RGB%20Bayer.htm */
#define V4AW_PIX_FMT_SBGGR8  v4aw_fourcc('B', 'A', '8', '1') /*  8  BGBG.. GRGR.. */
#define V4AW_PIX_FMT_SGBRG8  v4aw_fourcc('G', 'B', 'R', 'G') /*  8  GBGB.. RGRG.. */
#define V4AW_PIX_FMT_SGRBG8  v4aw_fourcc('G', 'R', 'B', 'G') /*  8  GRGR.. BGBG.. */
#define V4AW_PIX_FMT_SRGGB8  v4aw_fourcc('R', 'G', 'G', 'B') /*  8  RGRG.. GBGB.. */
#define V4AW_PIX_FMT_SBGGR10 v4aw_fourcc('B', 'G', '1', '0') /* 10  BGBG.. GRGR.. */
#define V4AW_PIX_FMT_SGBRG10 v4aw_fourcc('G', 'B', '1', '0') /* 10  GBGB.. RGRG.. */
#define V4AW_PIX_FMT_SGRBG10 v4aw_fourcc('B', 'A', '1', '0') /* 10  GRGR.. BGBG.. */
#define V4AW_PIX_FMT_SRGGB10 v4aw_fourcc('R', 'G', '1', '0') /* 10  RGRG.. GBGB.. */
    /* 10bit raw bayer packed, 5 bytes for every 4 pixels */
#define V4AW_PIX_FMT_SBGGR10P v4aw_fourcc('p', 'B', 'A', 'A')
#define V4AW_PIX_FMT_SGBRG10P v4aw_fourcc('p', 'G', 'A', 'A')
#define V4AW_PIX_FMT_SGRBG10P v4aw_fourcc('p', 'g', 'A', 'A')
#define V4AW_PIX_FMT_SRGGB10P v4aw_fourcc('p', 'R', 'A', 'A')
    /* 10bit raw bayer a-law compressed to 8 bits */
#define V4AW_PIX_FMT_SBGGR10ALAW8 v4aw_fourcc('a', 'B', 'A', '8')
#define V4AW_PIX_FMT_SGBRG10ALAW8 v4aw_fourcc('a', 'G', 'A', '8')
#define V4AW_PIX_FMT_SGRBG10ALAW8 v4aw_fourcc('a', 'g', 'A', '8')
#define V4AW_PIX_FMT_SRGGB10ALAW8 v4aw_fourcc('a', 'R', 'A', '8')
    /* 10bit raw bayer DPCM compressed to 8 bits */
#define V4AW_PIX_FMT_SBGGR10DPCM8 v4aw_fourcc('b', 'B', 'A', '8')
#define V4AW_PIX_FMT_SGBRG10DPCM8 v4aw_fourcc('b', 'G', 'A', '8')
#define V4AW_PIX_FMT_SGRBG10DPCM8 v4aw_fourcc('B', 'D', '1', '0')
#define V4AW_PIX_FMT_SRGGB10DPCM8 v4aw_fourcc('b', 'R', 'A', '8')
#define V4AW_PIX_FMT_SBGGR12 v4aw_fourcc('B', 'G', '1', '2') /* 12  BGBG.. GRGR.. */
#define V4AW_PIX_FMT_SGBRG12 v4aw_fourcc('G', 'B', '1', '2') /* 12  GBGB.. RGRG.. */
#define V4AW_PIX_FMT_SGRBG12 v4aw_fourcc('B', 'A', '1', '2') /* 12  GRGR.. BGBG.. */
#define V4AW_PIX_FMT_SRGGB12 v4aw_fourcc('R', 'G', '1', '2') /* 12  RGRG.. GBGB.. */
#define V4AW_PIX_FMT_SBGGR16 v4aw_fourcc('B', 'Y', 'R', '2') /* 16  BGBG.. GRGR.. */

/* 压缩格式 */
#define V4AW_PIX_FMT_MJPEG    v4aw_fourcc('M', 'J', 'P', 'G') /* Motion-JPEG   */
#define V4AW_PIX_FMT_JPEG     v4aw_fourcc('J', 'P', 'E', 'G') /* JFIF JPEG     */
#define V4AW_PIX_FMT_DV       v4aw_fourcc('d', 'v', 's', 'd') /* 1394          */
#define V4AW_PIX_FMT_MPEG     v4aw_fourcc('M', 'P', 'E', 'G') /* MPEG-1/2/4 Multiplexed */
#define V4AW_PIX_FMT_H264     v4aw_fourcc('H', '2', '6', '4') /* H264 with start codes */
#define V4AW_PIX_FMT_H264_NO_SC v4aw_fourcc('A', 'V', 'C', '1') /* H264 without start codes */
#define V4AW_PIX_FMT_H264_MVC v4aw_fourcc('M', '2', '6', '4') /* H264 MVC */
#define V4AW_PIX_FMT_H263     v4aw_fourcc('H', '2', '6', '3') /* H263          */
#define V4AW_PIX_FMT_MPEG1    v4aw_fourcc('M', 'P', 'G', '1') /* MPEG-1 ES     */
#define V4AW_PIX_FMT_MPEG2    v4aw_fourcc('M', 'P', 'G', '2') /* MPEG-2 ES     */
#define V4AW_PIX_FMT_MPEG4    v4aw_fourcc('M', 'P', 'G', '4') /* MPEG-4 part 2 ES */
#define V4AW_PIX_FMT_XVID     v4aw_fourcc('X', 'V', 'I', 'D') /* Xvid           */
#define V4AW_PIX_FMT_VC1_ANNEX_G v4aw_fourcc('V', 'C', '1', 'G') /* SMPTE 421M Annex G compliant stream */
#define V4AW_PIX_FMT_VC1_ANNEX_L v4aw_fourcc('V', 'C', '1', 'L') /* SMPTE 421M Annex L compliant stream */
#define V4AW_PIX_FMT_VP8      v4aw_fourcc('V', 'P', '8', '0') /* VP8 */

/* "性能"字段的值*/
#define V4AW_CAP_VIDEO_CAPTURE      0x00000001  /* 视频捕获设备*/
#define V4AW_CAP_VIDEO_OUTPUT       0x00000002  /* 视频输出设备*/
#define V4AW_CAP_VIDEO_OVERLAY      0x00000004  /* Can do video overlay */
#define V4AW_CAP_VBI_CAPTURE        0x00000010  /* Is a raw VBI capture device */
#define V4AW_CAP_VBI_OUTPUT     0x00000020  /* Is a raw VBI output device */
#define V4AW_CAP_SLICED_VBI_CAPTURE 0x00000040  /* Is a sliced VBI capture device */
#define V4AW_CAP_SLICED_VBI_OUTPUT  0x00000080  /* Is a sliced VBI output device */
#define V4AW_CAP_RDS_CAPTURE        0x00000100  /* RDS data capture */
#define V4AW_CAP_VIDEO_OUTPUT_OVERLAY   0x00000200  /* Can do video output overlay */
#define V4AW_CAP_HW_FREQ_SEEK       0x00000400  /* Can do hardware frequency seek  */
#define V4AW_CAP_RDS_OUTPUT     0x00000800  /* Is an RDS encoder */

enum v4aw_buf_type {
    V4AW_BUF_TYPE_VIDEO_CAPTURE        = 1,
    V4AW_BUF_TYPE_VIDEO_OUTPUT         = 2,
    V4AW_BUF_TYPE_VIDEO_OVERLAY        = 3,
    V4AW_BUF_TYPE_VBI_CAPTURE          = 4,
    V4AW_BUF_TYPE_VBI_OUTPUT           = 5,
    V4AW_BUF_TYPE_SLICED_VBI_CAPTURE   = 6,
    V4AW_BUF_TYPE_SLICED_VBI_OUTPUT    = 7,
#if 1
    /* 实验的 */
    V4AW_BUF_TYPE_VIDEO_OUTPUT_OVERLAY = 8,
#endif
    V4AW_BUF_TYPE_VIDEO_CAPTURE_MPLANE = 9,
    V4AW_BUF_TYPE_VIDEO_OUTPUT_MPLANE  = 10,
    V4AW_BUF_TYPE_SDR_CAPTURE          = 11,
    /* 已弃用，不要用 */
    V4AW_BUF_TYPE_PRIVATE              = 0x80,
};

enum v4aw_ctrl_type {
    V4AW_CTRL_TYPE_INTEGER       = 1,
    V4AW_CTRL_TYPE_BOOLEAN       = 2,
    V4AW_CTRL_TYPE_MENU      = 3,
    V4AW_CTRL_TYPE_BUTTON        = 4,
    V4AW_CTRL_TYPE_INTEGER64     = 5,
    V4AW_CTRL_TYPE_CTRL_CLASS    = 6,
    V4AW_CTRL_TYPE_STRING        = 7,
    V4AW_CTRL_TYPE_BITMASK       = 8,
    V4AW_CTRL_TYPE_INTEGER_MENU  = 9,

    /* 命令类型是 >= 0x0100*/
    V4AW_CTRL_COMPOUND_TYPES     = 0x0100,
    V4AW_CTRL_TYPE_U8        = 0x0100,
    V4AW_CTRL_TYPE_U16       = 0x0101,
    V4AW_CTRL_TYPE_U32       = 0x0102,
};

enum v4aw_field {
    V4AW_FIELD_ANY           = 0, /* driver can choose from none,
                     top, bottom, interlaced
                     depending on whatever it thinks
                     is approximate ... */
    V4AW_FIELD_NONE          = 1, /* this device has no fields ... */
    V4AW_FIELD_TOP           = 2, /* top field only */
    V4AW_FIELD_BOTTOM        = 3, /* bottom field only */
    V4AW_FIELD_INTERLACED    = 4, /* both fields interlaced */
    V4AW_FIELD_SEQ_TB        = 5, /* both fields sequential into one
                     buffer, top-bottom order */
    V4AW_FIELD_SEQ_BT        = 6, /* same as above + bottom-top order */
    V4AW_FIELD_ALTERNATE     = 7, /* both fields alternating into
                     separate buffers */
    V4AW_FIELD_INTERLACED_TB = 8, /* both fields interlaced, top field
                     first and the top field is
                     transmitted first */
    V4AW_FIELD_INTERLACED_BT = 9, /* both fields interlaced, top field
                     first and the bottom field is
                     transmitted first */
};
/* 时间戳类型*/
#define V4AW_BUF_FLAG_TIMESTAMP_MASK        0x0000e000
#define V4AW_BUF_FLAG_TIMESTAMP_UNKNOWN     0x00000000
#define V4AW_BUF_FLAG_TIMESTAMP_MONOTONIC   0x00002000
#define V4AW_BUF_FLAG_TIMESTAMP_COPY        0x00004000
/* 时间戳源*/
#define V4AW_BUF_FLAG_TSTAMP_SRC_MASK       0x00070000
#define V4AW_BUF_FLAG_TSTAMP_SRC_EOF        0x00000000
#define V4AW_BUF_FLAG_TSTAMP_SRC_SOE        0x00010000
#endif
