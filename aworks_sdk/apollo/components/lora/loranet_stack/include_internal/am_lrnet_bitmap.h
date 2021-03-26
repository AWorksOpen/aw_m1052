/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief   LoRaNet Bitmap Module Interface
 *
 * \internal
 * \par modification history:
 * - 2017-05-23 ebi, first implementation.
 * \endinternal
 */

#ifndef __AM_LRNET_BITMAP_H
#define __AM_LRNET_BITMAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_lrnet_internal.h"

/**
 * \brief   bitmap base class
 */
typedef struct {
    uint8_t size;
    uint8_t data[1];
} am_lrnet_bitmap_t;

/**
 * \brief sub-class of bitmap
 */
typedef struct {
    uint8_t size;
    uint8_t data[8];
} am_lrnet_bitmap64_t;

/**
 * \brief sub-class of bitmap
 */
typedef struct {
    uint8_t size;
    uint8_t data[18];
} am_lrnet_bitmap144_t;

/**
 * \brief sub-class of bitmap
 */
typedef struct {
    uint8_t size;
    uint8_t data[32];
} am_lrnet_bitmap256_t;

/**
 * \brief sub-class of bitmap256
 */
typedef struct {
    am_lrnet_bitmap256_t bitmap;
    uint8_t           addr_msb;
} am_lrnet_bitmap_addr_t;

#define BITMAP_CAST(p_bitmap_sub)     ((am_lrnet_bitmap_t*) p_bitmap_sub)

void am_lrnet_bitmap256_init (am_lrnet_bitmap256_t *p_bitmap256,
                              uint8_t              *p_bitmap_data);

void am_lrnet_bitmap144_init (am_lrnet_bitmap144_t *p_bitmap144,
                              uint8_t              *p_bitmap_data);

void am_lrnet_bitmap64_init (am_lrnet_bitmap64_t *p_bitmap64,
                             uint8_t             *p_bitmap_data);

am_lora_static_inline
uint8_t am_lrnet_bitmap_get_raw (const uint8_t *p_bitmap_raw, uint8_t n)
{
    return p_bitmap_raw[n/8] & (1 << (n%8));
}

am_lora_static_inline
uint8_t am_lrnet_bitmap_get (const am_lrnet_bitmap_t *p_bitmap, uint8_t n)
{
    return am_lrnet_bitmap_get_raw(p_bitmap->data, n);
}

am_lora_static_inline
void am_lrnet_bitmap_set_raw (uint8_t *p_bitmap_raw, uint8_t n)
{
    p_bitmap_raw[n/8] |= (1 << (n%8));
}

am_lora_static_inline
void am_lrnet_bitmap_set (am_lrnet_bitmap_t *p_bitmap, uint8_t n)
{
    am_lrnet_bitmap_set_raw(p_bitmap->data, n);
}

int am_lrnet_bitmap_count (const am_lrnet_bitmap_t *p_bitmap);

int am_lrnet_bitmap_bit_count_before (const am_lrnet_bitmap_t *p_bitmap,
                                      uint8_t                  index);

am_lora_static_inline
void am_lrnet_bitmap_addr_init (am_lrnet_bitmap_addr_t *p_bitmap_addr,
                                uint8_t                *p_bitmap_data,
                                uint8_t                 addr_msb)
{
    am_lrnet_bitmap256_init(&p_bitmap_addr->bitmap, p_bitmap_data);
    p_bitmap_addr->addr_msb = addr_msb;
}

int am_lrnet_bitmap_addr_from_list (am_lrnet_bitmap_addr_t  *p_bitmap_addr,
                                    const uint16_t          *p_dev_list,
                                    int                      dev_num);

void am_lrnet_bitmap_raw_addr_foreach (
         uint8_t  *p_bitmap_data,
         uint8_t   bitmap_size,
         uint8_t   addr_msb,
         void      pfn_cb (uint16_t addr, uint16_t idx, void *p_arg),
         void     *p_cb_arg);

void am_lrnet_bitmap_addr_foreach (
        am_lrnet_bitmap_addr_t  *p_bitmap_addr,
        void                     pfn_cb (uint16_t addr, uint16_t idx, void *p_arg),
        void                    *p_cb_arg);

int am_lrnet_bitmap_addr_set (am_lrnet_bitmap_addr_t  *p_bitmap_addr,
                              uint16_t                 addr);

am_lora_static_inline
void am_lrnet_bitmap_addr_clear (am_lrnet_bitmap_addr_t  *p_bitmap_addr)
{
    p_bitmap_addr->addr_msb = 0x00;
    am_lrnet_bitmap256_init(&p_bitmap_addr->bitmap, NULL);
}

/**
 * \brief       get index of specified add_lsb in the address list
 * \return      index of addr, negative error code on failed.
 */
int am_lrnet_bitmap_addr_idx_get (am_lrnet_bitmap_addr_t  *p_bitmap_addr,
                                  uint16_t                 addr);

#ifdef __cplusplus
}
#endif

#endif /* __AM_LRNET_BITMAP_H */

/* end of file */
