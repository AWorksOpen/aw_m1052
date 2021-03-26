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
 * - 1.00 20-10-15  win, first implementation.
 * \endinternal
 */

#include "aw_mp3_internal.h"

/* possible quantization per subband table */
const static struct {
    uint32_t      sblimit;
    const uint8_t offsets[30];
} __g_layer_ii_sbquant_table[5] = {

    /* ISO/IEC 11172-3 Table B.2a */
    {
        27,

        /* 0 */
        {
            7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3,
            3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0
        },
    },

    /* ISO/IEC 11172-3 Table B.2b */
    {
        30,

        /* 1 */
        {
            7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3,
            3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0
        },
    },

    /* ISO/IEC 11172-3 Table B.2c */
    {
        8,

        /* 2 */
        {
            5, 5, 2, 2, 2, 2, 2, 2, 0
        },
    },

    /* ISO/IEC 11172-3 Table B.2d */
    {
        12,

        /* 3 */
        {
            5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
        },
    },

    /* ISO/IEC 13818-3 Table B.1 */
    {
        30,

        /* 4 */
        {
            4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        },
    },
};

/**
 * \brief bit allocation table
 */
const static struct {
    uint16_t nbal;
    uint16_t offset;
} __g_layer_ii_bitalloc_table[8] = {
    { 2, 0 },  /* 0 */
    { 2, 3 },  /* 1 */
    { 3, 3 },  /* 2 */
    { 3, 1 },  /* 3 */
    { 4, 2 },  /* 4 */
    { 4, 3 },  /* 5 */
    { 4, 4 },  /* 6 */
    { 4, 5 }   /* 7 */
};

/**
 * \brief offsets into quantization class table
 */
const static uint8_t __g_layer_ii_offset_table[6][15] = {
    { 0, 1, 16                                             },  /* 0 */
    { 0, 1,  2, 3, 4, 5, 16                                },  /* 1 */
    { 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 14 },  /* 2 */
    { 0, 1,  3, 4, 5, 6,  7, 8,  9, 10, 11, 12, 13, 14, 15 },  /* 3 */
    { 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 16 },  /* 4 */
    { 0, 2,  4, 5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 15, 16 }   /* 5 */
};

/**
 * \brief scalefactor bit lengths
 *        derived from section 2.4.2.7 of ISO/IEC 11172-3
 */
const static struct {
    uint8_t slen1;
    uint8_t slen2;
} __g_layer_iii_sflen_table[16] = {
  { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 },
  { 3, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 },
  { 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 1 },
  { 3, 2 }, { 3, 3 }, { 4, 2 }, { 4, 3 },
};

/**
 * \brief number of LSF scalefactor band values
 *        derived from section 2.4.3.2 of ISO/IEC 13818-3
 */
const static uint8_t __layer_iii_nsfb_table[6][3][4] = {
    {
        { 6, 5, 5, 5 },
        { 9, 9, 9, 9 },
        { 6, 9, 9, 9 },
    },

    {
        { 6, 5, 7,  3 },
        { 9, 9, 12, 6 },
        { 6, 9, 12, 6 },
    },

    {
        { 11, 10, 0, 0 },
        { 18, 18, 0, 0 },
        { 15, 18, 0, 0 },
    },

    {
        { 7,  7,  7,  0 },
        { 12, 12, 12, 0 },
        { 6,  15, 12, 0 },
    },

    {
        { 6,  6,  6, 3 },
        { 12, 9,  9, 6 },
        { 6,  12, 9, 6 },
    },

    {
        { 8,  8,  5, 0 },
        { 15, 12, 9, 0 },
        { 6,  18, 9, 0 },
    },
};

/**
 * \brief MPEG-1 scalefactor band widths
 *        derived from Table B.8 of ISO/IEC 11172-3
 */
const static uint8_t __g_layer_iii_sfb_48000_long[] = {
    4,  4,  4,  4,  4,  4,  6,  6,  6,  8,  10,
    12, 16, 18, 22, 28, 34, 40, 46, 54, 54, 192,
};

const static uint8_t __g_layer_iii_sfb_44100_long[] = {
    4,  4,  4,  4,  4,  4,  6,  6,  8,  8,  10,
    12, 16, 20, 24, 28, 34, 42, 50, 54, 76, 158,
};

const static uint8_t __g_layer_iii_sfb_32000_long[] = {
    4,  4,  4,  4,  4,  4,  6,  6,  8,  10, 12,
    16, 20, 24, 30, 38, 46, 56, 68, 84, 102, 26,
};

const static uint8_t __g_layer_iii_sfb_48000_short[] = {
    4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  6,
    6,  6,  6,  6,  6,  10, 10, 10, 12, 12, 12, 14, 14,
    14, 16, 16, 16, 20, 20, 20, 26, 26, 26, 66, 66, 66,
};

const static uint8_t __g_layer_iii_sfb_44100_short[] = {
   4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  6,
   6,  6,  8,  8,  8,  10, 10, 10, 12, 12, 12, 14, 14,
   14, 18, 18, 18, 22, 22, 22, 30, 30, 30, 56, 56, 56,
};

const static uint8_t __g_layer_iii_sfb_32000_short[] = {
    4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  6,
    6,  6,  8,  8,  8,  12, 12, 12, 16, 16, 16, 20, 20,
    20, 26, 26, 26, 34, 34, 34, 42, 42, 42, 12, 12, 12,
};

const static uint8_t __g_layer_iii_sfb_48000_mixed[] = {
    4,  4,  4,  4,  4,  4,  6,  6,
    4,  4,  4,  6,  6,  6,  6,  6,  6,  10,
    10, 10, 12, 12, 12, 14, 14, 14, 16, 16,
    16, 20, 20, 20, 26, 26, 26, 66, 66, 66
};

const static uint8_t __g_layer_iii_sfb_44100_mixed[] = {
    4,  4,  4,  4,  4,  4,  6,  6,
    4,  4,  4,  6,  6,  6,  8,  8,  8,  10,
    10, 10, 12, 12, 12, 14, 14, 14, 18, 18,
    18, 22, 22, 22, 30, 30, 30, 56, 56, 56
};

const static uint8_t __g_layer_iii_sfb_32000_mixed[] = {
    4,  4,  4,  4,  4,  4,  6,  6,
    4,  4,  4,  6,  6,  6,  8,  8,  8,  12,
    12, 12, 16, 16, 16, 20, 20, 20, 26, 26,
    26, 34, 34, 34, 42, 42, 42, 12, 12, 12,
};

/**
 * \brief MPEG-2 scalefactor band widths
 *        derived from Table B.2 of ISO/IEC 13818-3
 */
const static uint8_t __g_layer_iii_sfb_24000_long[] = {
    6,  6,  6,  6,  6,  6,  8,  10, 12, 14, 16,
    18, 22, 26, 32, 38, 46, 54, 62, 70, 76, 36,
};

const static uint8_t __g_layer_iii_sfb_22050_long[] = {
    6,  6,  6,  6,  6,  6,  8,  10, 12, 14, 16,
    20, 24, 28, 32, 38, 46, 52, 60, 68, 58, 54,
};

#define __g_layer_iii_sfb_16000_long  __g_layer_iii_sfb_22050_long

const static uint8_t __g_layer_iii_sfb_24000_short[] = {
    4,  4,  4,  4,  4,  4,  4,  4,  4,  6,  6,  6,  8,
    8,  8,  10, 10, 10, 12, 12, 12, 14, 14, 14, 18, 18,
    18, 24, 24, 24, 32, 32, 32, 44, 44, 44, 12, 12, 12,
};

const static uint8_t __g_layer_iii_sfb_22050_short[] = {
    4,  4,  4,  4,  4,  4,  4,  4,  4,  6,  6,  6,  6,
    6,  6,  8,  8,  8,  10, 10, 10, 14, 14, 14, 18, 18,
    18, 26, 26, 26, 32, 32, 32, 42, 42, 42, 18, 18, 18,
};

const static uint8_t __g_layer_iii_sfb_16000_short[] = {
    4,  4,  4,  4,  4,  4,  4,  4,  4,  6,  6,  6,  8,
    8,  8,  10, 10, 10, 12, 12, 12, 14, 14, 14, 18, 18,
    18, 24, 24, 24, 30, 30, 30, 40, 40, 40, 18, 18, 18,
};

const static uint8_t __g_layer_iii_sfb_24000_mixed[] = {
    6,  6,  6,  6,  6,  6,
    6,  6,  6,  8,  8,  8,  10, 10, 10, 12,
    12, 12, 14, 14, 14, 18, 18, 18, 24, 24,
    24, 32, 32, 32, 44, 44, 44, 12, 12, 12,
};

const static uint8_t __g_layer_iii_sfb_22050_mixed[] = {
    6,  6,  6,  6,  6,  6,
    6,  6,  6,  6,  6,  6,  8,  8,  8,  10,
    10, 10, 14, 14, 14, 18, 18, 18, 26, 26,
    26, 32, 32, 32, 42, 42, 42, 18, 18, 18,
};

const static uint8_t __g_layer_iii_sfb_16000_mixed[] = {
    6,  6,  6,  6,  6,  6,
    6,  6,  6,  8,  8,  8,  10, 10, 10, 12,
    12, 12, 14, 14, 14, 18, 18, 18, 24, 24,
    24, 30, 30, 30, 40, 40, 40, 18, 18, 18
};

/**
 * \brief MPEG 2.5 scalefactor band widths
 *        derived from public sources
 */
#define __g_layer_iii_sfb_12000_long  __g_layer_iii_sfb_16000_long
#define __g_layer_iii_sfb_11025_long  __g_layer_iii_sfb_12000_long

const static uint8_t __g_layer_iii_sfb_8000_long[] = {
  12, 12, 12, 12, 12, 12, 16, 20, 24,  28,  32,
  40, 48, 56, 64, 76, 90,  2,  2,  2,   2,   2
};

# define __g_layer_iii_sfb_12000_short  __g_layer_iii_sfb_16000_short
# define __g_layer_iii_sfb_11025_short  __g_layer_iii_sfb_12000_short

const static uint8_t __g_layer_iii_sfb_8000_short[] = {
    8,  8,  8,  8,  8,  8,  8,  8,  8,  12, 12, 12, 16,
    16, 16, 20, 20, 20, 24, 24, 24, 28, 28, 28, 36, 36,
    36, 2,  2,  2,  2,  2,  2,  2,  2,  2,  26, 26, 26
};

# define __g_layer_iii_sfb_12000_mixed  __g_layer_iii_sfb_16000_mixed
# define __g_layer_iii_sfb_11025_mixed  __g_layer_iii_sfb_12000_mixed

/**
 * \brief the 8000 Hz short block scalefactor bands do not break after
 *  the first 36 frequency lines, so this is probably wrong
 */
const static uint8_t __g_layer_iii_sfb_8000_mixed[] = {
    12, 12, 12,
    4,  4,  4,  8,  8,  8,  12, 12, 12, 16, 16, 16,
    20, 20, 20, 24, 24, 24, 28, 28, 28, 36, 36, 36,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  26, 26, 26
};

/**
 * \brief layer iii sfb width table
 */
const static struct {
    const uint8_t *p_l;
    const uint8_t *p_s;
    const uint8_t *p_m;
} __layer_iii_sfb_width_table[9] = {

    {
        __g_layer_iii_sfb_48000_long,
        __g_layer_iii_sfb_48000_short,
        __g_layer_iii_sfb_48000_mixed
    },

    {
        __g_layer_iii_sfb_44100_long,
        __g_layer_iii_sfb_44100_short,
        __g_layer_iii_sfb_44100_mixed
    },

    {
        __g_layer_iii_sfb_32000_long,
        __g_layer_iii_sfb_32000_short,
        __g_layer_iii_sfb_32000_mixed
    },

    {
        __g_layer_iii_sfb_24000_long,
        __g_layer_iii_sfb_24000_short,
        __g_layer_iii_sfb_24000_mixed
    },

    {
        __g_layer_iii_sfb_22050_long,
        __g_layer_iii_sfb_22050_short,
        __g_layer_iii_sfb_22050_mixed
    },

    {
        __g_layer_iii_sfb_16000_long,
        __g_layer_iii_sfb_16000_short,
        __g_layer_iii_sfb_16000_mixed
    },

    {
        __g_layer_iii_sfb_12000_long,
        __g_layer_iii_sfb_12000_short,
        __g_layer_iii_sfb_12000_mixed
    },

    {
        __g_layer_iii_sfb_11025_long,
        __g_layer_iii_sfb_11025_short,
        __g_layer_iii_sfb_11025_mixed
    },

    {
        __g_layer_iii_sfb_8000_long,
        __g_layer_iii_sfb_8000_short,
        __g_layer_iii_sfb_8000_mixed
    },
};

/**
 * \brief scalefactor band preemphasis (used only when preflag is set)
 *        derived from Table B.6 of ISO/IEC 11172-3
 */
const static uint8_t __g_layer_iii_pretab[22] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 3, 2, 0
};

/**
 * \brief Layer III flag
 */
enum __layer_iii_flg {
    __LAYER_III_FLG_COUNT1_TABLE_SELECT = 0x01,
    __LAYER_III_FLG_SCALEFAC_SCALE      = 0x02,
    __LAYER_III_FLG_PRE                 = 0x04,
    __LAYER_III_FLG_MIXED_BLOCK         = 0x08
};

/**
 * \brief stereo
 */
enum __layer_iii_stereo {
     __LAYER_III_STEREO_I  = 0x1,
     __LAYER_III_STEREO_MS = 0x2
};

/**
 * \brief Layer III side information
 */
struct __layer_iii_side_info {

    uint32_t main_data_begin;
    uint32_t private_bits;

    uint8_t  scfsi[2];

    struct __layer_iii_granule {
        struct __layer_iii_channel {

            /* from side info */
            uint16_t part2_3_length;
            uint16_t big_values;
            uint16_t global_gain;
            uint16_t scalefac_compress;

            uint8_t  flags;
            uint8_t block_type;
            uint8_t table_select[3];
            uint8_t subblock_gain[3];
            uint8_t region0_count;
            uint8_t region1_count;

            /* from main_data */
            uint8_t scalefac[39];   /* scalefac_l and/or scalefac_s */
        } ch[2];
    } gr[2];

};

/**
 * \brief we must take care that sz >= bits and
 *        sz < sizeof(long) lest bits == 0
 */
# define __LAYER_MASK(cache, sz, bits)  \
    (((cache) >> ((sz) - (bits))) & ((1 << (bits)) - 1))

# define __LAYER_MASK_BIT(cache, sz)  \
    ((cache) & (1 << ((sz) - 1)))

/******************************************************************************/

/**
 * \brief decode one requantized Layer I sample from a bitstream
 */
static int32_t __layer_i_sample (struct aw_mp3_dec_bit *p_ptr,
                                 uint32_t               nb)
{
    int32_t sample;

    sample = aw_mp3_dec_bit_read(p_ptr, nb);

    /* invert most significant bit, extend sign, then scale to fixed format */

    sample ^= 1 << (nb - 1);
    sample |= -(sample & (1 << (nb - 1)));

    sample <<= AW_MP3_F_FRACBITS - (nb - 1);

    /* requantize the sample */

    /* s'' = (2^nb / (2^nb - 1)) * (s''' + 2^(-nb + 1)) */

    sample += AW_MP3_F_ONE >> (nb - 1);

    return AW_MP3_F_MUL(sample, g_mp3_dec_layer_i_linear[nb - 2]);

    /* s' = factor * s'' */
    /* (to be performed by caller) */
}

/**
 * \brief decode three requantized Layer II samples from a bitstream
 */
static
void __layer_ii_samples (struct aw_mp3_dec_bit               *p_ptr,
                         const struct aw_mp3_dec_quant_class *p_quant_class,
                         int32_t                              output[3])
{
    uint32_t nb, s, sample[3];

    if ((nb = p_quant_class->group) != 0) {
        uint32_t c, nlevels;

        /* degrouping */
        c       = aw_mp3_dec_bit_read(p_ptr, p_quant_class->bits);
        nlevels = p_quant_class->nlevels;

        for (s = 0; s < 3; ++s) {
            sample[s]  = c % nlevels;
            c         /= nlevels;
        }
    } else {
        nb = p_quant_class->bits;

        for (s = 0; s < 3; ++s) {
            sample[s] = aw_mp3_dec_bit_read(p_ptr, nb);
        }
    }

    for (s = 0; s < 3; ++s) {
        int32_t requantized;

        /*
         * invert most significant bit,
         * extend sign,
         * then scale to fixed format
         */

        requantized  = sample[s] ^ (1 << (nb - 1));
        requantized |= -(requantized & (1 << (nb - 1)));

        requantized <<= AW_MP3_F_FRACBITS - (nb - 1);

        /* requantize the sample */

        /* s'' = C * (s''' + D) */

        output[s] = AW_MP3_F_MUL(requantized + p_quant_class->d,
                                 p_quant_class->c);

        /* s' = factor * s'' */
        /* (to be performed by caller) */
    }
}

/**
 * \brief decode frame side information from a bitstream
 */
static enum aw_mp3_dec_err
__layer_iii_side_info (struct aw_mp3_dec_bit        *p_ptr,
                       uint32_t                      nch,
                       int                           lsf,
                       struct __layer_iii_side_info *p_si,
                       uint32_t                     *p_data_bit_len,
                       uint32_t                     *p_priv_bit_len)
{
    uint32_t            ngr;
    uint32_t            gr;
    uint32_t            ch;
    uint32_t            i;
    enum aw_mp3_dec_err result = AW_MP3_DEC_ERR_NONE;

    *p_data_bit_len = 0;
    *p_priv_bit_len = lsf ? ((nch == 1) ? 1 : 2) : ((nch == 1) ? 5 : 3);

    p_si->main_data_begin = aw_mp3_dec_bit_read(p_ptr, lsf ? 8 : 9);
    p_si->private_bits    = aw_mp3_dec_bit_read(p_ptr, *p_priv_bit_len);

    ngr = 1;
    if (!lsf) {
        ngr = 2;

        for (ch = 0; ch < nch; ++ch) {
            p_si->scfsi[ch] = aw_mp3_dec_bit_read(p_ptr, 4);
        }
    }

    for (gr = 0; gr < ngr; ++gr) {

        struct __layer_iii_granule *p_granule = &p_si->gr[gr];

        for (ch = 0; ch < nch; ++ch) {

            struct __layer_iii_channel *p_channel = &p_granule->ch[ch];

            p_channel->part2_3_length    = aw_mp3_dec_bit_read(p_ptr, 12);
            p_channel->big_values        = aw_mp3_dec_bit_read(p_ptr, 9);
            p_channel->global_gain       = aw_mp3_dec_bit_read(p_ptr, 8);
            p_channel->scalefac_compress = aw_mp3_dec_bit_read(p_ptr,
                                                               lsf ? 9 : 4);

            *p_data_bit_len += p_channel->part2_3_length;

            if (p_channel->big_values > 288 && result == 0) {
                result = AW_MP3_DEC_ERR_BADBIGVALUES;
            }

            p_channel->flags = 0;

            /* window_switching_flag */
            if (aw_mp3_dec_bit_read(p_ptr, 1)) {
                p_channel->block_type = aw_mp3_dec_bit_read(p_ptr, 2);

                if (p_channel->block_type == 0 && result == 0) {
                    result = AW_MP3_DEC_ERR_BADBLOCKTYPE;
                }

                if (!lsf                       &&
                    p_channel->block_type == 2 &&
                    p_si->scfsi[ch]            &&
                    result == 0) {
                    result = AW_MP3_DEC_ERR_BADSCFSI;
                }

                p_channel->region0_count = 7;
                p_channel->region1_count = 36;

                if (aw_mp3_dec_bit_read(p_ptr, 1)) {
                    p_channel->flags |= __LAYER_III_FLG_MIXED_BLOCK;
                } else if (p_channel->block_type == 2) {
                    p_channel->region0_count = 8;
                }

                for (i = 0; i < 2; ++i) {
                    p_channel->table_select[i] = aw_mp3_dec_bit_read(p_ptr, 5);
                }

#if (AW_MP3_DEC_DEBUG)
                p_channel->table_select[2] = 4;  /* not used */
#endif

                for (i = 0; i < 3; ++i) {
                    p_channel->subblock_gain[i] = aw_mp3_dec_bit_read(p_ptr, 3);
                }
            } else {
                p_channel->block_type = 0;

                for (i = 0; i < 3; ++i) {
                    p_channel->table_select[i] = aw_mp3_dec_bit_read(p_ptr, 5);
                }

                p_channel->region0_count = aw_mp3_dec_bit_read(p_ptr, 4);
                p_channel->region1_count = aw_mp3_dec_bit_read(p_ptr, 3);
            }

            /* [preflag,] scalefac_scale, count1table_select */
            p_channel->flags |= aw_mp3_dec_bit_read(p_ptr, lsf ? 2 : 3);
        }
    }

    return result;
}

/**
 * \brief decode channel scalefactors for LSF from a bitstream
 */
static uint32_t __layer_iii_scale_factors_lsf (
    struct aw_mp3_dec_bit      *p_ptr,
    struct __layer_iii_channel *p_channel,
    struct __layer_iii_channel *p_gr1ch,
    int                         mode_extension)
{
    struct aw_mp3_dec_bit  start;
    uint32_t               scalefac_compress;
    uint32_t               index;
    uint32_t               slen[4];
    uint32_t               part;
    uint32_t               n;
    uint32_t               i;
    const uint8_t         *p_nsfb;

    start = *p_ptr;

    scalefac_compress = p_channel->scalefac_compress;
    index             = (p_channel->block_type == 2) ?
                        ((p_channel->flags &
                          __LAYER_III_FLG_MIXED_BLOCK) ? 2 : 1) : 0;

    if (!((mode_extension & __LAYER_III_STEREO_I) && p_gr1ch != NULL)) {
        if (scalefac_compress < 400) {
            slen[0] = (scalefac_compress >> 4) / 5;
            slen[1] = (scalefac_compress >> 4) % 5;
            slen[2] = (scalefac_compress % 16) >> 2;
            slen[3] =  scalefac_compress %  4;

            p_nsfb  = __layer_iii_nsfb_table[0][index];
        } else if (scalefac_compress < 500) {
            scalefac_compress -= 400;

            slen[0] = (scalefac_compress >> 2) / 5;
            slen[1] = (scalefac_compress >> 2) % 5;
            slen[2] =  scalefac_compress %  4;
            slen[3] = 0;

            p_nsfb = __layer_iii_nsfb_table[1][index];
        } else {
            scalefac_compress -= 500;

            slen[0] = scalefac_compress / 3;
            slen[1] = scalefac_compress % 3;
            slen[2] = 0;
            slen[3] = 0;

            p_channel->flags |= __LAYER_III_FLG_PRE;
            p_nsfb            = __layer_iii_nsfb_table[2][index];
        }

        n = 0;
        for (part = 0; part < 4; ++part) {
            for (i = 0; i < p_nsfb[part]; ++i) {
                p_channel->scalefac[n++] = aw_mp3_dec_bit_read(p_ptr,
                                                               slen[part]);
            }
        }

        while (n < 39) {
            p_channel->scalefac[n++] = 0;
        }
    } else {  /* (mode_extension & I_STEREO) && gr1ch (i.e. ch == 1) */

        scalefac_compress >>= 1;

        if (scalefac_compress < 180) {
            slen[0] =  scalefac_compress / 36;
            slen[1] = (scalefac_compress % 36) / 6;
            slen[2] = (scalefac_compress % 36) % 6;
            slen[3] = 0;

            p_nsfb = __layer_iii_nsfb_table[3][index];
        }
        else if (scalefac_compress < 244) {
            scalefac_compress -= 180;

            slen[0] = (scalefac_compress % 64) >> 4;
            slen[1] = (scalefac_compress % 16) >> 2;
            slen[2] =  scalefac_compress %  4;
            slen[3] = 0;

            p_nsfb = __layer_iii_nsfb_table[4][index];
        }
        else {
            scalefac_compress -= 244;

            slen[0] = scalefac_compress / 3;
            slen[1] = scalefac_compress % 3;
            slen[2] = 0;
            slen[3] = 0;

            p_nsfb = __layer_iii_nsfb_table[5][index];
        }

        n = 0;
        for (part = 0; part < 4; ++part) {
            uint32_t max, is_pos;

            max = (1 << slen[part]) - 1;

            for (i = 0; i < p_nsfb[part]; ++i) {
                is_pos = aw_mp3_dec_bit_read(p_ptr, slen[part]);

                p_channel->scalefac[n] = is_pos;
                p_gr1ch->scalefac[n++] = (is_pos == max);
            }
        }

        while (n < 39) {
            p_channel->scalefac[n] = 0;
            p_gr1ch->scalefac[n++] = 0;  /* apparently not illegal */
        }
    }

    return aw_mp3_dec_bit_len_get(&start, p_ptr);
}

/**
 * \brief decode channel scalefactors of one granule from a bitstream
 */
static uint32_t __layer_iii_scale_factors (
    struct aw_mp3_dec_bit            *p_ptr,
    struct __layer_iii_channel       *p_channel,
    const struct __layer_iii_channel *p_gr0ch,
    uint32_t                          scfsi)
{
    struct aw_mp3_dec_bit start;
    uint32_t              slen1;
    uint32_t              slen2;
    uint32_t              sfbi;

    start = *p_ptr;

    slen1 = __g_layer_iii_sflen_table[p_channel->scalefac_compress].slen1;
    slen2 = __g_layer_iii_sflen_table[p_channel->scalefac_compress].slen2;

    if (p_channel->block_type == 2) {
        uint32_t nsfb;

        sfbi = 0;
        nsfb = (p_channel->flags &
                __LAYER_III_FLG_MIXED_BLOCK) ? 8 + 3 * 3 : 6 * 3;
        while (nsfb--) {
            p_channel->scalefac[sfbi++] = aw_mp3_dec_bit_read(p_ptr, slen1);
        }

        nsfb = 6 * 3;
        while (nsfb--) {
            p_channel->scalefac[sfbi++] = aw_mp3_dec_bit_read(p_ptr, slen2);
        }

        nsfb = 1 * 3;
        while (nsfb--) {
            p_channel->scalefac[sfbi++] = 0;
        }
    } else {  /* channel->block_type != 2 */
        if (scfsi & 0x8) {
            for (sfbi = 0; sfbi < 6; ++sfbi) {
                p_channel->scalefac[sfbi] = p_gr0ch->scalefac[sfbi];
            }
        } else {
            for (sfbi = 0; sfbi < 6; ++sfbi) {
                p_channel->scalefac[sfbi] = aw_mp3_dec_bit_read(p_ptr, slen1);
            }
        }

        if (scfsi & 0x4) {
            for (sfbi = 6; sfbi < 11; ++sfbi) {
                p_channel->scalefac[sfbi] = p_gr0ch->scalefac[sfbi];
            }
        } else {
            for (sfbi = 6; sfbi < 11; ++sfbi) {
                p_channel->scalefac[sfbi] = aw_mp3_dec_bit_read(p_ptr, slen1);
            }
        }

        if (scfsi & 0x2) {
            for (sfbi = 11; sfbi < 16; ++sfbi) {
                p_channel->scalefac[sfbi] = p_gr0ch->scalefac[sfbi];
            }
        } else {
            for (sfbi = 11; sfbi < 16; ++sfbi) {
                p_channel->scalefac[sfbi] = aw_mp3_dec_bit_read(p_ptr, slen2);
            }
        }

        if (scfsi & 0x1) {
            for (sfbi = 16; sfbi < 21; ++sfbi) {
                p_channel->scalefac[sfbi] = p_gr0ch->scalefac[sfbi];
            }
        } else {
            for (sfbi = 16; sfbi < 21; ++sfbi) {
                p_channel->scalefac[sfbi] = aw_mp3_dec_bit_read(p_ptr, slen2);
            }
        }

        p_channel->scalefac[21] = 0;
    }

    return aw_mp3_dec_bit_len_get(&start, p_ptr);
}

/**
 * \brief calculate scalefactor exponents
 *
 * The Layer III formula for requantization and scaling is defined by
 * section 2.4.3.4.7.1 of ISO/IEC 11172-3, as follows:
 *
 *   long blocks:
 *   xr[i] = sign(is[i]) * abs(is[i])^(4/3) *
 *           2^((1/4) * (global_gain - 210)) *
 *           2^-(scalefac_multiplier *
 *               (scalefac_l[sfb] + preflag * pretab[sfb]))
 *
 *   short blocks:
 *   xr[i] = sign(is[i]) * abs(is[i])^(4/3) *
 *           2^((1/4) * (global_gain - 210 - 8 * subblock_gain[w])) *
 *           2^-(scalefac_multiplier * scalefac_s[sfb][w])
 *
 *   where:
 *   scalefac_multiplier = (scalefac_scale + 1) / 2
 *
 * The routines III_exponents() and III_requantize() facilitate this
 * calculation.
 */
static
void __layer_iii_exponents (const struct __layer_iii_channel *p_channel,
                            const uint8_t                    *p_sfb_width,
                            int                               exponents[39])
{
    int      gain;
    uint32_t scalefac_multiplier;
    uint32_t sfbi;

    gain                = (int)p_channel->global_gain - 210;
    scalefac_multiplier = (p_channel->flags &
                           __LAYER_III_FLG_SCALEFAC_SCALE) ? 2 : 1;

    if (p_channel->block_type == 2) {
        uint32_t l;
        int      gain0;
        int      gain1;
        int      gain2;

        sfbi = l = 0;

        if (p_channel->flags & __LAYER_III_FLG_MIXED_BLOCK) {
            uint32_t premask;

            premask = (p_channel->flags & __LAYER_III_FLG_PRE) ? ~0 : 0;

            /* long block subbands 0-1 */
            while (l < 36) {
                exponents[sfbi] =
                    gain - (int)((p_channel->scalefac[sfbi] +
                                 (__g_layer_iii_pretab[sfbi] & premask)) <<
                                 scalefac_multiplier);

                l += p_sfb_width[sfbi++];
            }
        }

        /* this is probably wrong for 8000 Hz short/mixed blocks */
        gain0 = gain - 8 * (int)p_channel->subblock_gain[0];
        gain1 = gain - 8 * (int)p_channel->subblock_gain[1];
        gain2 = gain - 8 * (int)p_channel->subblock_gain[2];

        while (l < 576) {
            exponents[sfbi + 0] = gain0 - (int)(p_channel->scalefac[sfbi + 0] <<
                                                scalefac_multiplier);
            exponents[sfbi + 1] = gain1 - (int)(p_channel->scalefac[sfbi + 1] <<
                                                scalefac_multiplier);
            exponents[sfbi + 2] = gain2 - (int)(p_channel->scalefac[sfbi + 2] <<
                                                scalefac_multiplier);

            l    += 3 * p_sfb_width[sfbi];
            sfbi += 3;
        }
    } else {  /* channel->block_type != 2 */
        if (p_channel->flags & __LAYER_III_FLG_PRE) {
            for (sfbi = 0; sfbi < 22; ++sfbi) {
                exponents[sfbi] =
                    gain - (int)((p_channel->scalefac[sfbi] +
                                 __g_layer_iii_pretab[sfbi]) <<
                                scalefac_multiplier);
            }
        }
        else {
            for (sfbi = 0; sfbi < 22; ++sfbi) {
                exponents[sfbi] =
                    gain - (int)(p_channel->scalefac[sfbi] <<
                                 scalefac_multiplier);
            }
        }
    }
}

/**
 * \brief requantize one (positive) value
 */
static int32_t __layer_iii_requantize (uint32_t value, int exp)
{
    int32_t                              requantized;
    int                                  frac;
    const struct aw_mp3_dec_fixed_float *power;

    frac         = exp % 4;  /* assumes sign(frac) == sign(exp) */
    exp         /= 4;

    power        = &g_mp3_dec_rq_table[value];
    requantized  = power->mantissa;
    exp         += power->exponent;

    if (exp < 0) {
        if (-exp >= sizeof(int32_t) * AW_MP3_DEC_CHAR_BIT) {
            /* underflow */
            requantized = 0;
        } else {
            requantized += 1L << (-exp - 1);
            requantized >>= -exp;
        }
    } else {
        if (exp >= 5) {

            /* overflow */
            AW_ERRF(("requantize overflow (%f * 2^%d)\n",
                    AW_MP3_F_TO_DOUBLE(requantized), exp));
            requantized = AW_MP3_F_MAX;
        } else {
            requantized <<= exp;
        }
    }

    return frac ? AW_MP3_F_MUL(requantized,
                               g_mp3_dec_root_table[3 + frac]) :
                  requantized;
}

/**
 * \brief decode Huffman code words of one channel of one granule
 */
static enum aw_mp3_dec_err
__layer_iii_huffman_decode (struct aw_mp3_dec_bit      *p_ptr,
                            int32_t                     xr[576],
                            struct __layer_iii_channel *p_channel,
                            const uint8_t              *p_sfb_width,
                            uint32_t                    part2_length)
{
    int                    exponents[39];
    int                    exp;
    const int             *p_expptr;
    struct aw_mp3_dec_bit  peek;
    int                    bits_left;
    int                    cachesz;
    register int32_t      *p_xrptr;
    const int32_t         *p_sf_bound;
    register uint32_t      bit_cache;

    bits_left = (signed)p_channel->part2_3_length - (signed)part2_length;
    if (bits_left < 0) {
        return AW_MP3_DEC_ERR_BADPART3LEN;
    }

    __layer_iii_exponents(p_channel, p_sfb_width, exponents);

    peek = *p_ptr;

    aw_mp3_dec_bit_skip(p_ptr, bits_left);

    /* align bit reads to byte boundaries */
    cachesz    = aw_mp3_dec_bit_left_get(&peek);
    cachesz   += ((32 - 1 - 24) + (24 - cachesz)) & ~7;

    bit_cache  = aw_mp3_dec_bit_read(&peek, cachesz);
    bits_left -= cachesz;

    p_xrptr    = &xr[0];

    /* big_values */
    {
        uint32_t                           region;
        uint32_t                           rcount;
        const struct aw_mp3_huffman_table *p_entry;
        const union aw_mp3_huffman_pair   *p_table;
        uint32_t                           line_bits;
        uint32_t                           start_bits;
        uint32_t                           big_values;
        uint32_t                           reqhits;
        int32_t                            req_cache[16];

        p_sf_bound  = p_xrptr + *p_sfb_width++;
        rcount      = p_channel->region0_count + 1;

        p_entry     = &g_mp3_huffman_pair_table[
                          p_channel->table_select[region = 0]];
        p_table     = p_entry->p_table;
        line_bits   = p_entry->line_bits;
        start_bits  = p_entry->start_bits;

        if (p_table == NULL) {
            return AW_MP3_DEC_ERR_BADHUFFTABLE;
        }

        p_expptr = &exponents[0];
        exp      = *p_expptr++;
        reqhits  = 0;

        big_values = p_channel->big_values;

        while (big_values-- && cachesz + bits_left > 0) {
            const union aw_mp3_huffman_pair *p_pair;
            uint32_t                         clumpsz, value;
            register int32_t                 requantized;

            if (p_xrptr == p_sf_bound) {
                p_sf_bound += *p_sfb_width++;

                /* change table if region boundary */
                if (--rcount == 0) {
                    if (region == 0) {
                        rcount = p_channel->region1_count + 1;
                    } else {
                        rcount = 0;  /* all remaining */
                    }

                    p_entry    = &g_mp3_huffman_pair_table[
                                     p_channel->table_select[++region]];
                    p_table    = p_entry->p_table;
                    line_bits  = p_entry->line_bits;
                    start_bits = p_entry->start_bits;

                    if (p_table == NULL) {
                        return AW_MP3_DEC_ERR_BADHUFFTABLE;
                    }
                }

                if (exp != *p_expptr) {
                    exp     = *p_expptr;
                    reqhits = 0;
                }

                ++p_expptr;
            }

            if (cachesz < 21) {
                uint32_t bits;

                bits       = ((32 - 1 - 21) + (21 - cachesz)) & ~7;
                bit_cache  = (bit_cache << bits) |
                             aw_mp3_dec_bit_read(&peek, bits);
                cachesz   += bits;
                bits_left -= bits;
            }

            /* hcod (0..19) */
            clumpsz = start_bits;
            p_pair  = &p_table[__LAYER_MASK(bit_cache, cachesz, clumpsz)];

            while (!p_pair->final) {
                cachesz -= clumpsz;

                clumpsz  = p_pair->ptr.bits;
                p_pair   = &p_table[p_pair->ptr.offset +
                                    __LAYER_MASK(bit_cache, cachesz, clumpsz)];
            }

            cachesz -= p_pair->value.hlen;

            if (line_bits) {

                /* x (0..14) */
                value = p_pair->value.x;

                switch (value) {

                case 0:
                    p_xrptr[0] = 0;
                    break;

                case 15:
                    if (cachesz < line_bits + 2) {
                        bit_cache  = (bit_cache << 16) |
                                     aw_mp3_dec_bit_read(&peek, 16);
                        cachesz   += 16;
                        bits_left -= 16;
                    }

                    value   += __LAYER_MASK(bit_cache, cachesz, line_bits);
                    cachesz -= line_bits;

                    requantized = __layer_iii_requantize(value, exp);
                    goto __x_final;

                default:
                    if (reqhits & (1 << value)) {
                        requantized = req_cache[value];
                    } else {
                        reqhits     |= (1 << value);
                        requantized  = req_cache[value] =
                            __layer_iii_requantize(value, exp);
                    }

__x_final:
                    p_xrptr[0] = __LAYER_MASK_BIT(bit_cache, cachesz--) ?
                                 -requantized : requantized;
                }

                /* y (0..14) */
                value = p_pair->value.y;

                switch (value) {

                case 0:
                    p_xrptr[1] = 0;
                    break;

                case 15:
                    if (cachesz < line_bits + 1) {
                        bit_cache  = (bit_cache << 16) |
                                     aw_mp3_dec_bit_read(&peek, 16);
                        cachesz   += 16;
                        bits_left -= 16;
                    }

                    value   += __LAYER_MASK(bit_cache, cachesz, line_bits);
                    cachesz -= line_bits;

                    requantized = __layer_iii_requantize(value, exp);
                    goto __y_final;

                default:
                    if (reqhits & (1 << value)) {
                        requantized = req_cache[value];
                    } else {
                        reqhits     |= (1 << value);
                        requantized  = req_cache[value] =
                            __layer_iii_requantize(value, exp);
                    }

__y_final:
                    p_xrptr[1] = __LAYER_MASK_BIT(bit_cache, cachesz--) ?
                                 -requantized : requantized;
                }
            } else {

                /* x (0..1) */
                value = p_pair->value.x;

                if (value == 0) {
                    p_xrptr[0] = 0;
                } else {
                    if (reqhits & (1 << value)) {
                        requantized = req_cache[value];
                    } else {
                        reqhits     |= (1 << value);
                        requantized  = req_cache[value] =
                            __layer_iii_requantize(value, exp);
                    }

                    p_xrptr[0] = __LAYER_MASK_BIT(bit_cache, cachesz--) ?
                                 -requantized : requantized;
                }

                /* y (0..1) */
                value = p_pair->value.y;

                if (value == 0) {
                    p_xrptr[1] = 0;
                } else {
                    if (reqhits & (1 << value)) {
                        requantized = req_cache[value];
                    } else {
                        reqhits     |= (1 << value);
                        requantized  = req_cache[value] =
                            __layer_iii_requantize(value, exp);
                    }

                    p_xrptr[1] = __LAYER_MASK_BIT(bit_cache, cachesz--) ?
                                 -requantized : requantized;
                }
            }

            p_xrptr += 2;
        }
    }

    if (cachesz + bits_left < 0) {
        return AW_MP3_DEC_ERR_BADHUFFDATA;  /* big_values overrun */
    }

    /* count1 */
    {
        const union aw_mp3_huffman_quad *p_table;
        register int32_t                 requantized;

        p_table = g_mp3_huffman_quad_table[
                      p_channel->flags & __LAYER_III_FLG_COUNT1_TABLE_SELECT];

        requantized = __layer_iii_requantize(1, exp);

        while (cachesz + bits_left > 0 && p_xrptr <= &xr[572]) {
            const union aw_mp3_huffman_quad *p_quad;

            /* hcod (1..6) */
            if (cachesz < 10) {
                bit_cache  = (bit_cache << 16) | aw_mp3_dec_bit_read(&peek, 16);
                cachesz   += 16;
                bits_left -= 16;
            }

            p_quad = &p_table[__LAYER_MASK(bit_cache, cachesz, 4)];

            /* quad tables guaranteed to have at most one extra lookup */
            if (!p_quad->final) {
                cachesz -= 4;
                p_quad   = &p_table[p_quad->ptr.offset +
                                    __LAYER_MASK(bit_cache,
                                                 cachesz,
                                                 p_quad->ptr.bits)];
            }

            cachesz -= p_quad->value.hlen;

            if (p_xrptr == p_sf_bound) {
                p_sf_bound += *p_sfb_width++;

                if (exp != *p_expptr) {
                    exp         = *p_expptr;
                    requantized = __layer_iii_requantize(1, exp);
                }

                ++p_expptr;
            }

            /* v (0..1) */
            p_xrptr[0] = p_quad->value.v ?
                         (__LAYER_MASK_BIT(bit_cache, cachesz--) ?
                         -requantized : requantized) : 0;

            /* w (0..1) */
            p_xrptr[1] = p_quad->value.w ?
                         (__LAYER_MASK_BIT(bit_cache, cachesz--) ?
                         -requantized : requantized) : 0;

            p_xrptr += 2;

            if (p_xrptr == p_sf_bound) {
                p_sf_bound += *p_sfb_width++;

                if (exp != *p_expptr) {
                    exp = *p_expptr;
                    requantized = __layer_iii_requantize(1, exp);
                }

                ++p_expptr;
            }

            /* x (0..1) */
            p_xrptr[0] = p_quad->value.x ?
                         (__LAYER_MASK_BIT(bit_cache, cachesz--) ?
                         -requantized : requantized) : 0;

            /* y (0..1) */
            p_xrptr[1] = p_quad->value.y ?
                         (__LAYER_MASK_BIT(bit_cache, cachesz--) ?
                         -requantized : requantized) : 0;

            p_xrptr += 2;
        }

        if (cachesz + bits_left < 0) {

            AW_ERRF((
                "huffman count1 overrun (%d bits)\n", -(cachesz + bits_left)));

            /*
             * technically the bitstream is misformatted, but apparently
             * some encoders are just a bit sloppy with stuffing bits
             */
            p_xrptr -= 4;
        }
    }

    aw_assert(-bits_left <= AW_MP3_DEC_BUFFER_GUARD * AW_MP3_DEC_CHAR_BIT);

    if (bits_left < 0) {
        AW_DBGF(("read %d bits too many\n", -bits_left));
    } else if (cachesz + bits_left > 0) {
        AW_DBGF(("%d stuffing bits\n", cachesz + bits_left));
    }

    /* rzero */
    while (p_xrptr < &xr[576]) {
        p_xrptr[0]  = 0;
        p_xrptr[1]  = 0;
        p_xrptr    += 2;
    }

    return AW_MP3_DEC_ERR_NONE;
}

#undef __LAYER_MASK
#undef __LAYER_MASK_BIT

/**
 * \brief reorder frequency lines of a short block into subband order
 * \note p_xr[576], p_sb_width[39]
 */
static void __layer_iii_reorder (
    int32_t                          *p_xr,
    const struct __layer_iii_channel *p_channel,
    const uint8_t                    *p_sfb_width)
{
    int32_t  tmp[32][3][6];
    uint32_t sb;
    uint32_t l;
    uint32_t f;
    uint32_t w;
    uint32_t sbw[3];
    uint32_t sw[3];

    /* this is probably wrong for 8000 Hz mixed blocks */
    sb = 0;
    if (p_channel->flags & __LAYER_III_FLG_MIXED_BLOCK) {
        sb = 2;

        l = 0;
        while (l < 36) {
            l += *p_sfb_width++;
        }
    }

    for (w = 0; w < 3; ++w) {
        sbw[w] = sb;
        sw[w]  = 0;
    }

    f = *p_sfb_width++;
    w = 0;

    for (l = 18 * sb; l < 576; ++l) {
        if (f-- == 0) {
            f = *p_sfb_width++ - 1;
            w = (w + 1) % 3;
        }

        tmp[sbw[w]][w][sw[w]++] = p_xr[l];

        if (sw[w] == 6) {
            sw[w] = 0;
            ++sbw[w];
        }
    }

    memcpy(&p_xr[18 * sb],
           &tmp[sb],
           (576 - 18 * sb) * sizeof(int32_t));
}

/**
 * \brief perform joint stereo processing on a granule
 */
static enum aw_mp3_dec_err __layer_iii_stereo (
    int32_t                           xr[2][576],
    const struct __layer_iii_granule *p_granule,
    struct aw_mp3_dec_frame_header   *p_header,
    const uint8_t                    *p_sfb_width)
{
    short    modes[39];
    uint32_t sfbi;
    uint32_t l;
    uint32_t n;
    uint32_t i;

    if (p_granule->ch[0].block_type !=
        p_granule->ch[1].block_type ||
        (p_granule->ch[0].flags & __LAYER_III_FLG_MIXED_BLOCK) !=
        (p_granule->ch[1].flags & __LAYER_III_FLG_MIXED_BLOCK)) {
        return AW_MP3_DEC_ERR_BADSTEREO;
    }

    for (i = 0; i < 39; ++i) {
        modes[i] = p_header->mode_extension;
    }

    /* intensity stereo */
    if (p_header->mode_extension & __LAYER_III_STEREO_I) {
        const struct __layer_iii_channel *p_right_ch = &p_granule->ch[1];
        const int32_t                    *p_right_xr = xr[1];
        uint32_t                          is_pos;

        p_header->flags |= AW_MP3_DEC_FRAME_FLAG_I_STEREO;

        /* first determine which scalefactor bands are to be processed */
        if (p_right_ch->block_type == 2) {
            uint32_t lower;
            uint32_t start;
            uint32_t max;
            uint32_t bound[3];
            uint32_t w;

            lower = start = max = bound[0] = bound[1] = bound[2] = 0;

            sfbi = l = 0;

            if (p_right_ch->flags & __LAYER_III_FLG_MIXED_BLOCK) {
                while (l < 36) {
                    n = p_sfb_width[sfbi++];

                    for (i = 0; i < n; ++i) {
                        if (p_right_xr[i]) {
                            lower = sfbi;
                            break;
                        }
                    }

                    p_right_xr += n;
                    l += n;
                }

                start = sfbi;
            }

            w = 0;
            while (l < 576) {
                n = p_sfb_width[sfbi++];

                for (i = 0; i < n; ++i) {
                    if (p_right_xr[i]) {
                        max = bound[w] = sfbi;
                        break;
                    }
                }

                p_right_xr += n;
                l          += n;
                w          = (w + 1) % 3;
            }

            if (max) {
                lower = start;
            }

            /* long blocks */
            for (i = 0; i < lower; ++i) {
                modes[i] = p_header->mode_extension & ~__LAYER_III_STEREO_I;
            }

            /* short blocks */
            w = 0;
            for (i = start; i < max; ++i) {
                if (i < bound[w]) {
                    modes[i] = p_header->mode_extension & ~__LAYER_III_STEREO_I;
                }

                w = (w + 1) % 3;
            }
        } else {  /* right_ch->block_type != 2 */
            uint32_t bound;

            bound = 0;
            for (sfbi = l = 0; l < 576; l += n) {
                n = p_sfb_width[sfbi++];

                for (i = 0; i < n; ++i) {
                    if (p_right_xr[i]) {
                        bound = sfbi;
                        break;
                    }
                }

                p_right_xr += n;
            }

            for (i = 0; i < bound; ++i) {
                modes[i] = p_header->mode_extension & ~__LAYER_III_STEREO_I;
            }
        }

        /* now do the actual processing */
        if (p_header->flags & AW_MP3_DEC_FRAME_FLAG_LSF_EXT) {

            const uint8_t *p_illegal_pos = p_granule[1].ch[1].scalefac;
            const int32_t *p_lsf_scale;

            /* intensity_scale */
            p_lsf_scale = g_mp3_dec_is_lsf_table[
                              p_right_ch->scalefac_compress & 0x1];

            for (sfbi = l = 0; l < 576; ++sfbi, l += n) {
                n = p_sfb_width[sfbi];

                if (!(modes[sfbi] & __LAYER_III_STEREO_I)) {
                    continue;
                }

                if (p_illegal_pos[sfbi]) {
                    modes[sfbi] &= ~__LAYER_III_STEREO_I;
                    continue;
                }

                is_pos = p_right_ch->scalefac[sfbi];

                for (i = 0; i < n; ++i) {
                    register int32_t left;

                    left = xr[0][l + i];

                    if (is_pos == 0) {
                        xr[1][l + i] = left;
                    } else {
                        register int32_t opposite;

                        opposite = AW_MP3_F_MUL(left,
                                                p_lsf_scale[(is_pos - 1) / 2]);

                        if (is_pos & 1) {
                            xr[0][l + i] = opposite;
                            xr[1][l + i] = left;
                        } else {
                            xr[1][l + i] = opposite;
                        }
                    }
                }
            }
        } else {  /* !(header->flags & AW_MP3LAG_LSF_EXT) */
            for (sfbi = l = 0; l < 576; ++sfbi, l += n) {
                n = p_sfb_width[sfbi];

                if (!(modes[sfbi] & __LAYER_III_STEREO_I)) {
                    continue;
                }

                is_pos = p_right_ch->scalefac[sfbi];

                if (is_pos >= 7) {  /* illegal intensity position */
                    modes[sfbi] &= ~__LAYER_III_STEREO_I;
                    continue;
                }

                for (i = 0; i < n; ++i) {
                    register int32_t left;

                    left         = xr[0][l + i];
                    xr[0][l + i] = AW_MP3_F_MUL(left,
                                                g_mp3_dec_is_table[is_pos]);
                    xr[1][l + i] = AW_MP3_F_MUL(left,
                                                g_mp3_dec_is_table[6 - is_pos]);
                }
            }
        }
    }

    /* middle/side stereo */
    if (p_header->mode_extension & __LAYER_III_STEREO_MS) {
        register int32_t invsqrt2;

        p_header->flags |= AW_MP3_DEC_FRAME_FLAG_MS_STEREO;

        invsqrt2 = g_mp3_dec_root_table[3 + -2];

        for (sfbi = l = 0; l < 576; ++sfbi, l += n) {
            n = p_sfb_width[sfbi];

            if (modes[sfbi] != __LAYER_III_STEREO_MS) {
                continue;
            }

            for (i = 0; i < n; ++i) {
                register int32_t m, s;

                m            = xr[0][l + i];
                s            = xr[1][l + i];

                /* l = (m + s) / sqrt(2) */
                xr[0][l + i] = AW_MP3_F_MUL(m + s, invsqrt2);

                /* r = (m - s) / sqrt(2) */
                xr[1][l + i] = AW_MP3_F_MUL(m - s, invsqrt2);
            }
        }
    }

    return AW_MP3_DEC_ERR_NONE;
}

/**
 * \brief perform frequency line alias reduction
 */
static void __layer_iii_aliasreduce (int32_t xr[576], int lines)
{
    const int32_t *p_bound;
    int            i;

    p_bound = &xr[lines];
    for (xr += 18; xr < p_bound; xr += 18) {
        for (i = 0; i < 8; ++i) {
            register int32_t a, b;
            register int64_t hi;
            register int64_t lo;

            a = xr[-1 - i];
            b = xr[i];

#if (AW_MP3_DEC_ASO_ZEROCHECK)
            if (a | b) {
# endif
                AW_MP3_F_ML0(hi, lo,  a, g_mp3_dec_cs[i]);
                AW_MP3_F_MLA(hi, lo, -b, g_mp3_dec_ca[i]);

                xr[-1 - i] = AW_MP3_F_MLZ(hi, lo);

                AW_MP3_F_ML0(hi, lo,  b, g_mp3_dec_cs[i]);
                AW_MP3_F_MLA(hi, lo,  a, g_mp3_dec_ca[i]);

                xr[     i] = AW_MP3_F_MLZ(hi, lo);
# if (AW_MP3_DEC_ASO_ZEROCHECK)
            }
# endif
        }
    }
}

#if !(AW_MP3_DEC_ASO_IMDCT)
#  if 1
static void __layer_iii_fast_sdct (const int32_t x[9], int32_t y[18])
{
    int32_t a0,  a1,  a2,  a3,  a4,  a5,  a6,  a7,  a8,  a9,  a10, a11, a12;
    int32_t a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25;
    int32_t m0,  m1,  m2,  m3,  m4,  m5,  m6,  m7;

    enum __cx {
        c0 =  AW_MP3_F(0x1f838b8d),  /* 2 * cos( 1 * PI / 18) */
        c1 =  AW_MP3_F(0x1bb67ae8),  /* 2 * cos( 3 * PI / 18) */
        c2 =  AW_MP3_F(0x18836fa3),  /* 2 * cos( 4 * PI / 18) */
        c3 =  AW_MP3_F(0x1491b752),  /* 2 * cos( 5 * PI / 18) */
        c4 =  AW_MP3_F(0x0af1d43a),  /* 2 * cos( 7 * PI / 18) */
        c5 =  AW_MP3_F(0x058e86a0),  /* 2 * cos( 8 * PI / 18) */
        c6 = -AW_MP3_F(0x1e11f642)   /* 2 * cos(16 * PI / 18) */
    };

    a0 = x[3] + x[5];
    a1 = x[3] - x[5];
    a2 = x[6] + x[2];
    a3 = x[6] - x[2];
    a4 = x[1] + x[7];
    a5 = x[1] - x[7];
    a6 = x[8] + x[0];
    a7 = x[8] - x[0];

    a8  = a0  + a2;
    a9  = a0  - a2;
    a10 = a0  - a6;
    a11 = a2  - a6;
    a12 = a8  + a6;
    a13 = a1  - a3;
    a14 = a13 + a7;
    a15 = a3  + a7;
    a16 = a1  - a7;
    a17 = a1  + a3;

    m0 = AW_MP3_F_MUL(a17, -c3);
    m1 = AW_MP3_F_MUL(a16, -c0);
    m2 = AW_MP3_F_MUL(a15, -c4);
    m3 = AW_MP3_F_MUL(a14, -c1);
    m4 = AW_MP3_F_MUL(a5,  -c1);
    m5 = AW_MP3_F_MUL(a11, -c6);
    m6 = AW_MP3_F_MUL(a10, -c5);
    m7 = AW_MP3_F_MUL(a9,  -c2);

    a18 =     x[4] + a4;
    a19 = 2 * x[4] - a4;
    a20 = a19 + m5;
    a21 = a19 - m5;
    a22 = a19 + m6;
    a23 = m4  + m2;
    a24 = m4  - m2;
    a25 = m4  + m1;

    /* output to every other slot for convenience */
    y[ 0] = a18 + a12;
    y[ 2] = m0  - a25;
    y[ 4] = m7  - a20;
    y[ 6] = m3;
    y[ 8] = a21 - m6;
    y[10] = a24 - m1;
    y[12] = a12 - 2 * a18;
    y[14] = a23 + m0;
    y[16] = a22 + m7;
}

static
void __layer_iii_sdct_ii (const int32_t x[18], int32_t X[18])
{
    int32_t tmp[9];
    int     i;

  /* scale[i] = 2 * cos(PI * (2 * i + 1) / (2 * 18)) */
    static const int32_t scale[9] = {
        AW_MP3_F(0x1fe0d3b4),
        AW_MP3_F(0x1ee8dd47),
        AW_MP3_F(0x1d007930),
        AW_MP3_F(0x1a367e59),
        AW_MP3_F(0x16a09e66),
        AW_MP3_F(0x125abcf8),
        AW_MP3_F(0x0d8616bc),
        AW_MP3_F(0x08483ee1),
        AW_MP3_F(0x02c9fad7)
    };

    /* divide the 18-point SDCT-II into two 9-point SDCT-IIs */

    /* even input butterfly */
    for (i = 0; i < 9; i += 3) {
        tmp[i + 0] = x[i + 0] + x[18 - (i + 0) - 1];
        tmp[i + 1] = x[i + 1] + x[18 - (i + 1) - 1];
        tmp[i + 2] = x[i + 2] + x[18 - (i + 2) - 1];
    }

    __layer_iii_fast_sdct(tmp, &X[0]);

    /* odd input butterfly and scaling */
    for (i = 0; i < 9; i += 3) {
        tmp[i + 0] = AW_MP3_F_MUL(x[i + 0] - x[18 - (i + 0) - 1], scale[i + 0]);
        tmp[i + 1] = AW_MP3_F_MUL(x[i + 1] - x[18 - (i + 1) - 1], scale[i + 1]);
        tmp[i + 2] = AW_MP3_F_MUL(x[i + 2] - x[18 - (i + 2) - 1], scale[i + 2]);
    }

    __layer_iii_fast_sdct(tmp, &X[1]);

    /* output accumulation */
    for (i = 3; i < 18; i += 8) {
        X[i + 0] -= X[(i + 0) - 2];
        X[i + 2] -= X[(i + 2) - 2];
        X[i + 4] -= X[(i + 4) - 2];
        X[i + 6] -= X[(i + 6) - 2];
    }
}

static
void __layer_iii_dct_iv (const int32_t y[18], int32_t X[18])
{
    int32_t tmp[18];
    int     i;

    /* scale[i] = 2 * cos(PI * (2 * i + 1) / (4 * 18)) */
    static const int32_t scale[18] = {
        AW_MP3_F(0x1ff833fa),
        AW_MP3_F(0x1fb9ea93),
        AW_MP3_F(0x1f3dd120),
        AW_MP3_F(0x1e84d969),
        AW_MP3_F(0x1d906bcf),
        AW_MP3_F(0x1c62648b),
        AW_MP3_F(0x1afd100f),
        AW_MP3_F(0x1963268b),
        AW_MP3_F(0x1797c6a4),
        AW_MP3_F(0x159e6f5b),
        AW_MP3_F(0x137af940),
        AW_MP3_F(0x11318ef3),
        AW_MP3_F(0x0ec6a507),
        AW_MP3_F(0x0c3ef153),
        AW_MP3_F(0x099f61c5),
        AW_MP3_F(0x06ed12c5),
        AW_MP3_F(0x042d4544),
        AW_MP3_F(0x0165547c)
    };

    /* scaling */
    for (i = 0; i < 18; i += 3) {
        tmp[i + 0] = AW_MP3_F_MUL(y[i + 0], scale[i + 0]);
        tmp[i + 1] = AW_MP3_F_MUL(y[i + 1], scale[i + 1]);
        tmp[i + 2] = AW_MP3_F_MUL(y[i + 2], scale[i + 2]);
    }

    /* SDCT-II */
    __layer_iii_sdct_ii(tmp, X);

    /* scale reduction and output accumulation */
    X[0] /= 2;
    for (i = 1; i < 17; i += 4) {
        X[i + 0] = X[i + 0] / 2 - X[(i + 0) - 1];
        X[i + 1] = X[i + 1] / 2 - X[(i + 1) - 1];
        X[i + 2] = X[i + 2] / 2 - X[(i + 2) - 1];
        X[i + 3] = X[i + 3] / 2 - X[(i + 3) - 1];
    }
    X[17] = X[17] / 2 - X[16];
}

/**
 * \brief perform X[18]->x[36] IMDCT using Szu-Wei Lee's fast algorithm
 */
static
void __layer_iii_imdct36 (const int32_t x[18], int32_t y[36])
{
    int32_t tmp[18];
    int     i;

    /* DCT-IV */
    __layer_iii_dct_iv(x, tmp);

    /* convert 18-point DCT-IV to 36-point IMDCT */
    for (i =  0; i <  9; i += 3) {
        y[i + 0] = tmp[9 + (i + 0)];
        y[i + 1] = tmp[9 + (i + 1)];
        y[i + 2] = tmp[9 + (i + 2)];
    }

    for (i =  9; i < 27; i += 3) {
        y[i + 0] = -tmp[36 - (9 + (i + 0)) - 1];
        y[i + 1] = -tmp[36 - (9 + (i + 1)) - 1];
        y[i + 2] = -tmp[36 - (9 + (i + 2)) - 1];
    }

    for (i = 27; i < 36; i += 3) {
        y[i + 0] = -tmp[(i + 0) - 27];
        y[i + 1] = -tmp[(i + 1) - 27];
        y[i + 2] = -tmp[(i + 2) - 27];
    }
}
#  else

/**
 * \brief perform X[18]->x[36] IMDCT
 */
static
void __layer_iii_imdct36 (const int32_t X[18], int32_t x[36])
{
    int32_t          t0, t1, t2,  t3,  t4,  t5,  t6,  t7;
    int32_t          t8, t9, t10, t11, t12, t13, t14, t15;
    register int64_t hi;
    register int64_t lo;

    AW_MP3_F_ML0(hi, lo, X[4],  AW_MP3_F(0x0ec835e8));
    AW_MP3_F_MLA(hi, lo, X[13], AW_MP3_F(0x061f78aa));

    t6 = AW_MP3_F_MLZ(hi, lo);

    AW_MP3_F_MLA(hi, lo, (t14 = X[1] - X[10]), -AW_MP3_F(0x061f78aa));
    AW_MP3_F_MLA(hi, lo, (t15 = X[7] + X[16]), -AW_MP3_F(0x0ec835e8));

    t0 = AW_MP3_F_MLZ(hi, lo);

    AW_MP3_F_MLA(hi, lo, (t8  = X[0] - X[11] - X[12]),  AW_MP3_F(0x0216a2a2));
    AW_MP3_F_MLA(hi, lo, (t9  = X[2] - X[9]  - X[14]),  AW_MP3_F(0x09bd7ca0));
    AW_MP3_F_MLA(hi, lo, (t10 = X[3] - X[8]  - X[15]), -AW_MP3_F(0x0cb19346));
    AW_MP3_F_MLA(hi, lo, (t11 = X[5] - X[6]  - X[17]), -AW_MP3_F(0x0fdcf549));

    x[7]  = AW_MP3_F_MLZ(hi, lo);
    x[10] = -x[7];

    AW_MP3_F_ML0(hi, lo, t8,  -AW_MP3_F(0x0cb19346));
    AW_MP3_F_MLA(hi, lo, t9,   AW_MP3_F(0x0fdcf549));
    AW_MP3_F_MLA(hi, lo, t10,  AW_MP3_F(0x0216a2a2));
    AW_MP3_F_MLA(hi, lo, t11, -AW_MP3_F(0x09bd7ca0));

    x[19] = x[34] = AW_MP3_F_MLZ(hi, lo) - t0;

    t12   = X[0] - X[3] + X[8] - X[11] - X[12] + X[15];
    t13   = X[2] + X[5] - X[6] - X[9]  - X[14] - X[17];

    AW_MP3_F_ML0(hi, lo, t12, -AW_MP3_F(0x0ec835e8));
    AW_MP3_F_MLA(hi, lo, t13,  AW_MP3_F(0x061f78aa));

    x[22] = x[31] = AW_MP3_F_MLZ(hi, lo) + t0;

    AW_MP3_F_ML0(hi, lo, X[1],  -AW_MP3_F(0x09bd7ca0));
    AW_MP3_F_MLA(hi, lo, X[7],   AW_MP3_F(0x0216a2a2));
    AW_MP3_F_MLA(hi, lo, X[10], -AW_MP3_F(0x0fdcf549));
    AW_MP3_F_MLA(hi, lo, X[16],  AW_MP3_F(0x0cb19346));

    t1 = AW_MP3_F_MLZ(hi, lo) + t6;

    AW_MP3_F_ML0(hi, lo, X[0],   AW_MP3_F(0x03768962));
    AW_MP3_F_MLA(hi, lo, X[2],   AW_MP3_F(0x0e313245));
    AW_MP3_F_MLA(hi, lo, X[3],  -AW_MP3_F(0x0ffc19fd));
    AW_MP3_F_MLA(hi, lo, X[5],  -AW_MP3_F(0x0acf37ad));
    AW_MP3_F_MLA(hi, lo, X[6],   AW_MP3_F(0x04cfb0e2));
    AW_MP3_F_MLA(hi, lo, X[8],  -AW_MP3_F(0x0898c779));
    AW_MP3_F_MLA(hi, lo, X[9],   AW_MP3_F(0x0d7e8807));
    AW_MP3_F_MLA(hi, lo, X[11],  AW_MP3_F(0x0f426cb5));
    AW_MP3_F_MLA(hi, lo, X[12], -AW_MP3_F(0x0bcbe352));
    AW_MP3_F_MLA(hi, lo, X[14],  AW_MP3_F(0x00b2aa3e));
    AW_MP3_F_MLA(hi, lo, X[15], -AW_MP3_F(0x07635284));
    AW_MP3_F_MLA(hi, lo, X[17], -AW_MP3_F(0x0f9ee890));

    x[6]  = AW_MP3_MLZ(hi, lo) + t1;
    x[11] = -x[6];

    AW_MP3_F_ML0(hi, lo, X[0],  -AW_MP3_F(0x0f426cb5));
    AW_MP3_F_MLA(hi, lo, X[2],  -AW_MP3_F(0x00b2aa3e));
    AW_MP3_F_MLA(hi, lo, X[3],   AW_MP3_F(0x0898c779));
    AW_MP3_F_MLA(hi, lo, X[5],   AW_MP3_F(0x0f9ee890));
    AW_MP3_F_MLA(hi, lo, X[6],   AW_MP3_F(0x0acf37ad));
    AW_MP3_F_MLA(hi, lo, X[8],  -AW_MP3_F(0x07635284));
    AW_MP3_F_MLA(hi, lo, X[9],  -AW_MP3_F(0x0e313245));
    AW_MP3_F_MLA(hi, lo, X[11], -AW_MP3_F(0x0bcbe352));
    AW_MP3_F_MLA(hi, lo, X[12], -AW_MP3_F(0x03768962));
    AW_MP3_F_MLA(hi, lo, X[14],  AW_MP3_F(0x0d7e8807));
    AW_MP3_F_MLA(hi, lo, X[15],  AW_MP3_F(0x0ffc19fd));
    AW_MP3_F_MLA(hi, lo, X[17],  AW_MP3_F(0x04cfb0e2));

    x[23] = x[30] = AW_MP3_F_MLZ(hi, lo) + t1;

    AW_MP3_F_ML0(hi, lo, X[0],  -AW_MP3_F(0x0bcbe352));
    AW_MP3_F_MLA(hi, lo, X[2],   AW_MP3_F(0x0d7e8807));
    AW_MP3_F_MLA(hi, lo, X[3],  -AW_MP3_F(0x07635284));
    AW_MP3_F_MLA(hi, lo, X[5],   AW_MP3_F(0x04cfb0e2));
    AW_MP3_F_MLA(hi, lo, X[6],   AW_MP3_F(0x0f9ee890));
    AW_MP3_F_MLA(hi, lo, X[8],  -AW_MP3_F(0x0ffc19fd));
    AW_MP3_F_MLA(hi, lo, X[9],  -AW_MP3_F(0x00b2aa3e));
    AW_MP3_F_MLA(hi, lo, X[11],  AW_MP3_F(0x03768962));
    AW_MP3_F_MLA(hi, lo, X[12], -AW_MP3_F(0x0f426cb5));
    AW_MP3_F_MLA(hi, lo, X[14],  AW_MP3_F(0x0e313245));
    AW_MP3_F_MLA(hi, lo, X[15],  AW_MP3_F(0x0898c779));
    AW_MP3_F_MLA(hi, lo, X[17], -AW_MP3_F(0x0acf37ad));

    x[18] = x[35] = AW_MP3_F_MLZ(hi, lo) - t1;

    AW_MP3_F_ML0(hi, lo, X[4],   AW_MP3_F(0x061f78aa));
    AW_MP3_F_MLA(hi, lo, X[13], -AW_MP3_F(0x0ec835e8));

    t7 = AW_MP3_F_MLZ(hi, lo);

    AW_MP3_F_MLA(hi, lo, X[1],  -AW_MP3_F(0x0cb19346));
    AW_MP3_F_MLA(hi, lo, X[7],   AW_MP3_F(0x0fdcf549));
    AW_MP3_F_MLA(hi, lo, X[10],  AW_MP3_F(0x0216a2a2));
    AW_MP3_F_MLA(hi, lo, X[16], -AW_MP3_F(0x09bd7ca0));

    t2 = AW_MP3_F_MLZ(hi, lo);

    AW_MP3_F_MLA(hi, lo, X[0],   AW_MP3_F(0x04cfb0e2));
    AW_MP3_F_MLA(hi, lo, X[2],   AW_MP3_F(0x0ffc19fd));
    AW_MP3_F_MLA(hi, lo, X[3],  -AW_MP3_F(0x0d7e8807));
    AW_MP3_F_MLA(hi, lo, X[5],   AW_MP3_F(0x03768962));
    AW_MP3_F_MLA(hi, lo, X[6],  -AW_MP3_F(0x0bcbe352));
    AW_MP3_F_MLA(hi, lo, X[8],  -AW_MP3_F(0x0e313245));
    AW_MP3_F_MLA(hi, lo, X[9],   AW_MP3_F(0x07635284));
    AW_MP3_F_MLA(hi, lo, X[11], -AW_MP3_F(0x0acf37ad));
    AW_MP3_F_MLA(hi, lo, X[12],  AW_MP3_F(0x0f9ee890));
    AW_MP3_F_MLA(hi, lo, X[14],  AW_MP3_F(0x0898c779));
    AW_MP3_F_MLA(hi, lo, X[15],  AW_MP3_F(0x00b2aa3e));
    AW_MP3_F_MLA(hi, lo, X[17],  AW_MP3_F(0x0f426cb5));

    x[5]  = AW_MP3_F_MLZ(hi, lo);
    x[12] = -x[5];

    AW_MP3_F_ML0(hi, lo, X[0],   AW_MP3_F(0x0acf37ad));
    AW_MP3_F_MLA(hi, lo, X[2],  -AW_MP3_F(0x0898c779));
    AW_MP3_F_MLA(hi, lo, X[3],   AW_MP3_F(0x0e313245));
    AW_MP3_F_MLA(hi, lo, X[5],  -AW_MP3_F(0x0f426cb5));
    AW_MP3_F_MLA(hi, lo, X[6],  -AW_MP3_F(0x03768962));
    AW_MP3_F_MLA(hi, lo, X[8],   AW_MP3_F(0x00b2aa3e));
    AW_MP3_F_MLA(hi, lo, X[9],  -AW_MP3_F(0x0ffc19fd));
    AW_MP3_F_MLA(hi, lo, X[11],  AW_MP3_F(0x0f9ee890));
    AW_MP3_F_MLA(hi, lo, X[12], -AW_MP3_F(0x04cfb0e2));
    AW_MP3_F_MLA(hi, lo, X[14],  AW_MP3_F(0x07635284));
    AW_MP3_F_MLA(hi, lo, X[15],  AW_MP3_F(0x0d7e8807));
    AW_MP3_F_MLA(hi, lo, X[17], -AW_MP3_F(0x0bcbe352));

    x[0]  = AW_MP3_F_MLZ(hi, lo) + t2;
    x[17] = -x[0];

    AW_MP3_F_ML0(hi, lo, X[0],  -AW_MP3_F(0x0f9ee890));
    AW_MP3_F_MLA(hi, lo, X[2],  -AW_MP3_F(0x07635284));
    AW_MP3_F_MLA(hi, lo, X[3],  -AW_MP3_F(0x00b2aa3e));
    AW_MP3_F_MLA(hi, lo, X[5],   AW_MP3_F(0x0bcbe352));
    AW_MP3_F_MLA(hi, lo, X[6],   AW_MP3_F(0x0f426cb5));
    AW_MP3_F_MLA(hi, lo, X[8],   AW_MP3_F(0x0d7e8807));
    AW_MP3_F_MLA(hi, lo, X[9],   AW_MP3_F(0x0898c779));
    AW_MP3_F_MLA(hi, lo, X[11], -AW_MP3_F(0x04cfb0e2));
    AW_MP3_F_MLA(hi, lo, X[12], -AW_MP3_F(0x0acf37ad));
    AW_MP3_F_MLA(hi, lo, X[14], -AW_MP3_F(0x0ffc19fd));
    AW_MP3_F_MLA(hi, lo, X[15], -AW_MP3_F(0x0e313245));
    AW_MP3_F_MLA(hi, lo, X[17], -AW_MP3_F(0x03768962));

    x[24] = x[29] = AW_MP3_F_MLZ(hi, lo) + t2;

    AW_MP3_F_ML0(hi, lo, X[1],  -AW_MP3_F(0x0216a2a2));
    AW_MP3_F_MLA(hi, lo, X[7],  -AW_MP3_F(0x09bd7ca0));
    AW_MP3_F_MLA(hi, lo, X[10],  AW_MP3_F(0x0cb19346));
    AW_MP3_F_MLA(hi, lo, X[16],  AW_MP3_F(0x0fdcf549));

    t3 = AW_MP3_F_MLZ(hi, lo) + t7;

    AW_MP3_F_ML0(hi, lo, X[0],   AW_MP3_F(0x00b2aa3e));
    AW_MP3_F_MLA(hi, lo, X[2],   AW_MP3_F(0x03768962));
    AW_MP3_F_MLA(hi, lo, X[3],  -AW_MP3_F(0x04cfb0e2));
    AW_MP3_F_MLA(hi, lo, X[5],  -AW_MP3_F(0x07635284));
    AW_MP3_F_MLA(hi, lo, X[6],   AW_MP3_F(0x0898c779));
    AW_MP3_F_MLA(hi, lo, X[8],   AW_MP3_F(0x0acf37ad));
    AW_MP3_F_MLA(hi, lo, X[9],  -AW_MP3_F(0x0bcbe352));
    AW_MP3_F_MLA(hi, lo, X[11], -AW_MP3_F(0x0d7e8807));
    AW_MP3_F_MLA(hi, lo, X[12],  AW_MP3_F(0x0e313245));
    AW_MP3_F_MLA(hi, lo, X[14],  AW_MP3_F(0x0f426cb5));
    AW_MP3_F_MLA(hi, lo, X[15], -AW_MP3_F(0x0f9ee890));
    AW_MP3_F_MLA(hi, lo, X[17], -AW_MP3_F(0x0ffc19fd));

    x[8] = AW_MP3_F_MLZ(hi, lo) + t3;
    x[9] = -x[8];

    AW_MP3_F_ML0(hi, lo, X[0],  -AW_MP3_F(0x0e313245));
    AW_MP3_F_MLA(hi, lo, X[2],   AW_MP3_F(0x0bcbe352));
    AW_MP3_F_MLA(hi, lo, X[3],   AW_MP3_F(0x0f9ee890));
    AW_MP3_F_MLA(hi, lo, X[5],  -AW_MP3_F(0x0898c779));
    AW_MP3_F_MLA(hi, lo, X[6],  -AW_MP3_F(0x0ffc19fd));
    AW_MP3_F_MLA(hi, lo, X[8],   AW_MP3_F(0x04cfb0e2));
    AW_MP3_F_MLA(hi, lo, X[9],   AW_MP3_F(0x0f426cb5));
    AW_MP3_F_MLA(hi, lo, X[11], -AW_MP3_F(0x00b2aa3e));
    AW_MP3_F_MLA(hi, lo, X[12], -AW_MP3_F(0x0d7e8807));
    AW_MP3_F_MLA(hi, lo, X[14], -AW_MP3_F(0x03768962));
    AW_MP3_F_MLA(hi, lo, X[15],  AW_MP3_F(0x0acf37ad));
    AW_MP3_F_MLA(hi, lo, X[17],  AW_MP3_F(0x07635284));

    x[21] = x[32] = AW_MP3_F_MLZ(hi, lo) + t3;

    AW_MP3_F_ML0(hi, lo, X[0],  -AW_MP3_F(0x0d7e8807));
    AW_MP3_F_MLA(hi, lo, X[2],   AW_MP3_F(0x0f426cb5));
    AW_MP3_F_MLA(hi, lo, X[3],   AW_MP3_F(0x0acf37ad));
    AW_MP3_F_MLA(hi, lo, X[5],  -AW_MP3_F(0x0ffc19fd));
    AW_MP3_F_MLA(hi, lo, X[6],  -AW_MP3_F(0x07635284));
    AW_MP3_F_MLA(hi, lo, X[8],   AW_MP3_F(0x0f9ee890));
    AW_MP3_F_MLA(hi, lo, X[9],   AW_MP3_F(0x03768962));
    AW_MP3_F_MLA(hi, lo, X[11], -AW_MP3_F(0x0e313245));
    AW_MP3_F_MLA(hi, lo, X[12],  AW_MP3_F(0x00b2aa3e));
    AW_MP3_F_MLA(hi, lo, X[14],  AW_MP3_F(0x0bcbe352));
    AW_MP3_F_MLA(hi, lo, X[15], -AW_MP3_F(0x04cfb0e2));
    AW_MP3_F_MLA(hi, lo, X[17], -AW_MP3_F(0x0898c779));

    x[20] = x[33] = AW_MP3_F_MLZ(hi, lo) - t3;

    AW_MP3_F_ML0(hi, lo, t14, -AW_MP3_F(0x0ec835e8));
    AW_MP3_F_MLA(hi, lo, t15,  AW_MP3_F(0x061f78aa));

    t4 = AW_MP3_F_MLZ(hi, lo) - t7;

    AW_MP3_F_ML0(hi, lo, t12, AW_MP3_F(0x061f78aa));
    AW_MP3_F_MLA(hi, lo, t13, AW_MP3_F(0x0ec835e8));

    x[4]  = AW_MP3_F_MLZ(hi, lo) + t4;
    x[13] = -x[4];

    AW_MP3_F_ML0(hi, lo, t8,   AW_MP3_F(0x09bd7ca0));
    AW_MP3_F_MLA(hi, lo, t9,  -AW_MP3_F(0x0216a2a2));
    AW_MP3_F_MLA(hi, lo, t10,  AW_MP3_F(0x0fdcf549));
    AW_MP3_F_MLA(hi, lo, t11, -AW_MP3_F(0x0cb19346));

    x[1]  = AW_MP3_F_MLZ(hi, lo) + t4;
    x[16] = -x[1];

    AW_MP3_F_ML0(hi, lo, t8,  -AW_MP3_F(0x0fdcf549));
    AW_MP3_F_MLA(hi, lo, t9,  -AW_MP3_F(0x0cb19346));
    AW_MP3_F_MLA(hi, lo, t10, -AW_MP3_F(0x09bd7ca0));
    AW_MP3_F_MLA(hi, lo, t11, -AW_MP3_F(0x0216a2a2));

    x[25] = x[28] = AW_MP3_F_MLZ(hi, lo) + t4;

    AW_MP3_F_ML0(hi, lo, X[1],  -AW_MP3_F(0x0fdcf549));
    AW_MP3_F_MLA(hi, lo, X[7],  -AW_MP3_F(0x0cb19346));
    AW_MP3_F_MLA(hi, lo, X[10], -AW_MP3_F(0x09bd7ca0));
    AW_MP3_F_MLA(hi, lo, X[16], -AW_MP3_F(0x0216a2a2));

    t5 = AW_MP3_MLZ(hi, lo) - t6;

    AW_MP3_ML0(hi, lo, X[0],   AW_MP3(0x0898c779));
    AW_MP3_MLA(hi, lo, X[2],   AW_MP3(0x04cfb0e2));
    AW_MP3_MLA(hi, lo, X[3],   AW_MP3(0x0bcbe352));
    AW_MP3_MLA(hi, lo, X[5],   AW_MP3(0x00b2aa3e));
    AW_MP3_MLA(hi, lo, X[6],   AW_MP3(0x0e313245));
    AW_MP3_MLA(hi, lo, X[8],  -AW_MP3(0x03768962));
    AW_MP3_MLA(hi, lo, X[9],   AW_MP3(0x0f9ee890));
    AW_MP3_MLA(hi, lo, X[11], -AW_MP3(0x07635284));
    AW_MP3_MLA(hi, lo, X[12],  AW_MP3(0x0ffc19fd));
    AW_MP3_MLA(hi, lo, X[14], -AW_MP3(0x0acf37ad));
    AW_MP3_MLA(hi, lo, X[15],  AW_MP3(0x0f426cb5));
    AW_MP3_MLA(hi, lo, X[17], -AW_MP3(0x0d7e8807));

    x[2]  = AW_MP3_MLZ(hi, lo) + t5;
    x[15] = -x[2];

    AW_MP3_ML0(hi, lo, X[0],   AW_MP3(0x07635284));
    AW_MP3_MLA(hi, lo, X[2],   AW_MP3(0x0acf37ad));
    AW_MP3_MLA(hi, lo, X[3],   AW_MP3(0x03768962));
    AW_MP3_MLA(hi, lo, X[5],   AW_MP3(0x0d7e8807));
    AW_MP3_MLA(hi, lo, X[6],  -AW_MP3(0x00b2aa3e));
    AW_MP3_MLA(hi, lo, X[8],   AW_MP3(0x0f426cb5));
    AW_MP3_MLA(hi, lo, X[9],  -AW_MP3(0x04cfb0e2));
    AW_MP3_MLA(hi, lo, X[11],  AW_MP3(0x0ffc19fd));
    AW_MP3_MLA(hi, lo, X[12], -AW_MP3(0x0898c779));
    AW_MP3_MLA(hi, lo, X[14],  AW_MP3(0x0f9ee890));
    AW_MP3_MLA(hi, lo, X[15], -AW_MP3(0x0bcbe352));
    AW_MP3_MLA(hi, lo, X[17],  AW_MP3(0x0e313245));

    x[3]  = AW_MP3_MLZ(hi, lo) + t5;
    x[14] = -x[3];

    AW_MP3_ML0(hi, lo, X[0],  -AW_MP3(0x0ffc19fd));
    AW_MP3_MLA(hi, lo, X[2],  -AW_MP3(0x0f9ee890));
    AW_MP3_MLA(hi, lo, X[3],  -AW_MP3(0x0f426cb5));
    AW_MP3_MLA(hi, lo, X[5],  -AW_MP3(0x0e313245));
    AW_MP3_MLA(hi, lo, X[6],  -AW_MP3(0x0d7e8807));
    AW_MP3_MLA(hi, lo, X[8],  -AW_MP3(0x0bcbe352));
    AW_MP3_MLA(hi, lo, X[9],  -AW_MP3(0x0acf37ad));
    AW_MP3_MLA(hi, lo, X[11], -AW_MP3(0x0898c779));
    AW_MP3_MLA(hi, lo, X[12], -AW_MP3(0x07635284));
    AW_MP3_MLA(hi, lo, X[14], -AW_MP3(0x04cfb0e2));
    AW_MP3_MLA(hi, lo, X[15], -AW_MP3(0x03768962));
    AW_MP3_MLA(hi, lo, X[17], -AW_MP3(0x00b2aa3e));

    x[26] = x[27] = AW_MP3_MLZ(hi, lo) + t5;
}
#endif

#endif

/**
 * \brief perform IMDCT and windowing for short blocks
 */
static void __layer_iii_imdct_s (const int32_t X[18], int32_t z[36])
{
    int32_t           y[36], *p_yptr;
    const int32_t    *p_wptr;
    int               w, i;
    register int64_t  hi;
    register int64_t  lo;

    /* IMDCT */
    p_yptr = &y[0];

    for (w = 0; w < 3; ++w) {
        register const int32_t (*s)[6];

        s = g_mp3_dec_imdct_s;

        for (i = 0; i < 3; ++i) {
            AW_MP3_F_ML0(hi, lo, X[0], (*s)[0]);
            AW_MP3_F_MLA(hi, lo, X[1], (*s)[1]);
            AW_MP3_F_MLA(hi, lo, X[2], (*s)[2]);
            AW_MP3_F_MLA(hi, lo, X[3], (*s)[3]);
            AW_MP3_F_MLA(hi, lo, X[4], (*s)[4]);
            AW_MP3_F_MLA(hi, lo, X[5], (*s)[5]);

            p_yptr[i + 0] = AW_MP3_F_MLZ(hi, lo);
            p_yptr[5 - i] = -p_yptr[i + 0];

            ++s;

            AW_MP3_F_ML0(hi, lo, X[0], (*s)[0]);
            AW_MP3_F_MLA(hi, lo, X[1], (*s)[1]);
            AW_MP3_F_MLA(hi, lo, X[2], (*s)[2]);
            AW_MP3_F_MLA(hi, lo, X[3], (*s)[3]);
            AW_MP3_F_MLA(hi, lo, X[4], (*s)[4]);
            AW_MP3_F_MLA(hi, lo, X[5], (*s)[5]);

            p_yptr[ i + 6] = AW_MP3_F_MLZ(hi, lo);
            p_yptr[11 - i] = p_yptr[i + 6];

            ++s;
        }

        p_yptr += 12;
        X    += 6;
    }

    /* windowing, overlapping and concatenation */

    p_yptr = &y[0];
    p_wptr = &g_mp3_dec_window_s[0];

    for (i = 0; i < 6; ++i) {
        z[i +  0] = 0;
        z[i +  6] = AW_MP3_F_MUL(p_yptr[ 0 + 0], p_wptr[0]);

        AW_MP3_F_ML0(hi, lo, p_yptr[ 0 + 6], p_wptr[6]);
        AW_MP3_F_MLA(hi, lo, p_yptr[12 + 0], p_wptr[0]);

        z[i + 12] = AW_MP3_F_MLZ(hi, lo);

        AW_MP3_F_ML0(hi, lo, p_yptr[12 + 6], p_wptr[6]);
        AW_MP3_F_MLA(hi, lo, p_yptr[24 + 0], p_wptr[0]);

        z[i + 18] = AW_MP3_F_MLZ(hi, lo);

        z[i + 24] = AW_MP3_F_MUL(p_yptr[24 + 6], p_wptr[6]);
        z[i + 30] = 0;

        ++p_yptr;
        ++p_wptr;
    }
}

/**
 * \brief perform overlap-add of windowed IMDCT outputs
 */
static void __layer_iii_overlap (const int32_t output[36],
                                 int32_t       overlap[18],
                                 int32_t       sample[18][32],
                                 uint32_t      sb)
{
    uint32_t i;

#if (AW_MP3_DEC_ASO_INTERLEAVE2)
    {
        register int32_t tmp1, tmp2;

        tmp1 = overlap[0];
        tmp2 = overlap[1];

        for (i = 0; i < 16; i += 2) {
            sample[i + 0][sb] = output[i + 0 +  0] + tmp1;
            overlap[i + 0]    = output[i + 0 + 18];
            tmp1              = overlap[i + 2];

            sample[i + 1][sb] = output[i + 1 +  0] + tmp2;
            overlap[i + 1]    = output[i + 1 + 18];
            tmp2              = overlap[i + 3];
        }

        sample[16][sb] = output[16 +  0] + tmp1;
        overlap[16]    = output[16 + 18];
        sample[17][sb] = output[17 +  0] + tmp2;
        overlap[17]    = output[17 + 18];
    }
#elif 0
    for (i = 0; i < 18; i += 2) {
        sample[i + 0][sb] = output[i + 0 +  0] + overlap[i + 0];
        overlap[i + 0]    = output[i + 0 + 18];

        sample[i + 1][sb] = output[i + 1 +  0] + overlap[i + 1];
        overlap[i + 1]    = output[i + 1 + 18];
    }
#else
    for (i = 0; i < 18; ++i) {
        sample[i][sb] = output[i +  0] + overlap[i];
        overlap[i]    = output[i + 18];
    }
#endif
}

/**
 * \brief perform "overlap-add" of zero IMDCT outputs
 */
aw_static_inline
void __layer_iii_overlap_z (int32_t  overlap[18],
                            int32_t  sample[18][32],
                            uint32_t sb)
{
    uint32_t i;

#if (AW_MP3_DEC_ASO_INTERLEAVE2)
    {
        register int32_t tmp1, tmp2;

        tmp1 = overlap[0];
        tmp2 = overlap[1];

        for (i = 0; i < 16; i += 2) {
            sample[i + 0][sb] = tmp1;
            overlap[i + 0]    = 0;
            tmp1              = overlap[i + 2];

            sample[i + 1][sb] = tmp2;
            overlap[i + 1]    = 0;
            tmp2              = overlap[i + 3];
        }

        sample[16][sb] = tmp1;
        overlap[16]    = 0;
        sample[17][sb] = tmp2;
        overlap[17]    = 0;
    }
#else
    for (i = 0; i < 18; ++i) {
        sample[i][sb] = overlap[i];
        overlap[i]    = 0;
    }
#endif
}

/**
 * \brief perform subband frequency inversion for odd sample lines
 */
static void __layer_iii_freq_inversion (int32_t  sample[18][32],
                                        uint32_t sb)
{
    uint32_t i;

#if 1 || (AW_MP3_DEC_ASO_INTERLEAVE1) || (AW_MP3_DEC_ASO_INTERLEAVE2)
    {
        register int32_t tmp1, tmp2;

        tmp1 = sample[1][sb];
        tmp2 = sample[3][sb];

        for (i = 1; i < 13; i += 4) {
            sample[i + 0][sb] = -tmp1;
            tmp1              = sample[i + 4][sb];
            sample[i + 2][sb] = -tmp2;
            tmp2              = sample[i + 6][sb];
        }

        sample[13][sb] = -tmp1;
        tmp1           = sample[17][sb];
        sample[15][sb] = -tmp2;
        sample[17][sb] = -tmp1;
    }
# else
    for (i = 1; i < 18; i += 2) {
        sample[i][sb] = -sample[i][sb];
    }
#endif
}

/**
 * \brief decode frame main_data
 */
static enum aw_mp3_dec_err
__layer_iii_decode (struct aw_mp3_dec_bit        *p_ptr,
                    struct aw_mp3_dec_frame      *p_frame,
                    struct __layer_iii_side_info *p_si,
                    uint32_t                      nch)
{
    struct aw_mp3_dec_frame_header *p_header = &p_frame->header;
    uint32_t                        sfreqi;
    uint32_t                        ngr;
    uint32_t                        gr;

    uint32_t                        sfreq;

    sfreq = p_header->sample_rate;
    if (p_header->flags & AW_MP3_DEC_FRAME_FLAG_MPEG_2_5_EXT) {
        sfreq *= 2;
    }

    /*
     * 48000 => 0, 44100 => 1, 32000 => 2,
     * 24000 => 3, 22050 => 4, 16000 => 5
     */
    sfreqi = ((sfreq >>  7) & 0x000f) +
             ((sfreq >> 15) & 0x0001) - 8;

    if (p_header->flags & AW_MP3_DEC_FRAME_FLAG_MPEG_2_5_EXT) {
        sfreqi += 3;
    }

    /* scalefactors, Huffman decoding, requantization */
    ngr = (p_header->flags & AW_MP3_DEC_FRAME_FLAG_LSF_EXT) ? 1 : 2;

    for (gr = 0; gr < ngr; ++gr) {

        struct __layer_iii_granule *p_granule = &p_si->gr[gr];
        const uint8_t              *p_sfb_width[2];
        int32_t                     xr[2][576];
        uint32_t                    ch;
        enum aw_mp3_dec_err         error;

        for (ch = 0; ch < nch; ++ch) {
            struct __layer_iii_channel *p_channel = &p_granule->ch[ch];
            uint32_t                    part2_length;

            p_sfb_width[ch] = __layer_iii_sfb_width_table[sfreqi].p_l;
            if (p_channel->block_type == 2) {
                p_sfb_width[ch] = (p_channel->flags &
                                   __LAYER_III_FLG_MIXED_BLOCK)            ?
                                   __layer_iii_sfb_width_table[sfreqi].p_m :
                                   __layer_iii_sfb_width_table[sfreqi].p_s;
            }

            if (p_header->flags & AW_MP3_DEC_FRAME_FLAG_LSF_EXT) {
                part2_length = __layer_iii_scale_factors_lsf(
                                   p_ptr,
                                   p_channel,
                                   ch == 0 ? 0 : &p_si->gr[1].ch[1],
                                   p_header->mode_extension);
            } else {
                part2_length = __layer_iii_scale_factors(p_ptr,
                                                         p_channel,
                                                         &p_si->gr[0].ch[ch],
                                                         gr == 0 ?
                                                         0 : p_si->scfsi[ch]);
            }

            error = __layer_iii_huffman_decode(p_ptr,
                                               xr[ch],
                                               p_channel,
                                               p_sfb_width[ch],
                                               part2_length);
            if (error != AW_MP3_DEC_ERR_NONE) {
                return error;
            }
        }

        /* joint stereo processing */
        if (p_header->mode == AW_MP3_DEC_FRAME_MODE_JOINT_STEREO &&
            p_header->mode_extension) {
            error = __layer_iii_stereo(xr,
                                       p_granule,
                                       p_header,
                                       p_sfb_width[0]);
            if (error != AW_MP3_DEC_ERR_NONE) {
                return error;
            }
        }

        /*
         * reordering, alias reduction, IMDCT, overlap-add,
         * frequency inversion
         */
        for (ch = 0; ch < nch; ++ch) {
            const struct __layer_iii_channel *p_channel     = &p_granule->ch[ch];
            int32_t                         (*p_sample)[32] = \
                 &p_frame->sbsample[ch][18 * gr];

            uint32_t sb;
            uint32_t l;
            uint32_t i;
            uint32_t sblimit;
            int32_t  output[36];

            if (p_channel->block_type == 2) {
                __layer_iii_reorder(xr[ch], p_channel, p_sfb_width[ch]);

#if !(AW_MP3_DEC_OPT_STRICT)

                /*
                 * According to ISO/IEC 11172-3, "Alias reduction is not
                 * applied for granules with block_type == 2 (short block)."
                 * However, other sources suggest alias reduction should
                 * indeed be performed on the lower two subbands of mixed
                 * blocks. Most other implementations do this, so by default
                 * we will too.
                 */
                if (p_channel->flags & __LAYER_III_FLG_MIXED_BLOCK) {
                    __layer_iii_aliasreduce(xr[ch], 36);
                }
#endif
            } else {
                __layer_iii_aliasreduce(xr[ch], 576);
            }

            l = 0;

            /* subbands 0-1 */
            if (p_channel->block_type != 2 ||
                (p_channel->flags & __LAYER_III_FLG_MIXED_BLOCK)) {
                uint32_t block_type;

                block_type = p_channel->block_type;
                if (p_channel->flags & __LAYER_III_FLG_MIXED_BLOCK) {
                    block_type = 0;
                }

                /* long blocks */
                for (sb = 0; sb < 2; ++sb, l += 18) {
                    aw_mp3_dec_layer_iii_imdct_l(&xr[ch][l],
                                                 output,
                                                 block_type);
                    __layer_iii_overlap(output,
                                        p_frame->overlap[ch][sb],
                                        p_sample,
                                        sb);
                }
            } else {

                /* short blocks */
                for (sb = 0; sb < 2; ++sb, l += 18) {
                    __layer_iii_imdct_s(&xr[ch][l], output);
                    __layer_iii_overlap(output,
                                        p_frame->overlap[ch][sb],
                                        p_sample,
                                        sb);
                }
            }

            __layer_iii_freq_inversion(p_sample, 1);

            /* (nonzero) subbands 2-31 */
            i = 576;
            while (i > 36 && xr[ch][i - 1] == 0) {
                --i;
            }

            sblimit = 32 - (576 - i) / 18;

            if (p_channel->block_type != 2) {

                /* long blocks */
                for (sb = 2; sb < sblimit; ++sb, l += 18) {
                    aw_mp3_dec_layer_iii_imdct_l(&xr[ch][l],
                                                 output,
                                                 p_channel->block_type);
                    __layer_iii_overlap(output,
                                        p_frame->overlap[ch][sb],
                                        p_sample,
                                        sb);

                    if (sb & 1) {
                        __layer_iii_freq_inversion(p_sample, sb);
                    }
                }
            } else {

                /* short blocks */
                for (sb = 2; sb < sblimit; ++sb, l += 18) {
                    __layer_iii_imdct_s(&xr[ch][l], output);
                    __layer_iii_overlap(output,
                                        p_frame->overlap[ch][sb],
                                        p_sample,
                                        sb);

                    if (sb & 1) {
                        __layer_iii_freq_inversion(p_sample, sb);
                    }
                }
            }

            /* remaining (zero) subbands */
            for (sb = sblimit; sb < 32; ++sb) {
                __layer_iii_overlap_z(p_frame->overlap[ch][sb], p_sample, sb);

                if (sb & 1) {
                    __layer_iii_freq_inversion(p_sample, sb);
                }
            }
        }
    }

    return AW_MP3_DEC_ERR_NONE;
}

/******************************************************************************/

/**
 * \brief Layer I ½âÂë
 */
aw_err_t aw_mp3_dec_layer_i_decode (struct aw_mp3_dec_stream *p_stream,
                                    struct aw_mp3_dec_frame  *p_frame)
{
    struct aw_mp3_dec_frame_header *p_header = &p_frame->header;
    uint32_t                        nch, bound, ch, s, sb, nb;
    uint8_t                         allocation[2][32], scale_factor[2][32];

    nch = AW_MP3_DEC_FRAME_NCHANNELS(&p_frame->header);

    bound = 32;
    if (p_header->mode == AW_MP3_DEC_FRAME_MODE_JOINT_STEREO) {
        p_header->flags |= AW_MP3_DEC_FRAME_FLAG_I_STEREO;
        bound = 4 + p_header->mode_extension * 4;
    }

    /* check CRC word */
    if (p_header->flags & AW_MP3_DEC_FRAME_FLAG_PROTECTION) {
        p_header->crc_check = aw_mp3_dec_bit_crc(
                                  p_stream->ptr,
                                  4 * (bound * nch + (32 - bound)),
                                  p_header->crc_check);

        if (p_header->crc_check != p_header->crc_target &&
            !(p_frame->options & AW_MP3_DEC_OPTION_IGNORECRC)) {
            p_stream->error = AW_MP3_DEC_ERR_BADCRC;
            return AW_ERROR;
        }
    }

    /* decode bit allocations */
    for (sb = 0; sb < bound; ++sb) {
        for (ch = 0; ch < nch; ++ch) {
            nb = aw_mp3_dec_bit_read(&p_stream->ptr, 4);

            if (nb == 15) {
                p_stream->error = AW_MP3_DEC_ERR_BADBITALLOC;
                return AW_ERROR;
            }

            allocation[ch][sb] = nb ? nb + 1 : 0;
        }
    }

    for (sb = bound; sb < 32; ++sb) {
        nb = aw_mp3_dec_bit_read(&p_stream->ptr, 4);

        if (nb == 15) {
            p_stream->error = AW_MP3_DEC_ERR_BADBITALLOC;
            return AW_ERROR;
        }

        allocation[0][sb] = allocation[1][sb] = nb ? nb + 1 : 0;
    }

    /* decode scalefactors */

    for (sb = 0; sb < 32; ++sb) {
        for (ch = 0; ch < nch; ++ch) {
            if (allocation[ch][sb]) {
                scale_factor[ch][sb] = aw_mp3_dec_bit_read(&p_stream->ptr, 6);

#if (AW_MP3_DEC_OPT_STRICT)

               /*
                * Scalefactor index 63 does not appear in Table B.1 of
                * ISO/IEC 11172-3. Nonetheless, other implementations accept it,
                * so we only reject it if OPT_STRICT is defined.
                */
                if (scale_factor[ch][sb] == 63) {
                    p_stream->error = AW_MP3_DEC_ERR_BADSCALEFACTOR;
                    return AW_ERROR;
                }
#endif
            }
        }
    }

    /* decode samples */
    for (s = 0; s < 12; ++s) {
        for (sb = 0; sb < bound; ++sb) {
            for (ch = 0; ch < nch; ++ch) {
                nb = allocation[ch][sb];
                p_frame->sbsample[ch][s][sb] = \
                    nb ? AW_MP3_F_MUL(__layer_i_sample(&p_stream->ptr,
                                                       nb),
                                      g_mp3_dec_sf_table[scale_factor[ch][sb]])
                       : 0;
            }
        }

        for (sb = bound; sb < 32; ++sb) {
            if ((nb = allocation[0][sb]) != 0) {
                int32_t sample;

                sample = __layer_i_sample(&p_stream->ptr, nb);

                for (ch = 0; ch < nch; ++ch) {
                    p_frame->sbsample[ch][s][sb] =
                        AW_MP3_F_MUL(sample,
                                     g_mp3_dec_sf_table[scale_factor[ch][sb]]);
                }
            } else {
                for (ch = 0; ch < nch; ++ch) {
                    p_frame->sbsample[ch][s][sb] = 0;
                }
            }
        }
    }

    return AW_OK;
}

/**
 * \brief Layer II ½âÂë
 */
aw_err_t aw_mp3_dec_layer_ii_decode (struct aw_mp3_dec_stream *p_stream,
                                     struct aw_mp3_dec_frame  *p_frame)
{
    struct aw_mp3_dec_frame_header *p_header = &p_frame->header;
    struct aw_mp3_dec_bit           start;
    uint32_t                        index;
    uint32_t                        sblimit;
    uint32_t                        nbal;
    uint32_t                        nch;
    uint32_t                        bound;
    uint32_t                        gr;
    uint32_t                        ch;
    uint32_t                        s;
    uint32_t                        sb;
    const uint8_t                  *p_offsets;
    uint8_t                         allocation[2][32];
    uint8_t                         scfsi[2][32];
    uint8_t                         scale_factor[2][32][3];
    int32_t                         samples[3];

    nch = AW_MP3_DEC_FRAME_NCHANNELS(&p_frame->header);

    if (p_header->flags & AW_MP3_DEC_FRAME_FLAG_LSF_EXT) {
        index = 4;
    } else if (p_header->flags & AW_MP3_DEC_FRAME_FLAG_FREEFORMAT) {
        goto __free_format;
    } else {
        uint32_t bitrate_per_channel;

        bitrate_per_channel = p_header->bit_rate;
        if (nch == 2) {
            bitrate_per_channel /= 2;

#if (AW_MP3_OPT_STRICT)
            /*
             * ISO/IEC 11172-3 allows only single channel mode for 32, 48, 56,
             * and 80 kbps bitrates in Layer II, but some encoders ignore this
             * restriction. We enforce it if OPT_STRICT is defined.
             */
            if (bitrate_per_channel <= 28000 || bitrate_per_channel == 40000) {
                p_stream->error = AW_MP3_DEC_ERR_BADMODE;
                return AW_ERROR;
            }
#endif
        } else {

            /* nch == 1 */
            if (bitrate_per_channel > 192000) {
                /*
                 * ISO/IEC 11172-3 does not allow single channel mode for 224, 256,
                 * 320, or 384 kbps bitrates in Layer II.
                 */
                p_stream->error = AW_MP3_DEC_ERR_BADMODE;
                return AW_ERROR;
            }
        }

        if (bitrate_per_channel <= 48000) {
            index = (p_header->sample_rate == 32000) ? 3 : 2;
        } else if (bitrate_per_channel <= 80000) {
            index = 0;
        } else {
__free_format:
            index = (p_header->sample_rate == 48000) ? 0 : 1;
        }
    }

    sblimit   = __g_layer_ii_sbquant_table[index].sblimit;
    p_offsets = __g_layer_ii_sbquant_table[index].offsets;

    bound = 32;
    if (p_header->mode == AW_MP3_DEC_FRAME_MODE_JOINT_STEREO) {
        p_header->flags |= AW_MP3_DEC_FRAME_FLAG_I_STEREO;
        bound = 4 + p_header->mode_extension * 4;
    }

    if (bound > sblimit) {
        bound = sblimit;
    }

    start = p_stream->ptr;

    /* decode bit allocations */
    for (sb = 0; sb < bound; ++sb) {
        nbal = __g_layer_ii_bitalloc_table[p_offsets[sb]].nbal;

        for (ch = 0; ch < nch; ++ch) {
            allocation[ch][sb] = aw_mp3_dec_bit_read(&p_stream->ptr, nbal);
        }
    }

    for (sb = bound; sb < sblimit; ++sb) {
        nbal = __g_layer_ii_bitalloc_table[p_offsets[sb]].nbal;

        allocation[0][sb] = allocation[1][sb] =
            aw_mp3_dec_bit_read(&p_stream->ptr, nbal);
    }

    /* decode scalefactor selection info */
    for (sb = 0; sb < sblimit; ++sb) {
        for (ch = 0; ch < nch; ++ch) {
            if (allocation[ch][sb]) {
                scfsi[ch][sb] = aw_mp3_dec_bit_read(&p_stream->ptr, 2);
            }
        }
    }

    /* check CRC word */
    if (p_header->flags & AW_MP3_DEC_FRAME_FLAG_PROTECTION) {
        p_header->crc_check = aw_mp3_dec_bit_crc(
                                  start,
                                  aw_mp3_dec_bit_len_get(&start,
                                                         &p_stream->ptr),
                                  p_header->crc_check);

        if (p_header->crc_check != p_header->crc_target &&
            !(p_frame->options & AW_MP3_DEC_OPTION_IGNORECRC)) {
            p_stream->error = AW_MP3_DEC_ERR_BADCRC;
            return AW_ERROR;
        }
    }

    /* decode scalefactors */
    for (sb = 0; sb < sblimit; ++sb) {
        for (ch = 0; ch < nch; ++ch) {
            if (allocation[ch][sb]) {
                scale_factor[ch][sb][0] = aw_mp3_dec_bit_read(&p_stream->ptr,
                                                              6);

                switch (scfsi[ch][sb]) {

                case 2:
                    scale_factor[ch][sb][2] =
                    scale_factor[ch][sb][1] =
                    scale_factor[ch][sb][0];
                    break;

                case 0:
                    scale_factor[ch][sb][1] = aw_mp3_dec_bit_read(
                                                  &p_stream->ptr,
                                                  6);
                    /* no break */

                /* fall through */
                case 1:
                case 3:
                    scale_factor[ch][sb][2] = aw_mp3_dec_bit_read(
                                                  &p_stream->ptr,
                                                  6);
                    break;

                default:
                    break;
                }

                if (scfsi[ch][sb] & 1) {
                    scale_factor[ch][sb][1] =
                    scale_factor[ch][sb][scfsi[ch][sb] - 1];
                }

#if (AW_MP3_DEC_OPT_STRICT)

                /*
                 * Scalefactor index 63 does not appear in Table B.1 of
                 * ISO/IEC 11172-3. Nonetheless, other implementations accept it,
                 * so we only reject it if OPT_STRICT is defined.
                 */
                if (scale_factor[ch][sb][0] == 63 ||
                    scale_factor[ch][sb][1] == 63 ||
                    scale_factor[ch][sb][2] == 63) {
                    p_stream->error = AW_MP3_DEC_ERR_BADSCALEFACTOR;
                    return AW_ERROR;
                }
#endif
            }
        }
    }

    /* decode samples */
    for (gr = 0; gr < 12; ++gr) {
        for (sb = 0; sb < bound; ++sb) {
            for (ch = 0; ch < nch; ++ch) {
                if ((index = allocation[ch][sb]) != 0) {

                    index = __g_layer_ii_offset_table[
                                __g_layer_ii_bitalloc_table[
                                    p_offsets[sb]].offset][index - 1];

                    __layer_ii_samples(&p_stream->ptr,
                                       &g_mp3_dec_qc_table[index],
                                       samples);

                    for (s = 0; s < 3; ++s) {
                        p_frame->sbsample[ch][3 * gr + s][sb] =
                            AW_MP3_F_MUL(
                                samples[s],
                                g_mp3_dec_sf_table[
                                    scale_factor[ch][sb][gr / 4]]);
                    }
                } else {
                    for (s = 0; s < 3; ++s) {
                        p_frame->sbsample[ch][3 * gr + s][sb] = 0;
                    }
                }
            }
        }

        for (sb = bound; sb < sblimit; ++sb) {

            if ((index = allocation[0][sb]) != 0) {
                index = __g_layer_ii_offset_table[
                            __g_layer_ii_bitalloc_table[p_offsets[sb]].offset]
                                                       [index - 1];

                __layer_ii_samples(&p_stream->ptr,
                                   &g_mp3_dec_qc_table[index],
                                   samples);

                for (ch = 0; ch < nch; ++ch) {
                    for (s = 0; s < 3; ++s) {
                        p_frame->sbsample[ch][3 * gr + s][sb] =
                            AW_MP3_F_MUL(samples[s],
                                         g_mp3_dec_sf_table[
                                             scale_factor[ch][sb][gr / 4]]);
                    }
                }
            } else {
                for (ch = 0; ch < nch; ++ch) {
                    for (s = 0; s < 3; ++s) {
                        p_frame->sbsample[ch][3 * gr + s][sb] = 0;
                    }
                }
            }
        }

        for (ch = 0; ch < nch; ++ch) {
            for (s = 0; s < 3; ++s) {
                for (sb = sblimit; sb < 32; ++sb) {
                    p_frame->sbsample[ch][3 * gr + s][sb] = 0;
                }
            }
        }
    }

    return AW_OK;
}

/**
 * \brief Layer III ½âÂë
 */
aw_err_t aw_mp3_dec_layer_iii_decode (struct aw_mp3_dec_stream *p_stream,
                                      struct aw_mp3_dec_frame  *p_frame)
{
    struct aw_mp3_dec_frame_header *p_header      = &p_frame->header;
    uint32_t                        nch;
    uint32_t                        priv_bit_len;
    uint32_t                        next_md_begin = 0;
    uint32_t                        si_len;
    uint32_t                        data_bit_len;
    uint32_t                        md_len;
    uint32_t                        frame_space;
    uint32_t                        frame_used;
    uint32_t                        frame_free;
    struct aw_mp3_dec_bit           ptr;
    struct __layer_iii_side_info    si;
    enum aw_mp3_dec_err             error;
    int                             result        = 0;

    nch    = AW_MP3_DEC_FRAME_NCHANNELS(&p_frame->header);
    si_len = (p_header->flags & AW_MP3_DEC_FRAME_FLAG_LSF_EXT) ?
             (nch == 1 ? 9 : 17) : (nch == 1 ? 17 : 32);

    /* check frame sanity */
    if (p_stream->pos_next_frame - aw_mp3_dec_bit_next_byte(&p_stream->ptr) <
        (int32_t)si_len) {
        p_stream->error  = AW_MP3_DEC_ERR_BADFRAMELEN;
        p_stream->md_len = 0;
        return AW_ERROR;
    }

    /* check CRC word */
    if (p_header->flags & AW_MP3_DEC_FRAME_FLAG_PROTECTION) {

        p_header->crc_check = aw_mp3_dec_bit_crc(p_stream->ptr,
                                                 si_len * AW_MP3_DEC_CHAR_BIT,
                                                 p_header->crc_check);

        if (p_header->crc_check != p_header->crc_target &&
            !(p_frame->options & AW_MP3_DEC_OPTION_IGNORECRC)) {
            p_stream->error = AW_MP3_DEC_ERR_BADCRC;
            result = AW_ERROR;
        }
    }

    /* decode frame side information */
    error = __layer_iii_side_info(&p_stream->ptr,
                                  nch,
                                  (p_header->flags &
                                   AW_MP3_DEC_FRAME_FLAG_LSF_EXT),
                                  &si,
                                  &data_bit_len,
                                  &priv_bit_len);
    if (error && result == 0) {
        p_stream->error = error;
        result          = AW_ERROR;
    }

    p_header->flags        |= priv_bit_len;
    p_header->private_bits |= si.private_bits;

    /* find main_data of next frame */
    {
        struct aw_mp3_dec_bit peek;
        uint32_t              header;

        aw_mp3_dec_bit_init(&peek,
                            AW_MP3_DEC_BIT_TYPE_RNGBUF,
                            &p_stream->rngbuf,
                            p_stream->pos_next_frame);

        header = aw_mp3_dec_bit_read(&peek, 32);

        /* syncword | layer */
        if ((header & 0xffe60000L) == 0xffe20000L) {

            /* protection_bit */
            if (!(header & 0x00010000L)) {
                aw_mp3_dec_bit_skip(&peek, 16);  /* crc_check */
            }

            /* ID */
            next_md_begin = aw_mp3_dec_bit_read(&peek,
                                                (header & 0x00080000L) ? 9 : 8);
        }

        //aw_mp3_dec_bit_finish(&peek);
    }

    /* find main_data of this frame */
    frame_space = p_stream->pos_next_frame -
                  aw_mp3_dec_bit_next_byte(&p_stream->ptr);

    if (next_md_begin > si.main_data_begin + frame_space) {
        next_md_begin = 0;
    }

    md_len = si.main_data_begin + frame_space - next_md_begin;

    frame_used = 0;

    if (si.main_data_begin == 0) {
        ptr              = p_stream->ptr;
        p_stream->md_len = 0;

        frame_used = md_len;
    } else {
        if (si.main_data_begin > p_stream->md_len) {
            if (result == 0) {
                p_stream->error = AW_MP3_DEC_ERR_BADDATAPTR;
                result          = AW_ERROR;
            }
        } else {

            aw_mp3_dec_bit_init(&ptr,
                                AW_MP3_DEC_BIT_TYPE_BUF,
                                &p_stream->md_buf,
                                p_stream->md_len -
                                si.main_data_begin);

            if (md_len > si.main_data_begin) {
                aw_assert(p_stream->md_len + md_len -
                          si.main_data_begin <= AW_MP3_DEC_BUFFER_MD_LEN);

                aw_mp3_rngbuf_lseek(&p_stream->rngbuf,
                                    aw_mp3_dec_bit_next_byte(&p_stream->ptr));
                aw_mp3_rngbuf_read(&p_stream->rngbuf,
                                   p_stream->md + p_stream->md_len,
                                  (frame_used = md_len - si.main_data_begin));
                p_stream->md_len += frame_used;
            }
        }
    }

    frame_free = frame_space - frame_used;

    /* decode main_data */
    if (result == 0) {

        error = __layer_iii_decode(&ptr, p_frame, &si, nch);
        if (error != AW_MP3_DEC_ERR_NONE) {
            p_stream->error = error;
            result          = AW_ERROR;
        }

        /* designate ancillary bits */
        p_stream->anc_ptr    = ptr;
        p_stream->anc_bitlen = md_len * AW_MP3_DEC_CHAR_BIT - data_bit_len;
    }

    AW_DBGF(("main_data_begin:%u, md_len:%u, frame_free:%u, "
            "data_bitlen:%u, anc_bitlen: %u\n",
            si.main_data_begin, md_len, frame_free,
            data_bitlen, stream->anc_bitlen));

    /* preload main_data buffer with up to 511 bytes for next frame(s) */
    if (frame_free >= next_md_begin) {
        aw_mp3_rngbuf_lseek(&p_stream->rngbuf,
                            p_stream->pos_next_frame - next_md_begin);
        aw_mp3_rngbuf_read(&p_stream->rngbuf, p_stream->md, next_md_begin);

        p_stream->md_len = next_md_begin;
    } else {
        if (md_len < si.main_data_begin) {
            uint32_t extra;

            extra = si.main_data_begin - md_len;
            if (extra + frame_free > next_md_begin) {
                extra = next_md_begin - frame_free;
            }

            if (extra < p_stream->md_len) {
                memmove(p_stream->md,
                        p_stream->md + p_stream->md_len - extra,
                        extra);
                p_stream->md_len = extra;
            }
        } else {
            p_stream->md_len = 0;
        }

        aw_mp3_rngbuf_lseek(&p_stream->rngbuf,
                            p_stream->pos_next_frame - frame_free);
        aw_mp3_rngbuf_read(&p_stream->rngbuf,
                           p_stream->md + p_stream->md_len,
                           frame_free);
        p_stream->md_len += frame_free;
    }

    return result;
}

#if !(AW_MP3_DEC_ASO_IMDCT)

/**
 * \brief perform IMDCT and windowing for long blocks
 */
void aw_mp3_dec_layer_iii_imdct_l (const int32_t x[18],
                                   int32_t       z[36],
                                   uint32_t      block_type)
{
    uint32_t i;

    /* IMDCT */
    __layer_iii_imdct36(x, z);

    /* windowing */
    switch (block_type) {

    case 0:  /* normal window */
#if (AW_MP3_DEC_ASO_INTERLEAVE1)
    {
        register int32_t tmp1, tmp2;

        tmp1 = g_mp3_dec_window_l[0];
        tmp2 = g_mp3_dec_window_l[1];

        for (i = 0; i < 34; i += 2) {
            z[i + 0] = AW_MP3_F_MUL(z[i + 0], tmp1);
            tmp1 = g_mp3_dec_window_l[i + 2];
            z[i + 1] = AW_MP3_F_MUL(z[i + 1], tmp2);
            tmp2 = g_mp3_dec_window_l[i + 3];
        }

        z[34] = AW_MP3_F_MUL(z[34], tmp1);
        z[35] = AW_MP3_F_MUL(z[35], tmp2);
    }
#elif (AW_MP3_DEC_ASO_INTERLEAVE2)
    {
        register int32_t tmp1, tmp2;

        tmp1 = z[0];
        tmp2 = g_mp3_dec_window_l[0];

        for (i = 0; i < 35; ++i) {
            z[i] = AW_MP3_F_MUL(tmp1, tmp2);
            tmp1 = z[i + 1];
            tmp2 = g_mp3_dec_window_l[i + 1];
        }

        z[35] = AW_MP3_F_MUL(tmp1, tmp2);
    }
#elif 1
        for (i = 0; i < 36; i += 4) {
            z[i + 0] = AW_MP3_F_MUL(z[i + 0], g_mp3_dec_window_l[i + 0]);
            z[i + 1] = AW_MP3_F_MUL(z[i + 1], g_mp3_dec_window_l[i + 1]);
            z[i + 2] = AW_MP3_F_MUL(z[i + 2], g_mp3_dec_window_l[i + 2]);
            z[i + 3] = AW_MP3_F_MUL(z[i + 3], g_mp3_dec_window_l[i + 3]);
        }
#else
        for (i =  0; i < 36; ++i) {
            z[i] = AW_MP3_F_MUL(z[i], g_mp3_dec_window_l[i]);
        }
#endif
        break;

    case 1:  /* start block */

        for (i =  0; i < 18; i += 3) {
            z[i + 0] = AW_MP3_F_MUL(z[i + 0], g_mp3_dec_window_l[i + 0]);
            z[i + 1] = AW_MP3_F_MUL(z[i + 1], g_mp3_dec_window_l[i + 1]);
            z[i + 2] = AW_MP3_F_MUL(z[i + 2], g_mp3_dec_window_l[i + 2]);
        }

        /*  (i = 18; i < 24; ++i) z[i] unchanged */
        for (i = 24; i < 30; ++i) {
            z[i] = AW_MP3_F_MUL(z[i], g_mp3_dec_window_s[i - 18]);
        }

        for (i = 30; i < 36; ++i) {
            z[i] = 0;
        }
        break;

    case 3:  /* stop block */

        for (i =  0; i <  6; ++i) {
            z[i] = 0;
        }

        for (i =  6; i < 12; ++i) {
            z[i] = AW_MP3_F_MUL(z[i], g_mp3_dec_window_s[i - 6]);
        }

        /*  (i = 12; i < 18; ++i) z[i] unchanged */
        for (i = 18; i < 36; i += 3) {
            z[i + 0] = AW_MP3_F_MUL(z[i + 0], g_mp3_dec_window_l[i + 0]);
            z[i + 1] = AW_MP3_F_MUL(z[i + 1], g_mp3_dec_window_l[i + 1]);
            z[i + 2] = AW_MP3_F_MUL(z[i + 2], g_mp3_dec_window_l[i + 2]);
        }
        break;
    }
}

#endif

/* end of file */
