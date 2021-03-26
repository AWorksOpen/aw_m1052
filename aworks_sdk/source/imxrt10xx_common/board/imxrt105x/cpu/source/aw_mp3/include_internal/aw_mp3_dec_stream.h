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

#ifndef __AW_MP3_DEC_STREAM_H
#define __AW_MP3_DEC_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_mp3_internal.h"

/**
 * \brief ������
 */
struct aw_mp3_dec_stream {
    struct aw_mp3_rngbuf   rngbuf;          /**< \brief buffer */

    uint32_t               pos_buf_fer;    /**< \brief input bitstream buffer */
    uint32_t               pos_buf_end;    /**< \brief end of buffer */

    /** \brief bytes to skip before next frame */
    uint32_t               skip_len;

    int                    sync;           /**< \brief stream sync found */
    uint32_t               free_rate;      /**< \brief free bitrate (fixed) */

    uint32_t               pos_this_frame; /**< \brief start of current frame */
    uint32_t               pos_next_frame; /**< \brief start of next frame */

    /** \brief current processing bit pointer */
    struct aw_mp3_dec_bit  ptr;

    /** \brief ancillary bits pointer */
    struct aw_mp3_dec_bit  anc_ptr;

    /** \brief number of ancillary bits */
    uint32_t               anc_bitlen;

    /** \brief Layer III main_data() */
    uint8_t                md[AW_MP3_DEC_BUFFER_MD_LEN];

    /** \brief md_buf */
    aw_mp3_buf_t           md_buf;

    /** \brief bytes in main_data */
    uint32_t               md_len;

    /** \brief decoding options (see below) */
    int                    options;

    /** \brief error code (see above) */
    enum aw_mp3_dec_err    error;
};

/**
 * \brief ����ʼ��
 * \param[in] p_stream : ���ṹ��
 * \return ��
 */
void aw_mp3_dec_stream_init (struct aw_mp3_dec_stream *p_stream);

/**
 * \brief ����options
 *
 * \param[in] p_stream : ���ṹ��
 * \param[in] options  : ѡ��
 *
 * \return ��
 */
aw_static_inline
void aw_mp3_dec_stream_options_set (struct aw_mp3_dec_stream *p_stream,
                                    int                       options)
{
    p_stream->options = options;
}

/**
 * \brief ������������
 *
 * \param[in] p_stream : ���ṹ��
 * \param[in] p_buffer : ������
 * \param[in] len      : ��������С
 *
 * \return ��
 */
void aw_mp3_dec_stream_buffer_set (struct aw_mp3_dec_stream *p_stream,
                                   aw_mp3_buf_t             *p_buffer,
                                   uint32_t                  pos,
                                   uint32_t                  len);

/**
 * \brief �����̶�����
 *
 * \param[in] p_stream : ���ṹ��
 * \param[in] len      : ����
 *
 * \return ��
 */
void aw_mp3_dec_stream_skip (struct aw_mp3_dec_stream *p_stream,
                             uint32_t                  len);

/**
 * \brief ��λ��һ��ͬ����
 * \param[in] p_stream : ���ṹ��
 * \return AW_OK��ʾͬ���ɹ�
 */
aw_err_t aw_mp3_dec_stream_sync (struct aw_mp3_dec_stream *p_stream);

#ifdef __cplusplus
}
#endif

#endif /* __AW_MP3_DEC_STREAM_H */

/* end of file */
