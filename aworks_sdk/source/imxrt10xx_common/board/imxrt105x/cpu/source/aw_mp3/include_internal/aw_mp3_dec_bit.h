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

#ifndef __AW_MP3_DEC_BIT_H
#define __AW_MP3_DEC_BIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_mp3_internal.h"

/**
 * \brief 位类型
 */
enum aw_mp3_dec_bit_type {
    AW_MP3_DEC_BIT_TYPE_BUF,
    AW_MP3_DEC_BIT_TYPE_RNGBUF,
};

/**
 * \brief 位信息
 */
struct aw_mp3_dec_bit {
    //const uint8_t *p_byte;
    void                     *p_buf;
    enum aw_mp3_dec_bit_type  type;
    uint32_t                  pos;
    uint8_t                   cache;
    uint16_t                  left;
};

/**
 * \brief 初始化位信息
 *
 * \param[in] p_bit  : 位信息结构体
 * \param[in] p_byte : 字节数组
 *
 * \return 无
 */
void aw_mp3_dec_bit_init (struct aw_mp3_dec_bit    *p_bit,
                          enum aw_mp3_dec_bit_type  type,
                          void                     *p_buf,
                          uint32_t                  pos);

/**
 * \brief 获取长度
 *
 * \param[in] p_bit  : 开始位信息结构体
 * \param[in] p_bit  : 结束位信息结构体
 *
 * \return 位长度
 */
uint32_t aw_mp3_dec_bit_len_get (const struct aw_mp3_dec_bit *p_begin,
                                 struct aw_mp3_dec_bit       *p_end);

/**
 * \brief 返回剩余长度
 * \param[in] p_bit  : 位信息结构体
 * \return 剩余长度
 */
aw_static_inline
uint16_t aw_mp3_dec_bit_left_get (const struct aw_mp3_dec_bit *p_bit)
{
    return p_bit->left;
}

/**
 * \brief 下一个字节索引
 * \param[in] p_bit  : 位信息结构体
 * \return 下一个字节索引
 */
uint32_t aw_mp3_dec_bit_next_byte (const struct aw_mp3_dec_bit *p_bit);

/**
 * \brief 跳过固定长度的数据
 *
 * \param[in] p_bit  : 位信息结构体
 * \param[in] len    : 长度（字节）
 *
 * \return 无
 */
void aw_mp3_dec_bit_skip (struct aw_mp3_dec_bit *p_bit, uint32_t len);

/**
 * \brief 读取数据
 *
 * \param[in] p_bit  : 位信息结构体
 * \param[in] len    : 长度（字节）
 *
 * \return 读取的数据长度
 */
uint32_t aw_mp3_dec_bit_read (struct aw_mp3_dec_bit *p_bit, uint32_t len);

/**
 * \brief CRC
 *
 * \param[in] p_bit  : 位信息结构体
 * \param[in] len    : 长度（字节）
 * \param[in] init   : 初始值
 *
 * \return CRC
 */
uint16_t aw_mp3_dec_bit_crc (struct aw_mp3_dec_bit  bit,
                             uint32_t               len,
                             uint16_t               init);

#ifdef __cplusplus
}
#endif

#endif /* __AW_MP3_DEC_BIT_H */

/* end of file */
