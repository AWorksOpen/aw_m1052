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
 * \brief λ����
 */
enum aw_mp3_dec_bit_type {
    AW_MP3_DEC_BIT_TYPE_BUF,
    AW_MP3_DEC_BIT_TYPE_RNGBUF,
};

/**
 * \brief λ��Ϣ
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
 * \brief ��ʼ��λ��Ϣ
 *
 * \param[in] p_bit  : λ��Ϣ�ṹ��
 * \param[in] p_byte : �ֽ�����
 *
 * \return ��
 */
void aw_mp3_dec_bit_init (struct aw_mp3_dec_bit    *p_bit,
                          enum aw_mp3_dec_bit_type  type,
                          void                     *p_buf,
                          uint32_t                  pos);

/**
 * \brief ��ȡ����
 *
 * \param[in] p_bit  : ��ʼλ��Ϣ�ṹ��
 * \param[in] p_bit  : ����λ��Ϣ�ṹ��
 *
 * \return λ����
 */
uint32_t aw_mp3_dec_bit_len_get (const struct aw_mp3_dec_bit *p_begin,
                                 struct aw_mp3_dec_bit       *p_end);

/**
 * \brief ����ʣ�೤��
 * \param[in] p_bit  : λ��Ϣ�ṹ��
 * \return ʣ�೤��
 */
aw_static_inline
uint16_t aw_mp3_dec_bit_left_get (const struct aw_mp3_dec_bit *p_bit)
{
    return p_bit->left;
}

/**
 * \brief ��һ���ֽ�����
 * \param[in] p_bit  : λ��Ϣ�ṹ��
 * \return ��һ���ֽ�����
 */
uint32_t aw_mp3_dec_bit_next_byte (const struct aw_mp3_dec_bit *p_bit);

/**
 * \brief �����̶����ȵ�����
 *
 * \param[in] p_bit  : λ��Ϣ�ṹ��
 * \param[in] len    : ���ȣ��ֽڣ�
 *
 * \return ��
 */
void aw_mp3_dec_bit_skip (struct aw_mp3_dec_bit *p_bit, uint32_t len);

/**
 * \brief ��ȡ����
 *
 * \param[in] p_bit  : λ��Ϣ�ṹ��
 * \param[in] len    : ���ȣ��ֽڣ�
 *
 * \return ��ȡ�����ݳ���
 */
uint32_t aw_mp3_dec_bit_read (struct aw_mp3_dec_bit *p_bit, uint32_t len);

/**
 * \brief CRC
 *
 * \param[in] p_bit  : λ��Ϣ�ṹ��
 * \param[in] len    : ���ȣ��ֽڣ�
 * \param[in] init   : ��ʼֵ
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
