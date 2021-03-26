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
 * - 1.00 20-10-16  win, first implementation.
 * \endinternal
 */

#ifndef __AW_MP3_HUFFMAN_H
#define __AW_MP3_HUFFMAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_mp3_internal.h"

/**
 * \bief huffman quad
 */
union aw_mp3_huffman_quad {

    struct {
        uint16_t final;    /**< \brief 1bit */
        uint16_t bits;     /**< \brief 3bit */
        uint16_t offset;   /**< \brief 12bit */
    } ptr;

    struct {
        uint16_t final;    /**< \brief 1bit */
        uint16_t hlen;     /**< \brief 3bit */
        uint16_t v;        /**< \brief 1bit */
        uint16_t w;        /**< \brief 1bit */
        uint16_t x;        /**< \brief 1bit */
        uint16_t y;        /**< \brief 1bit */
    } value;

    uint16_t final;        /**< \brief 1bit */
};

/**
 * \brief huffman pair
 */
union aw_mp3_huffman_pair {

    struct {
        uint16_t final;    /**< \brief 1bit */
        uint16_t bits;     /**< \brief 3bit */
        uint16_t offset;   /**< \brief 12bit */
    } ptr;

    struct {
        uint16_t final;    /**< \brief 1bit */
        uint16_t hlen;     /**< \brief 3bit */
        uint16_t x;        /**< \brief 4bit */
        uint16_t y;        /**< \brief 4bit */
    } value;

    uint16_t final;        /**< \brief 1bit */
};

/**
 * \brief huffman table
 */
struct aw_mp3_huffman_table {
    const union aw_mp3_huffman_pair *p_table;
    uint16_t                         line_bits;
    uint16_t                         start_bits;
};

extern
const union aw_mp3_huffman_quad * const g_mp3_huffman_quad_table[2];

extern
const struct aw_mp3_huffman_table g_mp3_huffman_pair_table[32];

#ifdef __cplusplus
}
#endif

#endif /* __AW_MP3_HUFFMAN_H */

/* end of file */
