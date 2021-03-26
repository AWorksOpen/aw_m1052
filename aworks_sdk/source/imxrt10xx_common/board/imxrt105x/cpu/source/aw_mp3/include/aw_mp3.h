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

#ifndef __AW_MP3_DECODE_H
#define __AW_MP3_DECODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_types.h"
#include "aw_common.h"

/**
 * \brief MP3������ʵ��
 */
typedef void *aw_mp3_decode_t;

/**
 * \brief MP3��������Ϣ
 */
struct aw_mp3_stream_info {

    const char  *p_title;      /**< \brief ��������  */
    const char  *p_artist;     /**< \brief ���������� */
    const char  *p_album;      /**< \brief ר�� */
    int          year;         /**< \brief ������� */
    const char  *p_category;   /**< \brief �������� */
    int          track_m;      /**< \brief ����M����ʾר���и������� */
    int          track_n;      /**< \brief ����N����ʾר���еĵڼ��׸� */
    const char  *p_comment;    /**< \brief ��ע */

    uint8_t      quality;      /**< \brief ��Ƶ������0~100������ֵԽ��Խ�� */

    uint32_t     stream_size;  /**< \brief ��Ƶ���ݴ�С����λ���ֽڣ� */
    uint32_t     frame_num;    /**< \brief ��Ƶ֡�� */

    uint32_t     total_ms;     /**< \brief ������ʱ�䣨��λ���룩*/

    uint32_t     bit_rate;     /**< \brief �����ʣ���λ:bps�� */
    uint32_t     sample_rate;  /**< \brief ������ ����λ��Hz��*/

    uint8_t      sample_bit;   /**< \brief ����λ�� */

    /**
     * \brief ���������(��λ���ֽ�)
     * \note ���Ϊ˫ͨ����������洢����Ϊ ������ + ������ + ������ + ������ +... + ������ + ��������
     *       Ӧ�����ṩout_samples�ֽڵĿռ����ڴ�Ž��������ݡ�
     * \code
     *  aw_mp3_decode_t           decode;
     *  struct aw_mp3_stream_info info;
     *
     *  extern const uint8_t g_mp3_data[];
     *  extern const uint8_t g_mp3_data_len;
     *
     *  uint8_t *p_buffer;
     *
     *  decode = aw_mp3_decode_open();
     *  if (decode == NULL) {
     *      // ��ʧ��
     *      return;
     *  }
     *
     *  aw_mp3_decode_stream_mem_load(decode, g_mp3_data, g_mp3_data_len);
     *  aw_mp3_decode_stream_info_get(decode, &info);
     *
     *  p_buffer = aw_mem_align(info.out_samples, AW_CACHE_LINE_SIZE);
     *  if (p_buffer == NULL) {
     *      // ����ʧ��
     *      return;
     *  }
     *
     *  aw_mp3_decode_stream_frame(decode, p_buffer, info.out_samples, NULL);
     * \endcode
     */
    uint32_t     out_samples;

    /**
     * \brief ͨ����
     * \note ���Ϊ1�����ʾ�����������Ϊ2����Ϊ˫����
     */
    uint8_t      channels;

    /**
     * \brief ÿ֡������
     * \attention �����������ڽ������ݵ��ֽ���
     */
    uint32_t     samples_per_frame;
};

/** \brief ���� */
#define AW_MP3_TIME_RESOLUTION    (352800000UL)

/**
 * \brief ʱ�䵥λ
 */
struct aw_mp3_time {
    int seconds;    /** \brief �� */
    int fraction;   /** \brief 1/AW_MP3_TIME_RESOLUTION �� */
};

/**
 * \brief ��MP3������
 * \note ÿ�ε��øýӿڣ�������һ���µĽ�����ʵ����Ϊ�˱�֤�ڴ治й©����Ҫ����aw_mp3_decode_close()�رս�������
 * \return ����ɹ����ؽ�����ʵ����ʧ���򷵻�NULL
 */
aw_mp3_decode_t aw_mp3_decode_open (void);

/**
 * \brief ���ý�����λ��
 * \attention ��ǰ��֧��8λ��16λ��24λ��32λ��Ĭ�������Ϊ16λ��
 *
 * \param[in] decode : ������ʵ��
 * \param[in] bits   : λ��
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ��������
 */
aw_err_t aw_mp3_decode_stream_bits_set (aw_mp3_decode_t decode,
                                        uint8_t         bits);

/**
 * \brief ��������������������
 *
 * \param[in] decode : ������ʵ��
 * \param[in] p_data : ����
 * \param[in] len    : ���ݳ���
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_ENOMEM : �ڴ�ռ䲻��
 * \retval -AW_EINVAL : ��������
 */
aw_err_t aw_mp3_decode_stream_mem_load (aw_mp3_decode_t  decode,
                                        const uint8_t   *p_data,
                                        uint32_t         len);

/**
 * \brief �����ļ�������������
 *
 * \param[in] decode     : ������ʵ��
 * \param[in] p_filename : �ļ�����
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_ENOMEM : �ڴ�ռ䲻��
 * \retval -AW_EBADF  : �ļ���ʧ��
 * \retval -AW_EINVAL : ��������
 */
aw_err_t aw_mp3_decode_stream_file_load (aw_mp3_decode_t  decode,
                                         const char      *p_filename);

/**
 * \brief ж�ؽ������У��ļ�/���ݣ���
 * \param[in] decode : ������ʵ��
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ��������
 */
aw_err_t aw_mp3_decode_stream_unload (aw_mp3_decode_t decode);

/**
 * \brief ��ȡ�������У��ļ�/���ݣ�����Ϣ
 *
 * \param[in]  decode : ������ʵ��
 * \param[out] p_info : �������������Ϣ
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ��������
 */
aw_err_t aw_mp3_decode_stream_info_get (aw_mp3_decode_t            decode,
                                        struct aw_mp3_stream_info *p_info);

/**
 * \brief �ӽ��������л�ȡһ֡����
 *
 * \param[in]  decode      : ������ʵ��
 * \param[out] p_buffer    : ��Ž�����֡����
 * \param[in]  buffer_size : ��������С��һ���������Ϣ�е� out_samples
 * \param[out] p_frame_cnt : ���ۻ�����ʱ�䣬��������Ƶ����ʱ��
 *
 * \return > 0        : ���������֡����
 * \retval AW_ERROR   : �������
 * \retval -AW_EINVAL : ��������
 */
ssize_t aw_mp3_decode_stream_frame (aw_mp3_decode_t     decode,
                                    void               *p_buffer,
                                    uint16_t            buffer_size,
                                    struct aw_mp3_time *p_time);

/**
 * \brief �ƶ���������������ƫ��ʱ��
 * \note ͨ���ù���ʵ��MP3����Ŀ���ͺ���
 * \attention
 *  ��ƫ�Ƶ�λΪ���룬��������д�����ʵ��ƫ�����趨��ֵ��������
 *  ��ƫ��ֵС��һ֡�Ĳ���ʱ��ʱ�����������������κ�ƫ�ơ�
 *  һ֡�Ĳ���ʱ��(����)=ÿ֡��������samples_per_frame�� / ����Ƶ�ʣ�sample_rate�� * 1000
 *
 * \param[in] decode   : ������
 * \param[in] ms       : ƫ��ֵ(��λ������)
 * \param[in] whence   : SEEK_SET, ����ms��Ϊ�µĽ���λ��
 *                       SEEK_CUR, ��Ŀǰ�Ķ�дλ����������ms��λ����
 *                       SEEK_END, ����дλ��ָ���ļ�β��������ms��λ����
 *                       ��whenceֵΪSEEK_CUR��SEEK_ENDʱ������offset����ֵ�ĳ���
 *
 * \return ��ǰ���������������ͷ��ƫ��ֵ
 * \retval -1         : ƫ�ƴ���
 * \retval -AW_EINVAL : ��������
 */
aw_err_t aw_mp3_decode_stream_time (aw_mp3_decode_t decode,
                                    int             ms,
                                    int             whence);

/**
 * \brief �ر�MP3������
 *
 * \param[in] decode : ������
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ��������
 */
aw_err_t aw_mp3_decode_close (aw_mp3_decode_t decode);

/*******************************************************************************
 common
*******************************************************************************/

/**
 * \brief ��ȡMP3����汾��
 * \return �汾���ַ���
 */
const char *aw_mp3_version_get (void);

#ifdef __cplusplus
}
#endif

#endif /* __AW_MP3_DECODE_H */

/* end of file */
