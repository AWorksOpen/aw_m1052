/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Stock Co.,  Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief 
 *
 * \internal
 * \par Modification history
 * - 1.00 20-10-12  win, first implementation.
 * \endinternal
 */

#ifndef __AW_MP3_INTERNAL_H
#define __AW_MP3_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/** \brief AWorks平台 */
#include "aw_types.h"
#include "aw_common.h"
#include "aw_assert.h"
#include "aw_vdebug.h"
#include "aw_mem.h"
#include "aw_cache.h"
#include "aw_sem.h"

#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"

/** \brief 标准层 */
#include "aw_mp3.h"

/** \brief stand c */
#include <string.h>

/**
 * \brief 使用匿名联合体段开始
 * @{
 */

#if defined(__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)

  /* 默认使能匿名联合体 */
#elif defined(__TMS470__)

  /* 默认使能匿名联合体 */
#elif defined(__TASKING__)
  #pragma warning 586
#else
  #warning Not supported compiler t
#endif

/** @} */

/**
 * \name 变量对齐定义
 * @{
 */
#if defined (__CC_ARM)
#define aw_mp3_var_aligned(n) __align(n)
#elif defined (__ICCARM__)
#define aw_mp3_var_aligned(n) _Pragma data_alignment = n
#elif defined (__GNUC__)
#define aw_mp3_var_aligned(n) __attribute__ ((aligned(n)))
#endif

/** @} */

#define AW_MP3_VTOS(n)     #n               /**< \brief Convert n to string */
#define AW_MP3_SVAL(m)     AW_MP3_VTOS(m)   /**< \brief Get string value of macro m */

#include "aw_mp3_src_cfg.h"
#include "aw_mp3_fixed.h"
#include "aw_mp3_huffman.h"
#include "aw_mp3_buf.h"
#include "aw_mp3_rngbuf.h"
#include "aw_mp3_time.h"
#include "aw_mp3_version.h"

#define AW_MP3_DEC_CHAR_BIT        (8)

#define AW_MP3_DEC_BUFFER_GUARD    (8)
#define AW_MP3_DEC_BUFFER_MD_LEN   (511 + 2048 + AW_MP3_DEC_BUFFER_GUARD)

/**
 * \brief 错误类型
 */
enum aw_mp3_dec_err {

    /** \brief no error */
    AW_MP3_DEC_ERR_NONE           = 0x0000,

    /** \brief input buffer too small (or EOF) */
    AW_MP3_DEC_ERR_BUFLEN         = 0x0001,

    /** \brief invalid (null) buffer pointer */
    AW_MP3_DEC_ERR_BUFPTR         = 0x0002,

    /** \brief not enough memory */
    AW_MP3_DEC_ERR_NOMEM          = 0x0031,

    /** \brief lost synchronization */
    AW_MP3_DEC_ERR_LOSTSYNC       = 0x0101,

    /** \brief reserved header layer value */
    AW_MP3_DEC_ERR_BADLAYER       = 0x0102,

    /** \brief forbidden bitrate value */
    AW_MP3_DEC_ERR_BADBITRATE     = 0x0103,

    /** \brief reserved sample frequency value */
    AW_MP3_DEC_ERR_BADSAMPLERATE  = 0x0104,

    /** \brief reserved emphasis value */
    AW_MP3_DEC_ERR_BADEMPHASIS    = 0x0105,

    /** \brief CRC check failed */
    AW_MP3_DEC_ERR_BADCRC         = 0x0201,

    /** \brief forbidden bit allocation value */
    AW_MP3_DEC_ERR_BADBITALLOC    = 0x0211,

    /** \brief bad scalefactor index */
    AW_MP3_DEC_ERR_BADSCALEFACTOR = 0x0221,

    /** \brief bad bitrate/mode combination */
    AW_MP3_DEC_ERR_BADMODE        = 0x0222,

    /** \brief bad frame length */
    AW_MP3_DEC_ERR_BADFRAMELEN    = 0x0231,

    /** \brief bad big_values count */
    AW_MP3_DEC_ERR_BADBIGVALUES   = 0x0232,

    /** \brief reserved block_type */
    AW_MP3_DEC_ERR_BADBLOCKTYPE   = 0x0233,

    /** \brief bad scalefactor selection info */
    AW_MP3_DEC_ERR_BADSCFSI       = 0x0234,

    /** \brief bad main_data_begin pointer */
    AW_MP3_DEC_ERR_BADDATAPTR     = 0x0235,

    /** \brief bad audio data length */
    AW_MP3_DEC_ERR_BADPART3LEN    = 0x0236,

    /** \brief bad Huffman table select */
    AW_MP3_DEC_ERR_BADHUFFTABLE   = 0x0237,

    /** \brief Huffman data overrun */
    AW_MP3_DEC_ERR_BADHUFFDATA    = 0x0238,

    /** \brief incompatible block_type for JS */
    AW_MP3_DEC_ERR_BADSTEREO      = 0x0239,

};

/**
 * \brief MPEG解码可选项
 */
enum aw_mp3_dec_option {

    /** \brief ignore CRC errors */
    AW_MP3_DEC_OPTION_IGNORECRC      = 0x0001,

    /** \brief generate PCM at 1/2 sample rate */
    AW_MP3_DEC_OPTION_HALFSAMPLERATE = 0x0002,

    /** \brief decode left channel only */
    AW_MP3_DEC_OPTION_LEFTCHANNEL    = 0x0010,

    /** \brief decode right channel only */
    AW_MP3_DEC_OPTION_RIGHTCHANNEL   = 0x0020,

    /** \brief combine channels */
    AW_MP3_DEC_OPTION_SINGLECHANNEL  = 0x0030
};

/**
 * \brief 量化等级
 */
struct aw_mp3_dec_quant_class {
    uint16_t nlevels;
    uint8_t  group;
    uint8_t  bits;
    int32_t  c;
    int32_t  d;
};

/**
 * \brief table for requantization
 *
 * rq_table[x].mantissa * 2^(rq_table[x].exponent) = x^(4/3)
 */
struct aw_mp3_dec_fixed_float {
    uint32_t mantissa;   /**< \brief 27bit */
    uint16_t exponent;   /**< \brief 5bit */
};

/** \brief Layer I linear table */
extern const int32_t g_mp3_dec_layer_i_linear[14];

extern const int32_t g_mp3_dec_sf_table[64];

/**
 * \brief 量化等级
 */
extern const struct aw_mp3_dec_quant_class g_mp3_dec_qc_table[17];

extern const struct aw_mp3_dec_fixed_float g_mp3_dec_rq_table[8207];

extern const int32_t g_mp3_dec_root_table[7];

extern const int32_t g_mp3_dec_cs[8];

extern const int32_t g_mp3_dec_ca[8];

extern const int32_t g_mp3_dec_imdct_s[6][6];

extern const int32_t g_mp3_dec_window_l[36];

extern const int32_t g_mp3_dec_window_s[12];

extern const int32_t g_mp3_dec_is_table[7];

extern const int32_t g_mp3_dec_is_lsf_table[2][15];

extern const int32_t g_mp3_dec_d[17][32];

/** \brief 内部层 */
#include "aw_mp3_dec_bit.h"
#include "aw_mp3_dec_stream.h"
#include "aw_mp3_dec_frame.h"
#include "aw_mp3_dec_layer.h"
#include "aw_mp3_dec_synth.h"

/**
 * \brief 使用匿名联合体段结束
 * @{
 */

#if defined(__CC_ARM)
  #pragma pop
#elif defined(__ICCARM__)

  /* 允许匿名联合体使能 */
#elif defined(__GNUC__)

  /* 默认使用匿名联合体 */
#elif defined(__TMS470__)

  /* 默认使用匿名联合体 */
#elif defined(__TASKING__)
  #pragma warning restore
#else
  #warning Not supported compiler t
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __AW_MP3_INTERNAL_H */

/* end of file */
