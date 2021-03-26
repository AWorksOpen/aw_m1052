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

#ifndef __AW_MP3_DEC_LAYER_H
#define __AW_MP3_DEC_LAYER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_mp3_internal.h"

/**
 * \brief Layer I ����
 *
 * \param[in] p_stream : ���ṹ��
 * \param[in] p_frame  : ֡�ṹ��
 *
 * \return ����AW_OK��ʾ�ɹ�������ֵ��ʾʧ��
 */
aw_err_t aw_mp3_dec_layer_i_decode (struct aw_mp3_dec_stream *p_stream,
                                    struct aw_mp3_dec_frame  *p_frame);

/**
 * \brief Layer II ����
 *
 * \param[in] p_stream : ���ṹ��
 * \param[in] p_frame  : ֡�ṹ��
 *
 * \return ����AW_OK��ʾ�ɹ�������ֵ��ʾʧ��
 */
aw_err_t aw_mp3_dec_layer_ii_decode (struct aw_mp3_dec_stream *p_stream,
                                     struct aw_mp3_dec_frame  *p_frame);

/**
 * \brief Layer III ����
 *
 * \param[in] p_stream : ���ṹ��
 * \param[in] p_frame  : ֡�ṹ��
 *
 * \return ����AW_OK��ʾ�ɹ�������ֵ��ʾʧ��
 */
aw_err_t aw_mp3_dec_layer_iii_decode (struct aw_mp3_dec_stream *p_stream,
                                      struct aw_mp3_dec_frame  *p_frame);

/**
 * \brief perform IMDCT and windowing for long blocks
 */
void aw_mp3_dec_layer_iii_imdct_l (const int32_t x[18],
                                   int32_t       z[36],
                                   uint32_t      block_type);

#ifdef __cplusplus
}
#endif

#endif /* __AW_MP3_DEC_LAYER_H */

/* end of file */
