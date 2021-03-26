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
 * - 1.00 20-10-13  win, first implementation.
 * \endinternal
 */

#ifndef ___AW_MP3_DEC_FRAME_H
#define ___AW_MP3_DEC_FRAME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_mp3_internal.h"

/**
 * \brief MPEG ID
 */
enum aw_mp3_dec_frame_id {
    AW_MP3_DEC_FRAME_ID_2_5     = 0,    /**< \brief MPEG 2.5 */
    AW_MP3_DEC_FRAME_ID_RESERVE = 1,    /**< \brief ���� */
    AW_MP3_DEC_FRAME_ID_2       = 2,    /**< \brief MPEG 2 */
    AW_MP3_DEC_FRAME_ID_1       = 3,    /**< \brief MPEG 1 */
};

/**
 * \brief MPEG Layer
 */
enum aw_mp3_dec_frame_layer {
    AW_MP3_DEC_FRAME_LAYER_I   = 1, /**< \brief Layer I */
    AW_MP3_DEC_FRAME_LAYER_II  = 2, /**< \brief Layer II */
    AW_MP3_DEC_FRAME_LAYER_III = 3, /**< \brief Layer III */
};

/**
 * \brief MPEGͨ��ģʽ
 */
enum aw_mp3_dec_frame_mode {

    /** \brief single channel */
    AW_MP3_DEC_FRAME_MODE_SINGLE_CHANNEL = 0,

    /** \brief dual channel */
    AW_MP3_DEC_FRAME_MODE_DUAL_CHANNEL   = 1,

    /** \brief joint (MS/intensity) stereo */
    AW_MP3_DEC_FRAME_MODE_JOINT_STEREO   = 2,

    /** \brief normal LR stereo */
    AW_MP3_DEC_FRAME_MODE_STEREO         = 3,

};

/**
 * \brief ǿ��ģʽ
 */
enum aw_mp3_dec_frame_emphasis {

    /** \brief no emphasis */
    AW_MP3_DEC_FRAME_EMPHASIS_NONE       = 0,

    /** \brief 50/15 microseconds emphasis */
    AW_MP3_DEC_FRAME_EMPHASIS_50_15_US   = 1,

    /** \brief unknown emphasis */
    AW_MP3_DEC_FRAME_EMPHASIS_RESERVED   = 2,

    /** \brief CCITT J.17 emphasis */
    AW_MP3_DEC_FRAME_EMPHASIS_CCITT_J_17 = 3,
};

/**
 * \brief MPEG֡ͷ��
 * \note 32λ��4�ֽڣ�ͷ����Ϣ
 * ��1
 * -----------------+----------------+------------------------------------------
 * name             | bit length     | description
 * -----------------+----------------+------------------------------------------
 * syncword         | 11(31-21)      | ����λ��Ϊ1
 * -----------------+----------------+------------------------------------------
 * ID               | 2(20,19)       | MPEG ��Ƶ�汾ID
 *                  |                | 00 - MPEG 2.5
 *                  |                | 01 - ����
 *                  |                | 10 - MPEG 2(ISO/IEC 13818-3)
 *                  |                | 11 - MPEG 1(ISO/IEC 11172-3)
 *                  |                | ע��MPEG 2.5���ǹٷ���׼��֡ͷ��20������������ʾ2.5�汾��
 *                  |                |   ��֧�ָð汾��Ӧ�ó���һ����Ϊ�ñ���Ϊ֡ͬ��λ��Ҳ����˵֡ͬ��
 *                  |                | (syncword)�ĳ���Ϊ12����������Ĺ涨��11������ID�ͱ����
 *                  |                | 1λ���Ƽ�ʹ�øñ�ķ�������Ϊ���������3���汾�����߼����ԡ�
 * -----------------+----------------+------------------------------------------
 * layer            | 2(18,17)       | Layer����:
 *                  |                | 00 - ����
 *                  |                | 01 - Layer III
 *                  |                | 10 - Layer II
 *                  |                | 11 - Layer I
 * -----------------+----------------+------------------------------------------
 * protection_bit   | 1(16)          | У��λ
 *                  |                | 0 - ����֡ͷ����16λ����CRCУ��
 *                  |                | 1 - ��У��
 * -----------------+----------------+------------------------------------------
 * bitrate_index    | 4(15-12)       | λ�����������2��ʾ
 * -----------------+----------------+------------------------------------------
 * sample_frequency | 2(11,10)       | ����Ƶ�ʣ���λ��Hz�������3��ʾ
 * -----------------+----------------+------------------------------------------
 *  padding_bit     | 1(9)           | ���λ
 *                  |                | 0 - �����
 *                  |                | 1 - ���
 *                  |                | ��������ﵽ��ȷ�ı����ʣ����磺128K 44.1kHz LayerII
 *                  |                | ʹ���˺ܶ�418bit��417bit����֡���ﵽ��ȷ��128k�����ʣ�
 *                  |                | LayerI�Ŀ�λ��32bit����LayerII��LayerIII�Ŀ�λ��
 *                  |                | 8bit����
 * -----------------+----------------+------------------------------------------
 *  private_bit     | 1(8)           | ˽��bit����������������Ӧ�á����������������Ӧ�ó���������¼�
 * -----------------+----------------+------------------------------------------
 *  mode            | 2(7,6)         | ����
 *                  |                | 00 - ������
 *                  |                | 01 - ��������������������
 *                  |                | 10 - ˫����
 *                  |                | 11 - ������
 *                  |                | ע��˫�����ļ��ɶ��������ĵ�������ɣ�ÿһ������ʹ�������ļ�һ���
 *                  |                | λ�ʡ�������Ľ��������������������������������������Ϣ����ȫ��ͬ
 *                  |                | �ģ�����������ͬ��
 * -----------------+----------------+------------------------------------------
 *  mode_extension  | 2(5,4)         | ��չģʽ����������������ʱ��Ч��
 *                  |                | ��չģʽ�������Ӷ�������Ч�����õ���Ϣ������������Ҫ����Դ�� ������
 *                  |                | λ������������ģʽ���б�������ָ̬��
 *                  |                | ������MPEG�ļ���Ƶ�����зֳ���32���Ӵ�����LayerI��
 *                  |                | LayerII��������λȷ��ǿ��������Ӧ�õ�Ƶ����
 *                  |                | LayerIII��������λȷ��Ӧ������һ��������������M/S
 *                  |                | stereo����Intensity stereo)Ƶ���ɽ�ѹ�㷨������
 *                  |                | ���4��ʾ��
 * -----------------+----------------+------------------------------------------
 *  copyright       | 1(3)           | ��Ȩ
 *                  |                | 0 - �ް�Ȩ
 *                  |                | 1 - �а�Ȩ
 * -----------------+----------------+------------------------------------------
 *  original/copy   | 1(2)           | ԭ��
 *                  |                | 0 - ����
 *                  |                | 1 - ԭ��
 * -----------------+----------------+------------------------------------------
 *  emphasis        | 2(1,0)         | ǿ��
 *                  |                | 00 - ��
 *                  |                | 01 - 50/15 ms
 *                  |                | 10 - ����
 *                  |                | 11 - CCIT J.17
 * -----------------+----------------+------------------------------------------
 *
 * ��2
 * ---------------+--------------------------------+--------------------------
 *  bitrate_index | MPEG 1                         | MPEG 2,2.5(LSF)
 *                +---------+----------+-----------+---------+----------------
 *                | Layer I | Layer II | Layer III | Layer I | Layer II & III
 * ---------------+---------+----------+-----------+---------+----------------
 *  0000          | Free
 * ---------------+---------+----------+-----------+---------+----------------
 *  0001          | 32      | 32       | 32        | 32      | 8
 * ---------------+---------+----------+-----------+---------+----------------
 *  0010          | 64      | 48       | 40        | 48      | 16
 * ---------------+---------+----------+-----------+---------+----------------
 *  0011          | 96      | 56       | 48        | 56      | 24
 * ---------------+---------+----------+-----------+---------+----------------
 *  0100          | 128     | 64       | 56        | 64      | 32
 * ---------------+---------+----------+-----------+---------+----------------
 *  0101          | 160     | 80       | 64        | 80      | 40
 * ---------------+---------+----------+-----------+---------+----------------
 *  0110          | 192     | 96       | 80        | 96      | 48
 * ---------------+---------+----------+-----------+---------+----------------
 *  0111          | 224     | 112      | 96        | 112     | 56
 * ---------------+---------+----------+-----------+---------+----------------
 *  1000          | 256     | 128      | 112       | 128     | 64
 * ---------------+---------+----------+-----------+---------+----------------
 *  1001          | 288     | 160      | 128       | 144     | 80
 * ---------------+---------+----------+-----------+---------+----------------
 *  1010          | 320     | 192      | 160       | 160     | 96
 * ---------------+---------+----------+-----------+---------+----------------
 *  1011          | 352     | 224      | 192       | 176     | 112
 * ---------------+---------+----------+-----------+---------+----------------
 *  1100          | 384     | 256      | 224       | 192     | 128
 * ---------------+---------+----------+-----------+---------+----------------
 *  1101          | 416     | 320      | 224       | 192     | 128
 * ---------------+---------+----------+-----------+---------+----------------
 *  1110          | 448     | 384      | 320       | 256     | 160
 * ---------------+---------+----------+-----------+---------+----------------
 *  1111          | Forbidden
 * ---------------+-----------------------------------------------------------
 * ע����λkbps��1kbit=1000bit
 *
 * Free��ʾ���У�����̶������ʣ������ļ����ܱ任�����ʣ����ϱ���Ĳ�ͬ��Ӧ����Ӧ�ó�����������������ʵ��Ӧ��ֻ�����ڲ�Ŀ��
 * ����Ϊ������Ӧ�ó������޷��ҳ���ȷ�ı����ʡ�Forbidden��ʾ��ֵ��Ч��
 * MPEG�ļ�������VBR����ʾ�ļ��ı����ʿ��Ա仯��
 * �����ʱ任��bitrate switching��: ÿһ֡�������ɲ�ͬ�ı����ʡ�����Ӧ�����κβ㡣
 * Layer III ����������֧�ָ÷�����Layer I��Layer II����ѡ��֧�֡�
 *
 * ���س�(bit reservoir)�������ʿ���ʹ�ô�ǰ���֡�н����ģ����ޣ����Ա����ڳ��ռ������������źŲ��֣�Ȼ�������͵��¸�֡
 * ֮�䲻���໥��������ζ�Ų������ָ��ļ������ַ���ֻ��LayerIII֧�֡�
 *
 * LayerII����һЩ����������������ģʽ���±�����������
 * ��2.1
 * ---------+-------------------------------------------
 *  bitrate | allowed modes
 * ---------+-------------------------------------------
 *  free    | all
 * ---------+-------------------------------------------
 *  32      | single channel
 * ---------+-------------------------------------------
 *  48      | single channel
 * ---------+-------------------------------------------
 *  56      | single channel
 * ---------+-------------------------------------------
 *  64      | all
 * ---------+-------------------------------------------
 *  80      | single channel
 * ---------+-------------------------------------------
 *  90      | all
 * ---------+-------------------------------------------
 *  112     | all
 * ---------+-------------------------------------------
 *  128     | all
 * ---------+-------------------------------------------
 *  160     | all
 * ---------+-------------------------------------------
 *  192     | all
 * ---------+-------------------------------------------
 *  224     | stereo, intensity stereo, dual channel
 * ---------+-------------------------------------------
 *  256     | stereo, intensity stereo, dual channel
 * ---------+-------------------------------------------
 *  320     | stereo, intensity stereo, dual channel
 * ---------+-------------------------------------------
 *  384     | stereo, intensity stereo, dual channel
 * ---------+-------------------------------------------
 *
 * ����Ƶ�ʣ���λ��Hz��
 * ��3
 * ---------+---------+----------+----------
 *  bits    | MPEG1   |  MPEG2   | MPEG2.5
 * ---------+---------+----------+----------
 *  00      | 44100   | 22050    | 11025
 * ---------+---------+----------+----------
 *  01      | 48000   | 24000    | 12000
 * ---------+---------+----------+----------
 *  10      | 32000   | 16000    | 8000
 * ---------+---------+----------+----------
 *  11      | reserved
 * ---------+-------------------------------
 *
 * ��չģʽ����������������ʱ��Ч��
 * ��4
 * ------+----------------+-------------------------------
 *  bits | Layer I & II   | Layer III
 *       |                +------------+------------------
 *       |                | M/S stereo | Intensity stereo
 * ------+----------------+------------+------------------
 *  00   | bands 4 to 31  | off        | off
 * ------+----------------+------------+------------------
 *  01   | bands 8 to 31  | off        | on
 * ------+----------------+------------+------------------
 *  10   | bands 12 to 31 | on         | off
 * ------+----------------+------------+------------------
 *  11   | bands 16 to 31 | on         | on
 * ------+----------------+------------+------------------
 *
 * ֡������
 * ��5
 * ------------+-------------+-------------+---------------
 *             | MPEG1       | MPEG2(LSF)  | MPEG2.5(LSF)
 * ------------+-------------+-------------+---------------
 *  Layer I    | 384         | 384         | 384
 * ------------+-------------+-------------+---------------
 *  Lyaer II   | 1152        | 1152        | 1152
 * ------------+-------------+-------------+---------------
 *  Lyaer III  | 1152        | 576         | 576
 * ------------+-------------+-------------+---------------
 *
 * LayerI:
 * ֡���ȣ��ֽڣ�=��ÿ֡������  / 8 * �����ʣ� / ����Ƶ�� + ��� * 4
 * LayerII&LayerIII
 * ֡���ȣ��ֽڣ�=��ÿ֡������ / 8 * �����ʣ� / ����Ƶ�� + ���
 *
 * ÿ֡����ʱ�䣨���룩=ÿ֡������ / ����Ƶ�� * 1000
 */
struct aw_mp3_dec_frame_header {

    /** \brief audio layer (1, 2, or 3) */
    enum aw_mp3_dec_frame_layer    layer;

    /** \brief audio layer (1, 2, or 3) */
    enum aw_mp3_dec_frame_mode     mode;

    /** \brief additional mode info */
    int                            mode_extension;

    /** \brief de-emphasis to use (see above) */
    enum aw_mp3_dec_frame_emphasis emphasis;

    /** \brief bitrate bps */
    uint32_t                       bit_rate;

    /** \brief sample frequency */
    uint32_t                       sample_rate;

    /** \brief frame CRC accumulator */
    uint16_t                       crc_check;

    /** \brief final target CRC checksum */
    uint16_t                       crc_target;

    /** \brief flags (see below) */
    int                            flags;

    /** \brief private bits (see below) */
    int                            private_bits;

    /** \brief audio playing time of frame */
    struct aw_mp3_time             duration;
};

/**
 * \brief MPEG ֡
 */
struct aw_mp3_dec_frame {

    /** \brief MPEG audio header */
    struct aw_mp3_dec_frame_header header;

    /** \brief decoding options (from stream) */
    int                            options;

    /** \brief synthesis subband filter samples */
    int32_t                        sbsample[2][36][32];

    /** \brief Layer III block overlap data */
    int32_t                        overlap[2][32][18];

};

#define AW_MP3_DEC_FRAME_NCHANNELS(p_header)   ((p_header)->mode ? 2 : 1)

#define AW_MP3_DEC_FRAME_NSBSAMPLES(p_header)  \
    ((p_header)->layer == AW_MP3_DEC_FRAME_LAYER_I ? 12 :  \
     (((p_header)->layer == AW_MP3_DEC_FRAME_LAYER_III &&  \
       ((p_header)->flags & AW_MP3_DEC_FRAME_FLAG_LSF_EXT)) ? 18 : 36))

/**
 * \brief frame flag
 */
enum aw_mp3_dec_frame_flag {

    /** \brief number of Layer III private bits */
    AW_MP3_DEC_FRAME_FLAG_NPRIVATE_III = 0x0007,

    /** \brief header but not data is decoded */
    AW_MP3_DEC_FRAME_FLAG_INCOMPLETE   = 0x0008,

    /** \brief frame has CRC protection */
    AW_MP3_DEC_FRAME_FLAG_PROTECTION   = 0x0010,

    /** \brief frame is copyright */
    AW_MP3_DEC_FRAME_FLAG_COPYRIGHT    = 0x0020,

    /** \brief frame is original (else copy) */
    AW_MP3_DEC_FRAME_FLAG_ORIGINAL     = 0x0040,

    /** \brief frame has additional slot */
    AW_MP3_DEC_FRAME_FLAG_PADDING      = 0x0080,

    /** \brief uses intensity joint stereo */
    AW_MP3_DEC_FRAME_FLAG_I_STEREO     = 0x0100,

    /** \brief uses middle/side joint stereo */
    AW_MP3_DEC_FRAME_FLAG_MS_STEREO    = 0x0200,

    /** \brief uses free format bitrate */
    AW_MP3_DEC_FRAME_FLAG_FREEFORMAT   = 0x0400,

    /** \brief lower sampling freq. extension */
    AW_MP3_DEC_FRAME_FLAG_LSF_EXT      = 0x1000,

    /** \brief multichannel audio extension */
    AW_MP3_DEC_FRAME_FLAG_MC_EXT       = 0x2000,

    /** \brief MPEG 2.5 (unofficial) extension */
    AW_MP3_DEC_FRAME_FLAG_MPEG_2_5_EXT = 0x4000
};

/**
 * \brief private
 */
enum aw_mp3_dec_frame_private {

    /** \brief header private bit */
    AW_MP3_DEC_FRAME_PRIVATE_HEADER  = 0x0100,

    /** \brief Layer III private bits (up to 5) */
    AW_MP3_DEC_FRAME_PRIVATE_III     = 0x001f
};

/**
 * \brief ��ʼ��֡ͷ��
 * \param[in] p_header : ֡ͷ���ṹ��
 * \return ��
 */
void aw_mp3_dec_frame_header_init (struct aw_mp3_dec_frame_header *p_header);

/**
 * \brief ֡ͷ������
 * \param[in] p_header : ֡ͷ���ṹ��
 * \return AW_OK��ʾ�ɹ�������ֵ��ʾ����
 */
int aw_mp3_dec_frame_header_decode (struct aw_mp3_dec_frame_header *p_header,
                                    struct aw_mp3_dec_stream       *p_stream);

/**
 * \brief MPEG ֡��ʼ��
 * \param[in] p_frame : ֡�ṹ��
 * \return ��
 */
void aw_mp3_dec_frame_init (struct aw_mp3_dec_frame *p_frame);

/**
 * \brief ֡����
 *
 * \param[in] p_frame  : ֡�ṹ��
 * \param[in] p_stream : ���ṹ��
 *
 * \return ��
 */
aw_err_t aw_mp3_dec_frame_decode (struct aw_mp3_dec_frame  *p_frame,
                                  struct aw_mp3_dec_stream *p_stream);

/**
 * \brief ������ǰ֡
 * \param[in] p_frame : ֡�ṹ��
 * \return ��
 */
void aw_mp3_dec_frame_mute (struct aw_mp3_dec_frame *p_frame);

/**
 * \brief ��ȡͨ������
 * \param[in] p_frame : ֡�ṹ��
 * \return ͨ������
 */
uint8_t
aw_mp3_dec_frame_channel_get (const struct aw_mp3_dec_frame_header *p_header);

/**
 * \brief synthesis subband filter samples
 */
uint8_t
aw_mp3_dec_frame_sbsamples_get (const struct aw_mp3_dec_frame_header *p_header);

#ifdef __cplusplus
}
#endif

#endif /* ___AW_MP3_DEC_FRAME_H */

/* end of file */
